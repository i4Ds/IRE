#pragma once

#include <windows.h>
//#ifdef USE_DIRECTX_SDK
//#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\include\xinput.h>
//#pragma comment(lib,"xinput.lib")
//#elif (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
//#include <XInput.h>
//#pragma comment(lib,"xinput.lib")
//#else
#include <XInput.h>
#pragma comment(lib,"xinput9_1_0.lib")
//#endif

class Gamepad
{
#define INPUT_DEADZONE  ( 0.02f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.
//#define INPUT_MAX 32760
//#define INPUT_MIN -32760
public:
	static enum Axis { LEFT_X, LEFT_Y, RIGHT_X, RIGHT_Y };
	Gamepad();
	~Gamepad();
	bool IsConnected();
	HRESULT UpdateControllerState();
	short GetControllerAxis(Axis axis);
private:
	XINPUT_STATE state;
	bool isConnected;
};

