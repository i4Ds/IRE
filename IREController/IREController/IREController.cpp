#include "stdafx.h"
#include "conio.h"
#include <windows.h>
#include "OVR_CAPI.h"
#include "Kernel\OVR_Math.h"
#include "MaestroController.h"
#include "Gamepad.h"

#pragma comment (lib, "user32.lib")

using namespace OVR;
using namespace std;

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define PI 3.14159265f
#define HALF_PI 3.14159265f / 2
#define RADIANS_TO_DEGREES (180.0f / PI)

#define GAMEPAD_MAX 32760
#define GAMEPAD_MIN -32760
#define LEFT_MOTOR_MAX 8000
#define LEFT_MOTOR_MID 6000
#define LEFT_MOTOR_MIN 4000
#define RIGHT_MOTOR_MAX 8000
#define RIGHT_MOTOR_MID 6000
#define RIGHT_MOTOR_MIN 4000
#define YAW_SERVO_MIN 3012
#define YAW_SERVO_MAX 9932
#define YAW_SERVO_MID (YAW_SERVO_MIN + YAW_SERVO_MAX) / 2
#define PITCH_SERVO_MIN 2900
#define PITCH_SERVO_MAX 9508
#define PITCH_SERVO_MID (PITCH_SERVO_MIN + PITCH_SERVO_MAX) / 2
#define ROLL_SERVO_MIN 2952
#define ROLL_SERVO_MAX 8624
#define ROLL_SERVO_MID (ROLL_SERVO_MIN + ROLL_SERVO_MAX) / 2
#define SERVO_ANGLE_MAX 78.5f
#define SERVO_ANGLE_MIN -78.5f

Gamepad	_gamepad;
MaestroController _maestroController;
ovrHmd hmd;
bool terminateApp = false;
bool pauseApp = false;
bool tankControlMode = false;
bool pauseMotor = false;
bool pauseServo = false;
unsigned int mainCycleCounter = 0;
float lastYawBeforeOutsideViewport = NULL;
float lastRollBeforeOutsideViewport = NULL;

//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------
bool SendCommands(unsigned short leftMotor, unsigned short rightMotor, unsigned short yawServo, unsigned short pitchServo, unsigned short rollServo);
void StopRobot();
void GetMotorTargets(unsigned short &leftMotor, unsigned short &rightMotor);
void GetServoTargets(unsigned short &yawServo, unsigned short &pitchServo, unsigned short &rollServo);
BOOL CtrlHandler(DWORD fdwCtrlType);
void CheckForHotkey();
static long Mapl(long x, long in_min, long in_max, long out_min, long out_max);
static short Maps(short x, short in_min, short in_max, short out_min, short out_max);
static float Mapf(float x, float in_min, float in_max, float out_min, float out_max);
static float Clip(float val, float min, float max);
static long ElapsedMillis();

//-----------------------------------------------------------------------------
// Main Function
//-----------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	if (!_maestroController.Connect("\\\\.\\COM3", 230400)) {
		printf("Unable to connect to Maestro.");
		return -1;
	}

	ovr_Initialize();
	hmd = ovrHmd_Create(0);
	if (!hmd || !ovrHmd_StartSensor(hmd, ovrSensorCap_Orientation | ovrSensorCap_YawCorrection, ovrSensorCap_Orientation)) {
		printf("Unable to detect Rift head tracker\n");
		return -1;
	}

	float         hertz = 0;
	int cycleCount = 0;
	long start = ElapsedMillis();

	unsigned short leftMotor = 0; unsigned short rightMotor = 0;
	unsigned short yawServo = 0; unsigned short pitchServo = 0; unsigned short rollServo = 0;
	while (!terminateApp)
	{
		if (!pauseApp) {
			if (!pauseMotor){
				GetMotorTargets(leftMotor, rightMotor);
			}
			else {
				printf("Pausing MOTOR\n");
				leftMotor = LEFT_MOTOR_MID;
				rightMotor = RIGHT_MOTOR_MID;
			}

			if (!pauseServo) {
				GetServoTargets(yawServo, pitchServo, rollServo);
			}
			else {
				printf("Pausing SERVO\n");
			}

			SendCommands(leftMotor, rightMotor, yawServo, pitchServo, rollServo);

			long now = ElapsedMillis();
			++cycleCount;
			if ((now - start) >= 500) {
				float elapsed = (now - start) / 1000.f;
				hertz = (float)cycleCount / elapsed;
				start = now;
				cycleCount = 0;
			}
			printf("Hertz: %0.2f \n", hertz);

			mainCycleCounter++;
		}

		CheckForHotkey();

		Sleep(1);
	}

	if (!pauseApp) {
		StopRobot();
	}

	_maestroController.Disconnect();

	ovrHmd_Destroy(hmd);
	ovr_Shutdown();

	return 0;
}

//-----------------------------------------------------------------------------
bool SendCommands(unsigned short leftMotor, unsigned short rightMotor, unsigned short yawServo, unsigned short pitchServo, unsigned short rollServo)
{
	printf("LEFT MOTOR: %d RIGHT MOTOR %d YAW SERVO: %d PITCH SERVO: %d ROLL SERVO: %d \n", leftMotor, rightMotor, yawServo, pitchServo, rollServo);

	return _maestroController.SetMultipleTargets(0, { leftMotor, rightMotor, yawServo, pitchServo, rollServo });
}

void StopRobot() {
	SendCommands(6000, 6000, YAW_SERVO_MID, PITCH_SERVO_MIN, ROLL_SERVO_MID);

	Sleep(500);

	SendCommands(0, 6000, 0, 0, 0);
}

//-----------------------------------------------------------------------------
void GetMotorTargets(unsigned short& leftMotor, unsigned short& rightMotor) {
	_gamepad.UpdateControllerState();

	unsigned short thisLeftMotor = leftMotor;
	unsigned short thisRightMotor = rightMotor;

	if (tankControlMode) {
		short leftStickY = _gamepad.GetControllerAxis(Gamepad::LEFT_Y);
		short rightStick = _gamepad.GetControllerAxis(Gamepad::RIGHT_Y);

		printf("LEFT Y: %d RIGHT Y: %d \n", leftStickY, rightStick);

		thisLeftMotor = Maps(leftStickY, GAMEPAD_MIN, GAMEPAD_MAX, LEFT_MOTOR_MIN, LEFT_MOTOR_MAX);
		thisRightMotor = Maps(rightStick, GAMEPAD_MIN, GAMEPAD_MAX, RIGHT_MOTOR_MIN, RIGHT_MOTOR_MAX);
	}
	else {
		short stickY = _gamepad.GetControllerAxis(Gamepad::LEFT_Y);
		short stickX = _gamepad.GetControllerAxis(Gamepad::RIGHT_X);

		printf("LEFT Y: %d RIGHT X: %d \n", stickY, stickX);

		short leftMotorInput = (stickY + stickX) / 2;
		short rightMotorInput = (stickY - stickX) / 2;
		if (stickX == 0) {
			leftMotorInput = stickY * 0.8f;
			rightMotorInput = stickY * 0.8f;
		}

		thisLeftMotor = Maps(leftMotorInput, GAMEPAD_MIN, GAMEPAD_MAX, LEFT_MOTOR_MIN, LEFT_MOTOR_MAX);
		thisRightMotor = Maps(rightMotorInput, GAMEPAD_MIN, GAMEPAD_MAX, RIGHT_MOTOR_MIN, RIGHT_MOTOR_MAX);
	}

	//if (thisRightMotor > 6000) {
	//	thisRightMotor -= 120;
	//}
	//else {
	//	thisRightMotor += 60;
	//}

	leftMotor = thisLeftMotor;
	rightMotor = thisRightMotor;
}

//-----------------------------------------------------------------------------
void GetServoTargets(unsigned short &yawServo, unsigned short &pitchServo, unsigned short &rollServo){
	ovrSensorState state = ovrHmd_GetSensorState(hmd, ovr_GetTimeInSeconds()/* + 0.1f*/);
	Quatf orientation = state.Recorded.Pose.Orientation;

	float yaw; float pitch; float roll;
	orientation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);

	//if (abs(yaw) > HALF_PI && abs(roll) > HALF_PI) {
	//	if (pitch >= 0) {
	//		pitch = 2 * HALF_PI - pitch;
	//	}
	//	else {
	//		pitch = -2 * HALF_PI - pitch;
	//	}

	//	if (roll >= 0) {
	//		roll = PI - roll;
	//	}
	//	else {
	//		roll = -PI - roll;
	//	}
	//}

	//yaw = fmodf((yaw + PI), PI);

	//if (HALF_PI <= abs(pitch) && HALF_PI <= abs(yaw)) {
	//	yaw -= PI;
	//}

	//Vector3f lookAt = Vector3f(0.f, 0.f, 1.f);
	//lookAt = orientation.Rotate(lookAt);

	//printf("Lookat: X: %0.2f Y: %0.2f Z: %0.2f \n", lookAt.x, lookAt.y, lookAt.z);

	//Vector3f axis = Vector3f(0.f, 0.f, 1.f);
	//float angle;
	//orientation.GetAxisAngle(&axis, &angle);

	//printf("axis: X: %0.2f Y: %0.2f Z: %0.2f Angle %0.2f\n", axis.x, axis.y, axis.z, angle);

	//Quatf testQuat = Quatf(lookAt, angle);
	//testQuat.Invert();
	//printf("X: %0.2f Y: %0.2f Z: %0.2f W: %0.2f \n", testQuat.x, testQuat.y, testQuat.z, testQuat.w);

	//if (lookAt.z < 0) {
	//	if (yaw > 0) {
	//		yaw -= PI;
	//	}
	//	else {
	//		yaw += PI;
	//	}
	//}



	/*yaw = Clip(yaw, DegreeToRad(SERVO_ANGLE_MIN), DegreeToRad(SERVO_ANGLE_MAX));
	pitch = Clip(pitch, DegreeToRad(SERVO_ANGLE_MIN), DegreeToRad(SERVO_ANGLE_MAX));*/
	//pitch = 0;
	//printf("YAW: %0.2f PITCH: %0.2f ROLL: %0.2f \n", yaw, pitch, roll);

	//Vector3f lookAt = Vector3f(0.f, 0.f, 1.f);
	//lookAt = orientation.Rotate(lookAt);

	//float x2 = lookAt.x;
	//float y2 = lookAt.y;
	//float z2 = lookAt.z;

	//printf("Positon: X: %0.2f Y: %0.2f Z: %0.2f \n", x2, y2, z2);


	float x = sin(yaw) * cos(pitch);
	float y = -sin(pitch);
	float z = cos(yaw) * cos(pitch);

	//y = Clip(y, -0.5722f, 0.5722f);
	printf("Vector: X: %0.2f Y: %0.2f Z: %0.2f \n", x, y, z);
	printf("YAW: %0.2f PITCH: %0.2f ROLL: %0.2f \n", yaw*RADIANS_TO_DEGREES, pitch*RADIANS_TO_DEGREES, roll*RADIANS_TO_DEGREES);

	if (z < 0)
	{
		if (abs(roll) > HALF_PI){
			x = -x;
			z = fabsf(z);
		}
		//y = -y;

		if (roll > HALF_PI) {
			roll = PI - roll;
		}
		else if (roll < -HALF_PI) {
			roll = -PI - roll;
		}
		//roll = fmodf(roll + PI, 2 * PI) - PI;
	}


	yaw = atan2(x, z);
	pitch = -atan2(y, sqrtf(z*z + x*x));

	//yaw = atan2(y, x);
	//pitch = acos(z / sqrtf(x*x + y*y + z*z));

	///*yaw = fmodf(yaw + PI * 2, PI * 2);
	//pitch = fmodf(pitch + PI * 2, PI * 2);*/


	yaw *= RADIANS_TO_DEGREES;
	pitch *= RADIANS_TO_DEGREES;
	roll *= RADIANS_TO_DEGREES;

	printf("YAW: %0.2f PITCH: %0.2f ROLL: %0.2f \n", yaw, pitch, roll);


	if (pitch > 50)
	{
		if (lastYawBeforeOutsideViewport == NULL){
			lastYawBeforeOutsideViewport = yaw;
		}
		if (lastRollBeforeOutsideViewport == NULL) {
			lastRollBeforeOutsideViewport = roll;
		}

		float a = Clip(1 - (pitch - 60) / 10.f, 0, 1);
		yaw = yaw*a + (1 - a)*lastYawBeforeOutsideViewport;

		a = Clip(1 - (pitch - 50) / 10.f, 0, 1);
		roll = roll*a + (1 - a) * lastRollBeforeOutsideViewport;
	}
	else if (pitch < -50)
	{
		if (lastYawBeforeOutsideViewport == NULL){
			lastYawBeforeOutsideViewport = yaw;
		}
		if (lastRollBeforeOutsideViewport == NULL) {
			lastRollBeforeOutsideViewport = roll;
		}

		float a = Clip(1 - (pitch - -60) / -10.f, 0, 1);
		yaw = yaw*a + (1 - a)*lastYawBeforeOutsideViewport;

		a = Clip(1 - (pitch - -50) / -10.f, 0, 1);
		roll = roll*a + (1 - a)*lastRollBeforeOutsideViewport;
	}
	else {
		lastYawBeforeOutsideViewport = NULL;
		lastRollBeforeOutsideViewport = NULL;
	}

	printf("YAW: %0.2f PITCH: %0.2f ROLL: %0.2f \n", yaw, pitch, roll);

	//if (abs(yaw) <= SERVO_ANGLE_MAX && abs(pitch) <= SERVO_ANGLE_MAX && abs(roll) <= SERVO_ANGLE_MAX) {

	//if (abs(pitch) < 80.f || abs(pitch) > 100.f) {
	yaw = Clip(yaw, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX);
	//if (yaw <= SERVO_ANGLE_MAX && yaw >= SERVO_ANGLE_MIN && (abs(pitch) < 80.f || abs(pitch) > 100.f) ){
	yawServo = Mapf(yaw, -79, 79, YAW_SERVO_MIN, YAW_SERVO_MAX);
	//}

	pitch = Clip(pitch, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX);
	//if (pitch <= SERVO_ANGLE_MAX && pitch >= SERVO_ANGLE_MIN) {
	pitchServo = Mapf(pitch, 79, -79, PITCH_SERVO_MIN, PITCH_SERVO_MAX);
	//}

	//if (roll <= SERVO_ANGLE_MAX && roll >= SERVO_ANGLE_MIN && (abs(pitch) < 77.f || abs(pitch) > 103.f)) {
	if (pitch >= -79 && pitch <= -5) {
		float rollMax = (-0.0003f * powf(pitch, 3)) - (0.0512f * powf(pitch, 2)) - 3.3477 * pitch - 90.803f;
		printf("Roll max: %0.2f \n", rollMax);
		roll = max(roll, rollMax);
	}
	roll = Clip(roll, -58, 58);
	rollServo = Mapf(roll, 79, -79, ROLL_SERVO_MIN, ROLL_SERVO_MAX);
	//}
	//}

	//}
}

//-----------------------------------------------------------------------------
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		terminateApp = true;
		Sleep(3000);
		return TRUE;

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		printf("Ctrl-Close event\n\n");
		terminateApp = true;
		Sleep(3000);
		return TRUE;

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		Beep(900, 200);
		printf("Ctrl-Break event\n\n");
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		Beep(1000, 200);
		printf("Ctrl-Logoff event\n\n");
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		Beep(750, 500);
		printf("Ctrl-Shutdown event\n\n");
		return FALSE;

	default:
		return FALSE;
	}
}

void CheckForHotkey() {
	if (_kbhit() != 0) {
		// keycodes: http://msdn.microsoft.com/en-us/library/aa299374%28VS.60%29.aspx
		int ch = _getch();
		ch = toupper(ch);

		switch (ch)
		{
		case 'R':
		case 63:
		case 27:
			printf("Reset HMD Sensor.\n");
			ovrHmd_ResetSensor(hmd);
			break;
		case 'C':
			tankControlMode = !tankControlMode;
			printf("Tank control mode %s \n", tankControlMode ? "enabled" : "disabled");
			break;
		case ' ':
		case 46:
			if (!pauseApp) {
				pauseApp = true;
				StopRobot();
			}
			else {
				pauseApp = false;
			}
			break;
		case 73:
			pauseMotor = !pauseMotor;
			break;
		case 81:
			pauseServo = !pauseServo;
			break;
		}
	}
}

//-----------------------------------------------------------------------------
static long Mapl(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//-----------------------------------------------------------------------------
static short Maps(short x, short in_min, short in_max, short out_min, short out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//-----------------------------------------------------------------------------
static float Mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
	return ((unsigned short)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min));
}

//-----------------------------------------------------------------------------
static float Clip(float val, float min, float max) {
	return min(max(val, min), max);
}

static long ElapsedMillis() {
	static long start = GetTickCount();
	return GetTickCount() - start;
}