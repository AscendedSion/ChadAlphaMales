#include "AntiAim.h"
#include "../Vars.h"

void CAntiAim::FixMovement(CUserCmd* pCmd, Vec3 vOldAngles, float fOldSideMove, float fOldForwardMove)
{
	Vec3 curAngs = pCmd->viewangles;

	float fDelta = pCmd->viewangles.y - vOldAngles.y;
	float f1, f2;

	if (vOldAngles.y < 0.0f)
		f1 = 360.0f + vOldAngles.y;

	else f1 = vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;

	else f2 = pCmd->viewangles.y;

	if (f2 < f1)
		fDelta = abs(f2 - f1);

	else fDelta = 360.0f - abs(f1 - f2);

	fDelta = 360.0f - fDelta;

	pCmd->forwardmove = cos(DEG2RAD(fDelta)) * fOldForwardMove + cos(DEG2RAD(fDelta + 90.0f)) * fOldSideMove;
	pCmd->sidemove = sin(DEG2RAD(fDelta)) * fOldForwardMove + sin(DEG2RAD(fDelta + 90.0f)) * fOldSideMove;
}

float CurrentYaw = 0.0f;
float EdgeDistance(float edgeRayYaw)
{
	CBaseEntity* pLocal = g_EntityCache.m_pLocal;

	CGameTrace Trace;
	Ray_t Ray;
	Vector Forward;
	float SP, SY, CP, CY;

	SY = sinf(DEG2RAD(edgeRayYaw)); //Yaw
	CY = cosf(DEG2RAD(edgeRayYaw));
	SP = sinf(DEG2RAD(0)); //Pitch
	CP = cosf(DEG2RAD(0));

	Forward.x = CP * CY;
	Forward.y = CP * SY;
	Forward.z = -SP;

	Forward = Forward * 300.0f + pLocal->GetEyePosition();
	Ray.Init(pLocal->GetEyePosition(), Forward);

	g_Interfaces.EngineTrace->TraceRay(Ray, 0x4200400B, NULL, &Trace);

	float EdgeDistance = (sqrt(pow(Trace.vStartPos.x - Trace.vEndPos.x, 2) + pow(Trace.vStartPos.y - Trace.vEndPos.y, 2)));

	return EdgeDistance;
}

float EdgeYaw = 0;
bool FindEdge(float EdgeOrigYaw)
{
	int edgeLeftDist = EdgeDistance(EdgeOrigYaw);
	edgeLeftDist = edgeLeftDist + EdgeDistance(EdgeOrigYaw);

	int edgeRightDist = EdgeDistance(EdgeOrigYaw);
	edgeRightDist = edgeRightDist + EdgeDistance(EdgeOrigYaw);

	if (edgeLeftDist >= 270)
		edgeLeftDist = -180;

	if (edgeRightDist >= 270)
		edgeRightDist = 0;

	if (edgeLeftDist == edgeRightDist)
		return false;
}

float UseEdge(float EdgeViewAngle)
{

	bool edgeTest = true;
	if (((EdgeViewAngle < -135) || (EdgeViewAngle > 135)) && edgeTest == true)
	{
		EdgeYaw = (float)0;

		edgeTest = false;
	}

	if ((EdgeViewAngle >= -135) && (EdgeViewAngle < -45) && edgeTest == true)
	{
		EdgeYaw = (float)-89;
		edgeTest = false;
	}

	if ((EdgeViewAngle >= -45) && (EdgeViewAngle < 45) && edgeTest == true)
	{
		EdgeYaw = (float)180;
		edgeTest = false;
	}

	if ((EdgeViewAngle <= 135) && (EdgeViewAngle >= 45) && edgeTest == true)
	{
		EdgeYaw = (float)90;
		edgeTest = false;
	}
	return EdgeYaw;
}

void CAntiAim::Pitch(CUserCmd* pCmd)
{
	bool bPitchSet = Vars::AntiHack::AntiAim::Pitch.m_Var;

	if (!bPitchSet)
		return;

	switch (g_EntityCache.m_pLocal->GetClassNum()) {
	case CLASS_SCOUT:
		pCmd->viewangles.x = -89.f;
		break;
	case CLASS_SOLDIER:
		pCmd->viewangles.x = -89.f;
		break;
	case CLASS_PYRO:
		pCmd->viewangles.x = 89.f;
		break;
	case CLASS_DEMOMAN:
		pCmd->viewangles.x = 89.f;
		break;
	case CLASS_HEAVY:
		pCmd->viewangles.x = 89.f;
		break;
	case CLASS_ENGINEER:
		pCmd->viewangles.x = 89.f;
		break;
	case CLASS_MEDIC:
		pCmd->viewangles.x = -89.f;
		break;
	case CLASS_SNIPER:
		pCmd->viewangles.x = -89.f;
		break;
	case CLASS_SPY:
		pCmd->viewangles.x = -89.f;
		break;
	}
}

void CAntiAim::Yaw(CUserCmd* pCmd)
{
	bool bYawSet = Vars::AntiHack::AntiAim::Yaw.m_Var;
	if (!bYawSet)
		return;

	if (FindEdge(pCmd->viewangles.y))
		pCmd->viewangles.y = UseEdge(pCmd->viewangles.y);
	else
		pCmd->viewangles.y = 180.f;
}

void CAntiAim::FakeYaw(CUserCmd* pCmd)
{
	bool bFakeSet = Vars::AntiHack::AntiAim::Fake.m_Var;
	if (!bFakeSet)
		return;

	static bool jitter;
	jitter = !jitter;

	pCmd->viewangles.y += jitter ? 35 : -35;
}


void CAntiAim::Run(CUserCmd* pCmd, bool* pSendPacket)
{
	g_GlobalInfo.m_bAAActive = false;
	g_GlobalInfo.m_vRealViewAngles = g_GlobalInfo.m_vViewAngles;
	g_GlobalInfo.m_vFakeViewAngles = g_GlobalInfo.m_vViewAngles;

	if (!Vars::AntiHack::AntiAim::Active.m_Var)
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!pLocal->IsAlive()
			|| pLocal->IsTaunting()
			|| pLocal->IsInBumperKart()
			|| pLocal->IsAGhost())
			return;

		if (g_GlobalInfo.m_bAttacking)
			return;

		else
		{
			if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon) {
				if (Utils::IsAttacking(pCmd, pWeapon))
					return;
			}
		}

		Vec3 vOldAngles = pCmd->viewangles;
		float fOldSideMove = pCmd->sidemove;
		float fOldForwardMove = pCmd->forwardmove;

		Vec3 vAngles = pCmd->viewangles;


		static bool b = false;

		if (b)
		{
			Yaw(pCmd);
			g_GlobalInfo.m_vRealViewAngles.y = pCmd->viewangles.y;
		}

		else
		{
			FakeYaw(pCmd);
			g_GlobalInfo.m_vFakeViewAngles.y = pCmd->viewangles.y;
		}

		*pSendPacket = b = !b;
		Pitch(pCmd);
		g_GlobalInfo.m_bAAActive = pCmd->viewangles.x || pCmd->viewangles.y;
		FixMovement(pCmd, vOldAngles, fOldSideMove, fOldForwardMove);
	}
}
