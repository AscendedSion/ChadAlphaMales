#pragma once
#include "../../SDK/SDK.h"

class CAntiAim
{
private:
	void FixMovement(CUserCmd *pCmd, Vec3 vOldAngles, float fOldSideMove, float fOldForwardMove);

public:
	void Pitch(CUserCmd* pCmd);
	void Yaw(CUserCmd* pCmd);
	void FakeYaw(CUserCmd* pCmd);
	void Run(CUserCmd *pCmd, bool *pSendPacket);
};

inline CAntiAim g_AntiAim;