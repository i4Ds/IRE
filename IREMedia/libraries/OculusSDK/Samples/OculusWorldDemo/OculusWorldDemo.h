/************************************************************************************

Filename    :   OculusWorldDemo.h
Content     :   First-person view test application for Oculus Rift - Header file
Created     :   October 4, 2012
Authors     :   Michael Antonov, Andrew Reisse, Steve LaValle, Dov Katz
				Peter Hoff, Dan Goodman, Bryan Croteau                

Copyright   :   Copyright 2012 Oculus VR, Inc. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*************************************************************************************/

#ifndef INC_OculusWorldDemo_h
#define INC_OculusWorldDemo_h

#include "OVR.h"

#include "../CommonSrc/Platform/Platform_Default.h"
#include "../CommonSrc/Render/Render_Device.h"
#include "../CommonSrc/Render/Render_XmlSceneLoader.h"
#include "../CommonSrc/Platform/Gamepad.h"
#include "../CommonSrc/Util/OptionMenu.h"
#include "../CommonSrc/Util/RenderProfiler.h"

#include "Util/Util_Render_Stereo.h"
using namespace OVR::Util::Render;

#include <Kernel/OVR_SysFile.h>
#include <Kernel/OVR_Log.h>
#include <Kernel/OVR_Timer.h>

#include "Player.h"
#include "OVR_DeviceConstants.h"

// Filename to be loaded by default, searching specified paths.
#define WORLDDEMO_ASSET_FILE  "Tuscany.xml"

#define WORLDDEMO_ASSET_PATH1 "Assets/Tuscany/"
#define WORLDDEMO_ASSET_PATH2 "../../../Assets/Tuscany/"
// This path allows the shortcut to work.
#define WORLDDEMO_ASSET_PATH3 "Samples/OculusWorldDemo/Assets/Tuscany/"

using namespace OVR;
using namespace OVR::Platform;
using namespace OVR::Render;


//-------------------------------------------------------------------------------------
// ***** OculusWorldDemo Description

// This app renders a loaded scene allowing the user to move along the
// floor and look around with an HMD, mouse and keyboard. The following keys work:
//
//  'W', 'S', 'A', 'D' and Arrow Keys - Move forward, back; strafe left/right.
//
//  Space - Bring up status and help display.
//  Tab   - Bring up/hide menu with editable options. 
//  F4    - Toggle MSAA.
//  F9    - Cycle through fullscreen and windowed modes.
//          Necessary for previewing content with Rift.
//
// Important Oculus-specific logic can be found at following locations:
//
//  OculusWorldDemoApp::OnStartup - This function will initialize the SDK, creating the Hmd
//									and delegating to CalculateHmdValues to initialize it.
//
//  OculusWorldDemoApp::OnIdle    - Here we poll SensorFusion for orientation, apply it
//									to the scene and handle movement.
//									Stereo rendering is also done here, by delegating to
//									to the RenderEyeView() function for each eye.
//

//-------------------------------------------------------------------------------------
// ***** OculusWorldDemo Application class

// An instance of this class is created on application startup (main/WinMain).
// It then works as follows:
//  - Graphics and HMD setup is done OculusWorldDemoApp::OnStartup(). Much of
//    HMD configuration here is moved to CalculateHmdValues.
//    OnStartup also creates the room model from Slab declarations.
//    
//  - Per-frame processing is done in OnIdle(). This function processes
//    sensor and movement input and then renders the frame.
//
//  - Additional input processing is done in OnMouse, OnKey.

class OculusWorldDemoApp : public Application
{
public:
    OculusWorldDemoApp();
    ~OculusWorldDemoApp();

    virtual int  OnStartup(int argc, const char** argv);
    virtual void OnIdle();

    virtual void OnMouseMove(int x, int y, int modifiers);
    virtual void OnKey(OVR::KeyCode key, int chr, bool down, int modifiers);
    virtual void OnResize(int width, int height);

    bool         SetupWindowAndRendering(int argc, const char** argv);
    
    // Adds room model to scene.
    void         InitMainFilePath();
    void         PopulateScene(const char* fileName);
    void         PopulatePreloadScene();
    void		 ClearScene();
    void         PopulateOptionMenu();


    // Computes all of the Hmd values and configures render targets.
    void         CalculateHmdValues();
    // Returns the actual size present.
    Sizei        EnsureRendertargetAtLeastThisBig (int rtNum, Sizei size);


    // Renders full stereo scene for one eye.
    void         RenderEyeView(ovrEyeType eye);
    // Renderes HUD overlay brough up by spacebar; 2D viewport must be set before call.
    void         RenderTextInfoHud(float textHeight);
    void         RenderAnimatedBlocks(ovrEyeType eye, double appTime);
    void         RenderGrid(ovrEyeType eye);
    
    Matrix4f     CalculateViewFromPose(const Transformf& pose);

    // Determine whether this frame needs rendering based on timewarp timing and flags.
    bool        FrameNeedsRendering(double curtime);
    void        ApplyDynamicResolutionScaling();
    void        UpdateFrameRateCounter(double curtime);


    // Model creation and misc functions.
    Model*      CreateModel(Vector3f pos, struct SlabModel* sm);
    Model*      CreateBoundingModel(CollisionModel &cm);
    void        ChangeDisplay ( bool bBackToWindowed, bool bNextFullscreen, bool bFullWindowDebugging );
    void        GamepadStateChanged(const GamepadState& pad);    

    // Processes DeviceNotificationStatus queue to handles plug/unplug.
    void         ProcessDeviceNotificationQueue();


    // ***** Callbacks for Menu option changes

    // These contain extra actions to be taken in addition to switching the state.
    void HmdSettingChange(OptionVar* = 0)   { HmdSettingsChanged = true; }
    void BlockShowChange(OptionVar* = 0)    { BlocksCenter = ThePlayer.BodyPos; }
    void EyeHeightChange(OptionVar* = 0)    { ThePlayer.BodyPos.y = ThePlayer.UserEyeHeight; }

    void HmdSettingChangeFreeRTs(OptionVar* = 0);
    void MultisampleChange(OptionVar* = 0);
    void CenterPupilDepthChange(OptionVar* = 0);
    void DistortionClearColorChange(OptionVar* = 0);


protected:
    RenderDevice*       pRender;
    RendererParams      RenderParams;
    Sizei               WindowSize;
    int                 ScreenNumber;
    int                 FirstScreenInCycle;

    struct RenderTarget
    {
        Ptr<Texture>     pTex;
        ovrTexture       Tex;
    };
    enum RendertargetsEnum
    {
        Rendertarget_Left,
        Rendertarget_Right,
        Rendertarget_BothEyes,    // Used when both eyes are rendered to the same target.
        Rendertarget_LAST
    };
    RenderTarget        RenderTargets[Rendertarget_LAST];


    // ***** Oculus HMD Variables

    ovrHmd              Hmd;
    ovrHmdDesc          HmdDesc;
    ovrEyeRenderDesc    EyeRenderDesc[2];
    Matrix4f            Projection[2];          // Projection matrix for eye.
    Matrix4f            OrthoProjection[2];     // Projection for 2D.
    ovrTexture          EyeTexture[2];
    Sizei               EyeRenderSize[2];       // Saved render eye sizes; base for dynamic sizing.
    // Sensor caps applied.
    unsigned            StartSensorCaps;    
    bool                UsingDebugHmd;

    // Frame timing logic.
    enum { SecondsOfFpsMeasurement = 1 };
    int                 FrameCounter;
    double              NextFPSUpdate;	
    float               SecondsPerFrame;
    float               FPS;
    double              LastFpsUpdate;

    // Times a single frame.
    double              LastUpdate;         
  
    // Loaded data.
    String	                    MainFilePath;
    Array<Ptr<CollisionModel> > CollisionModels;
    Array<Ptr<CollisionModel> > GroundCollisionModels;

    // Loading process displays screenshot in first frame
    // and then proceeds to load until finished.
    enum LoadingStateType
    {
        LoadingState_Frame0,
        LoadingState_DoLoad,
        LoadingState_Finished
    } LoadingState;

    // Set when vision tracking is detected.
    bool                HaveVisionTracking;
    
    GamepadState        LastGamepadState;

    Player				ThePlayer;
    Matrix4f            View;
    Scene               MainScene;
    Scene               LoadingScene;
    Scene               SmallGreenCube;

	Scene				OculusCubesScene;
	Scene               RedCubesScene;
	Scene				BlueCubesScene;

    // Last frame asn sensor data reported by BeginFrame().
    ovrFrameTiming      HmdFrameTiming;
    unsigned            HmdStatus;


    // ***** Modifiable Menu Options 

    // This flag is set when HMD settings change, causing HMD to be re-initialized.
    bool                HmdSettingsChanged;

    // Render Target - affecting state.
    bool                RendertargetIsSharedByBothEyes;
    bool                DynamicRezScalingEnabled;
    bool                ForceZeroIpd;
    float               DesiredPixelDensity;    
    float               FovSideTanMax;
    float               FovSideTanLimit; // Limit value for Fov.    
    // Time-warp.
    bool                TimewarpEnabled;
    float               TimewarpRenderIntervalInSeconds;    
    bool                FreezeEyeUpdate;
    bool                FreezeEyeOneFrameRendered;

    // Other global settings.
    float               CenterPupilDepthMeters;
    // float               IPD;
    bool                ForceZeroHeadMovement;
    bool                VsyncEnabled;
    bool                MultisampleEnabled;
    // DK2 only
    bool                IsLowPersistence;
    bool                DynamicPrediction;
    bool                PositionTrackingEnabled;

    // Support toggling background color for distortion so that we can see
    // the effect on the periphery.
    int                 DistortionClearBlue;

    // Stereo settings adjustment state.
    bool                ShiftDown;
    bool                CtrlDown;


    // ***** Scene Rendering Modes

    enum SceneRenderMode
    {
        Scene_World,
		Scene_Cubes,
		Scene_OculusCubes
    };
    SceneRenderMode    SceneMode;

    enum GridDispayModeType
    {
        GridDisplay_None,
        GridDisplay_GridOnly,
        GridDisplay_GridAndScene
    };
    GridDispayModeType  GridDisplayMode;

    // What type of grid to display.
    enum GridModeType
    {
        Grid_Rendertarget4,
        Grid_Rendertarget16,
        Grid_Lens,
        Grid_Last
    };
    GridModeType       GridMode;

    // What help screen we display, brought up by 'Spacebar'.
    enum TextScreen
    {
        Text_None,
        Text_Info,
        Text_Timing,        
        Text_Help1,
        Text_Help2,
        Text_Count
    };
    TextScreen          TextScreen;

    // Whether we are displaying animated blocks and what type.
    int                 BlocksShowType;
    Vector3f            BlocksCenter;


    // User configurable options, brought up by 'Tab' key.
    // Also handles shortcuts and pop-up overlay messages.
    OptionSelectionMenu Menu;

    // Profiler for rendering - displays timing stats.
    RenderProfiler      Profiler;
};



#endif // INC_OculusWorldDemo_h
