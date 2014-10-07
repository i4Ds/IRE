#include "Common.h"

using namespace glm;

class Tracker {
protected:
public:

	int run() {
		ovrHmd hmd = ovrHmd_Create(0);
		if (!hmd || !ovrHmd_StartSensor(hmd, ovrSensorCap_Orientation, 0)) {
			SAY_ERR("Unable to detect Rift head tracker");
			return -1;
		}
		for (int i = 0; i < 100; ++i) {
			ovrSensorState state = ovrHmd_GetSensorState(hmd, 0);

			ovrQuatf orientation = state.Recorded.Pose.Orientation;
			glm::quat q = glm::make_quat(&orientation.x);
			glm::vec3 euler = glm::eulerAngles(q);

			SAY("Current orientation - roll %0.2f, pitch %0.2f, yaw %0.2f",
				euler.z * RADIANS_TO_DEGREES,
				euler.x * RADIANS_TO_DEGREES,
				euler.y * RADIANS_TO_DEGREES);
			Platform::sleepMillis(1000);
		}
		ovrHmd_StopSensor(hmd);
		ovrHmd_Destroy(hmd);
		return 0;
	}
};

RUN_OVR_APP(Tracker);
