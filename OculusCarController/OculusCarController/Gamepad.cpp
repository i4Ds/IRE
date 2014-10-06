#include "stdafx.h"
#include "Gamepad.h"


Gamepad::Gamepad()
{
}


Gamepad::~Gamepad()
{
}

bool Gamepad::IsConnected() {
	return isConnected;
}

HRESULT Gamepad::UpdateControllerState() {
	DWORD dwResult;
	// Simply get the state of the controller from XInput.
	dwResult = XInputGetState(0, &state);

	if (dwResult == ERROR_SUCCESS)
		isConnected = true;
	else
		isConnected = false;

	return S_OK;
}


short Gamepad::GetControllerAxis(Axis axis) {
	short axisValue;
	switch (axis)
	{
	case LEFT_X:
		axisValue = state.Gamepad.sThumbLX;
		break;
	case LEFT_Y:
		axisValue = state.Gamepad.sThumbLY;
		break;
	case RIGHT_X:
		axisValue = state.Gamepad.sThumbRX;
		break;
	case RIGHT_Y:
		axisValue = state.Gamepad.sThumbRY;
		break;
	default:
		return 0;
	}

	if (axisValue < INPUT_DEADZONE && axisValue > -INPUT_DEADZONE) {
		axisValue = 0;
	}

	axisValue = min(axisValue, 0x7FFF);

	axisValue = max(axisValue, -0x7FFF);

	return (short) axisValue;
}




