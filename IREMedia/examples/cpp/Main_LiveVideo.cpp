
#include "Common.h"
#include "OVR_CAPI_GL.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace cv;
using namespace std;

struct EyeArgs {
	glm::mat4 projection;
	glm::mat4 viewOffset;
	gl::FrameBufferWrapper framebuffer;
	ovrGLTexture textures;
};

class HelloRift : public RiftGlfwApp {
protected:
#define CAM_IMAGE_WIDTH 1280.f
#define CAM_IMAGE_HEIGHT 720.f
#define RENDER_IMAGE_WIDTH 900.f
#define RENDER_IMAGE_HEIGHT 720.f
#define M_PI 3.14159265358979323846

	bool				terminateApp = false;
	gl::ProgramPtr		texturedPtr;
	gl::GeometryPtr     quadGeom;
	gl::Texture2dPtr	imageTextures[2];
	EyeArgs				perEyeArgs[2];
	IplImage			*perEyeWriteImage[2];
	IplImage			*perEyeReadImage[2];
	mutex				camMutex;
	condition_variable	camFinishCondition;
	volatile bool		camFinish;
	condition_variable	drawFinishCondition;
	volatile bool		drawFinish;

public:
	~HelloRift() {
		ovrHmd_Destroy(hmd);
	}

	void initGl() {
		typedef gl::Texture < GL_TEXTURE_2D >
			Texture;
		typedef std::shared_ptr < gl::Texture<GL_TEXTURE_2D> >
			TexturePtr;
		texturedPtr = GlUtils::getProgram(Resource::SHADERS_TEXTURED_VS, Resource::SHADERS_TEXTURED_FS);
		quadGeom = GlUtils::getQuadGeometry(RENDER_IMAGE_WIDTH / RENDER_IMAGE_HEIGHT, 3.f);
		GlfwApp::initGl();
		ovrFovPort eyeFovPorts[2];

		for_each_eye([&](ovrEyeType eye){
			EyeArgs & eyeArgs = perEyeArgs[eye];
			ovrTextureHeader & eyeTextureHeader = eyeArgs.textures.OGL.Header;
			eyeFovPorts[eye] = hmdDesc.DefaultEyeFov[eye];
			eyeTextureHeader.TextureSize = ovrHmd_GetFovTextureSize(hmd, eye, hmdDesc.DefaultEyeFov[eye], 1.0f);
			eyeTextureHeader.RenderViewport.Size = eyeTextureHeader.TextureSize;
			eyeTextureHeader.RenderViewport.Pos.x = 0;
			eyeTextureHeader.RenderViewport.Pos.y = 0;
			eyeTextureHeader.API = ovrRenderAPI_OpenGL;

			imageTextures[eye] = TexturePtr(new Texture());
			imageTextures[eye]->bind();
			imageTextures[eye]->parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			imageTextures[eye]->parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RENDER_IMAGE_WIDTH, RENDER_IMAGE_HEIGHT, 0, GL_BGR, GL_UNSIGNED_BYTE, 0x0000);

			perEyeReadImage[eye] = cvCreateImage(cvSize(CAM_IMAGE_WIDTH, CAM_IMAGE_HEIGHT), 8, 3);
			perEyeWriteImage[eye] = cvCreateImage(cvSize(CAM_IMAGE_WIDTH, CAM_IMAGE_HEIGHT), 8, 3);

			eyeArgs.framebuffer.init(Rift::fromOvr(eyeTextureHeader.TextureSize));
			eyeArgs.textures.OGL.TexId = eyeArgs.framebuffer.color->texture;
		});

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		ovrGLConfig cfg;
		cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
		cfg.OGL.Header.RTSize = hmdDesc.Resolution;
		cfg.OGL.Header.Multisample = 1;

#ifdef OVR_OS_WINDOWS
		cfg.OGL.Window = 0;
#elif defined(OVR_OS_LINUX)
		cfg.OGL.Disp = 0;
		cfg.OGL.Win = 0;
#endif

		int distortionCaps = ovrDistortionCap_TimeWarp | ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette | ovrDistortionCap_NoSwapBuffers;

		ovrEyeRenderDesc eyeRenderDescs[2];
		int configResult = ovrHmd_ConfigureRendering(hmd, &cfg.Config,
			distortionCaps, eyeFovPorts, eyeRenderDescs);

		for_each_eye([&](ovrEyeType eye){
			EyeArgs & eyeArgs = perEyeArgs[eye];
			eyeArgs.projection = Rift::fromOvr(
				ovrMatrix4f_Projection(eyeFovPorts[eye], 0.01, 100, true));
		});

		thread cameraThread;
		cameraThread = thread(&HelloRift::updateCameraImages, this);

		cameraThread.detach();
	}

	void updateCameraImages() {
		CvCapture* cam = cvCaptureFromCAM(701);
		cvSetCaptureProperty(cam, CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
		cvSetCaptureProperty(cam, CV_CAP_PROP_FRAME_WIDTH, CAM_IMAGE_WIDTH);
		cvSetCaptureProperty(cam, CV_CAP_PROP_FRAME_HEIGHT, CAM_IMAGE_HEIGHT);

		CvCapture* cam2 = cvCaptureFromCAM(700);
		cvSetCaptureProperty(cam2, CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
		cvSetCaptureProperty(cam2, CV_CAP_PROP_FRAME_WIDTH, CAM_IMAGE_WIDTH);
		cvSetCaptureProperty(cam2, CV_CAP_PROP_FRAME_HEIGHT, CAM_IMAGE_HEIGHT);

		IplImage *image;
		IplImage *image2;
		float         fps = 0.f;
		int framecount = 0;
		long start = Platform::elapsedMillis();

		while (!terminateApp) {
			// wait on draw thread
				{
					unique_lock<mutex> lck(camMutex);
					while (!drawFinish && !terminateApp) { drawFinishCondition.wait(lck); }
				}

			image = cvQueryFrame(cam);
			if (image) {
				memcpy(perEyeWriteImage[0]->imageData, image->imageData, CAM_IMAGE_HEIGHT * CAM_IMAGE_WIDTH * 3);
			}
			else{
				SAY("Didn't get image of cam 1");
			}

			image2 = cvQueryFrame(cam2);
			if (image2) {
				memcpy(perEyeWriteImage[1]->imageData, image2->imageData, CAM_IMAGE_HEIGHT * CAM_IMAGE_WIDTH * 3);
			}
			else {
				SAY("Didn't get image of cam 2");
			}

			long now = Platform::elapsedMillis();
			++framecount;
			if ((now - start) >= 2000) {
				float elapsed = (now - start) / 1000.f;
				fps = (float)framecount / elapsed;
				SAY("FPS cams: %0.2f\n", fps);
				start = now;
				framecount = 0;
			}

			{
				unique_lock<mutex> lck(camMutex);
				drawFinish = false;
				camFinish = true;
				camFinishCondition.notify_all();
			}

		}

		cvReleaseCapture(&cam);
	}


	virtual void update() {
		static const glm::vec3 EYE = glm::vec3(0, 0, 1);
		static const glm::vec3 LOOKAT = glm::vec3(0, 0, 0);
		gl::Stacks::modelview().top() = glm::lookAt(EYE, LOOKAT, GlUtils::UP);
	}

	void draw() {
		static int frameIndex = 0;
		ovrHmd_BeginFrame(hmd, frameIndex++);
		short textureSwitch = 0;// frameIndex % 2;

		// wait for camera thread
		{
			unique_lock<mutex> lck(camMutex);

			while (!camFinish){
				camFinishCondition.wait(lck);
			}
		}

		// swap image pointers
		for (int i = 0; i < 2; ++i)
		{
			IplImage* tempImage = perEyeReadImage[i];
			perEyeReadImage[i] = perEyeWriteImage[i];
			perEyeWriteImage[i] = tempImage;
		}

		// notify camera thread to resume
		{
			unique_lock<mutex> lck(camMutex);
			camFinish = false;
			drawFinish = true;
			drawFinishCondition.notify_all();
		}

		for (int i = 0; i < 2; ++i)
		{
			ovrEyeType eye = hmdDesc.EyeRenderOrder[i];

			if (perEyeReadImage[eye] && !terminateApp) {
				EyeArgs & eyeArgs = perEyeArgs[eye];
				gl::Stacks::projection().top() = eyeArgs.projection;
				gl::MatrixStack & mv = gl::Stacks::modelview();

				eyeArgs.framebuffer.activate();
				ovrPosef renderPose = ovrHmd_BeginEyeRender(hmd, eye);
				mv.withPush([&]{
					glClear(GL_DEPTH_BUFFER_BIT);
					//GlUtils::renderFloorGrid(glm::mat4());
					//GlUtils::renderSkybox(Resource::IMAGES_SKY_CITY_XNEG_PNG);

					if (ovrEye_Left == eye) {
						mv.rotate(M_PI / 2, glm::vec3(0, 0, 1));
					}
					else {
						mv.rotate(-M_PI / 2, glm::vec3(0, 0, 1));
					}

					mv.scale(1);
					mv.rotate(M_PI, glm::vec3(0, 1, 0));

					glPixelStorei(GL_UNPACK_ROW_LENGTH, CAM_IMAGE_WIDTH);
					glPixelStorei(GL_UNPACK_SKIP_PIXELS, 189);

					imageTextures[eye]->bind();
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, RENDER_IMAGE_WIDTH, RENDER_IMAGE_HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, perEyeReadImage[eye]->imageData);

					glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
					glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

					GlUtils::renderGeometry(quadGeom, texturedPtr);
				});

				ovrHmd_EndEyeRender(hmd, eye, renderPose, &perEyeArgs[eye].textures.Texture);
			}
			//eyeArgs.framebuffer.deactivate();
		};
		ovrHmd_EndFrame(hmd);
	}

	void onKey(int key, int scancode, int action, int mods) {
		if (GLFW_PRESS != action) {
			return;
		}

		switch (key) {
		case GLFW_KEY_F12:
			GlfwApp::screenshot();
			break;
		case GLFW_KEY_R:
			ovrHmd_ResetSensor(hmd);
			break;

		case GLFW_KEY_ESCAPE:
			terminateApp = true;
			Platform::sleepMillis(3000);
			//break;

		default:
			GlfwApp::onKey(key, scancode, action, mods);
			break;
		}
	}
};

RUN_OVR_APP(HelloRift);
