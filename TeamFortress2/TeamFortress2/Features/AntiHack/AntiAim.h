#pragma once
#include "../../SDK/SDK.h"

class CAntiAim
{
private:
	void FixMovement(CUserCmd *pCmd, Vec3 vOldAngles, float fOldSideMove, float fOldForwardMove);

public:
	bool bRight;
	bool bBack;
	bool bLeft;

	enum class Directions : int {
		YAW_RIGHT,
		YAW_BACK,
		YAW_LEFT
	};

	Directions m_CurrentDirection = Directions::YAW_BACK;

	void Pitch(CUserCmd* pCmd);
	void Yaw(CUserCmd* pCmd);
	void YawDirection(CUserCmd* pCmd);
	void FakeYaw(CUserCmd* pCmd);
	void Run(CUserCmd *pCmd, bool *pSendPacket);
};

inline CAntiAim g_AntiAim;