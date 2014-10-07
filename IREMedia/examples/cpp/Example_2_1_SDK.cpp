#include "Common.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

class Example_SdkInit {
protected:
	float         fps;
public:

	virtual int run() {
		/*SAY("Initializing SDK");
		ovr_Initialize();

		SAY("De-initializing the SDK");
		ovr_Shutdown();

		SAY("Exiting");*/

		CvCapture* cam = cvCaptureFromCAM(700);
		//cvSetCaptureProperty(cam, CV_CAP_PROP_FPS, 60);
		//cvSetCaptureProperty(cam, CV_CAP_PROP_FRAME_WIDTH, 1280);
		//cvSetCaptureProperty(cam, CV_CAP_PROP_FRAME_HEIGHT, 720);
		cvSetCaptureProperty(cam, CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
		cvSetCaptureProperty(cam, CV_CAP_PROP_FRAME_WIDTH, 1280);
		cvSetCaptureProperty(cam, CV_CAP_PROP_FRAME_HEIGHT, 720);

		CvCapture* cam2 = cvCaptureFromCAM(701);
		//cvSetCaptureProperty(cam2, CV_CAP_PROP_FPS, 60);
		//cvSetCaptureProperty(cam2, CV_CAP_PROP_FRAME_WIDTH, 1280);
		//cvSetCaptureProperty(cam2, CV_CAP_PROP_FRAME_HEIGHT, 720);
		cvSetCaptureProperty(cam2, CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
		cvSetCaptureProperty(cam2, CV_CAP_PROP_FRAME_WIDTH, 1280);
		cvSetCaptureProperty(cam2, CV_CAP_PROP_FRAME_HEIGHT, 720);

		int framecount = 0;
		long start = Platform::elapsedMillis();
		IplImage* image;
		IplImage* iamgeTwo;
		while (true) {
			image = cvQueryFrame(cam);
			iamgeTwo = cvQueryFrame(cam2);

			//cvShowImage("One", image);
			//cvShowImage("Two", iamgeTwo);

			long now = Platform::elapsedMillis();
			++framecount;
			if ((now - start) >= 2000) {
				float elapsed = (now - start) / 1000.f;
				fps = (float)framecount / elapsed;
				SAY("FPS: %0.2f\n", fps);
				start = now;
				framecount = 0;
			}

			//cvWaitKey(1);
			//Sleep(100);
		}

		return 0;
	}
};

RUN_APP(Example_SdkInit);
