#pragma once
#include "../../SDK/SDK.h"

class CAntiAim
{
private:
	void FixMovement(CUserCmd *pCmd, Vec3 vOldAngles, float fOldSideMove, float fOldForwardMove);

public:
	bool bRight, bLeft, bBack;

	enum class Directions : int {
		YAW_RIGHT,
		YAW_LEFT,
		YAW_BACK,
		YAW_NONE
	};

	Directions m_CurrentDirection = Directions::YAW_NONE;

	void Pitch(CUserCmd* pCmd);
	void BaseYaw(CUserCmd* pCmd);
	void HandleYaw(CUserCmd* pCmd);
	void YawDirection(CUserCmd* pCmd);
	void FakeYaw(CUserCmd* pCmd);
	void Run(CUserCmd *pCmd, bool *pSendPacket);
};

inline CAntiAim g_AntiAim;