/************************************************************************************

Filename    :   OVR_CAPI_GL.h
Content     :   GL specific structures used by the CAPI interface.
Created     :   November 7, 2013
Authors     :   Lee Cooper

Copyright   :   Copyright 2013 Oculus VR, Inc. All Rights reserved.

Use of this software is subject to the terms of the Oculus Inc license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

************************************************************************************/
#ifndef OVR_CAPI_GL_h
#define OVR_CAPI_GL_h

#include "OVR_CAPI.h"

//-----------------------------------------------------------------------------------
// ***** GL Specific

#if defined(OVR_OS_WIN32)
    #include <Windows.h>
    #include <GL/gl.h>
    #include <GL/glext.h>
    #include <GL/wglext.h>
#elif defined(OVR_OS_MAC)
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
    #include <OpenGL/OpenGL.h>
#else
    #include <GL/gl.h>
    #include <GL/glext.h>
    #include <GL/glx.h>
#endif


// Used to configure slave GL rendering (i.e. for devices created externally).
typedef struct ovrGLConfigData_s
{
    // General device settings.
    ovrRenderAPIConfigHeader Header;

#if defined(OVR_OS_WIN32)
    HWND   Window;
#elif defined(OVR_OS_LINUX)
    Display* Disp;
    Window  Win;
#endif
} ovrGLConfigData;

union ovrGLConfig
{
    ovrRenderAPIConfig Config;
    ovrGLConfigData OGL;
};

// Used to pass GL eye texture data to ovrHmd_EndFrame.
typedef struct ovrGLTextureData_s
{
    // General device settings.
    ovrTextureHeader          Header;
    GLuint           TexId;       
} ovrGLTextureData;

typedef union ovrGLTexture_s
{
    ovrTexture          Texture;
    ovrGLTextureData	OGL;
} ovrGLTexture;

#endif	// OVR_CAPI_GL_h
