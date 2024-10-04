#include "Misc.h"

#include "../Vars.h"
#include "../Visuals/Visuals.h"
#include "../SpectatorList/SpectatorList.h"

//#define TICK_INTERVAL			(g_Interfaces.GlobalVars->interval_per_tick)
//#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define MAX_CACHE TIME_TO_TICKS(1)

//static bool push = true;
//char bufferz[256];
void CMisc::Run(CUserCmd* pCmd)
{
	AutoJump(pCmd);
	AutoStrafe(pCmd);
	NoiseMakerSpam();
	ChatSpam();
	InstantRespawn();
	NoPush();
}

void CMisc::InstantRespawn()
{
	if (!Vars::Misc::InstantRespawn.m_Var || !g_EntityCache.m_pLocal || g_EntityCache.m_pLocal->IsAlive())
		return;

	auto* KV = new KeyValues("MVM_Revive_Response");
	KV->SetInt("accepted", 1);
	g_Interfaces.Engine->ServerCmdKeyValues(KV);
}

void VectorAngles(Vector& forward, Vector& angles)
{
	float tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt((forward[0] * forward[0] + forward[1] * forward[1]));
		pitch = (atan2(-forward[2], tmp) * 180 / PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void AngleVectors2(const QAngle& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	Math::SinCos(DEG2RAD(angles.x), &sy, &cy);
	Math::SinCos(DEG2RAD(angles.y), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

QAngle VectorToQAngle(Vector in)
{
	return *(QAngle*)&in;
}

void CMisc::CheatsBypass()
{
	if (Vars::Misc::CheatsBypass.m_Var) {
		auto cheats = g_Interfaces.CVars->FindVar(_("sv_cheats"));
		if (cheats) {
			if (cheats->GetInt() == 0) {
				cheats->SetValue(1);
			}
		}
	}
}

void CMisc::EdgeJump(CUserCmd* pCmd, const int nOldFlags)
{
	if ((nOldFlags & FL_ONGROUND) && Vars::Misc::EdgeJump.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			if (pLocal->IsAlive() && !pLocal->IsOnGround() && !pLocal->IsSwimming())
				pCmd->buttons |= IN_JUMP;
		}
	}
}

void CMisc::AutoJump(CUserCmd* pCmd)
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!Vars::Misc::AutoJump.m_Var
			|| !pLocal->IsAlive()
			|| pLocal->IsSwimming()
			|| pLocal->IsInBumperKart()
			|| pLocal->IsAGhost())
			return;

		static bool bJumpState = false;

		if (pCmd->buttons & IN_JUMP)
		{
			if (!bJumpState && !pLocal->IsOnGround())
				pCmd->buttons &= ~IN_JUMP;

			else if (bJumpState)
				bJumpState = false;
		}

		else if (!bJumpState)
			bJumpState = true;
	}
}
bool AutoStrafeFlip = false;

static float normalizeRad(float a) noexcept
{
	return std::isfinite(a) ? std::remainder(a, PI * 2) : 0.0f;
}

static float angleDiffRad(float a1, float a2) noexcept
{
	float delta;

	delta = normalizeRad(a1 - a2);
	if (a1 > a2)
	{
		if (delta >= PI)
			delta -= PI * 2;
	}
	else
	{
		if (delta <= -PI)
			delta += PI * 2;
	}
	return delta;
}

void CMisc::AutoStrafe(CUserCmd* pCmd)
{
	if (!Vars::Misc::AutoStrafe.m_Var)
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		static bool was_jumping = false;
		bool is_jumping = pCmd->buttons & IN_JUMP;

		if ((!is_jumping || was_jumping) && !pLocal->IsSwimming() && !pLocal->IsOnGround())
		{

			const float speed = pLocal->GetVelocity().Lenght2D();
			auto vel = pLocal->GetVelocity();

			if (pLocal->GetMoveType() & (MOVETYPE_LADDER | MOVETYPE_NOCLIP))
				return;

			if (speed < 2.0f)
				return;

			constexpr auto perfectDelta = [](float speed) noexcept
			{
				if (const auto& pLocal = g_EntityCache.m_pLocal) {
					static auto speedVar = pLocal->GetMaxSpeed();
					static auto airVar = g_Interfaces.CVars->FindVar(_("sv_airaccelerate"));

					static auto wishSpeed = 30.0f;

					const auto term = wishSpeed / airVar->GetFloat() / speedVar * 100.f / speed;

					if (term < 1.0f && term > -1.0f)
						return acosf(term);
				}
				return 0.0f;
			};

			const float pDelta = perfectDelta(speed);
			if (pDelta)
			{
				const float yaw = DEG2RAD(pCmd->viewangles.y);
				const float velDir = atan2f(vel.y, vel.x) - yaw;
				const float wishAng = atan2f(-pCmd->sidemove, pCmd->forwardmove);
				const float delta = angleDiffRad(velDir, wishAng);

				float moveDir = delta < 0.0f ? velDir + pDelta : velDir - pDelta;

				pCmd->forwardmove = cosf(moveDir) * 450.f;
				pCmd->sidemove = -sinf(moveDir) * 450.f;
			}


		}
		was_jumping = is_jumping;
	}
}

void CMisc::InitSpamKV(void* pKV)
{
	char chCommand[30] = "use_action_slot_item_server";
	typedef int(__cdecl* HashFunc_t)(const char*, bool);

	static DWORD dwHashFunctionLocation = g_Pattern.Find(_(L"client.dll"), _(L"FF 15 ? ? ? ? 83 C4 08 89 06 8B C6"));
	static HashFunc_t SymbForString = (HashFunc_t) * *(PDWORD*)(dwHashFunctionLocation + 0x2);

	int nAddr = 0;
	while (nAddr < 29)
	{
		switch (nAddr)
		{
		case 0:
			*(PDWORD)((DWORD)pKV + nAddr) = SymbForString(chCommand, true);
			break;
		case 16:
			*(PDWORD)((DWORD)pKV + nAddr) = 0x10000;
			break;
		default:
			*(PDWORD)((DWORD)pKV + nAddr) = 0;
			break;
		}

		nAddr += 4;
	}
}

void CMisc::NoiseMakerSpam()
{
	if (!Vars::Misc::NoisemakerSpam.m_Var)
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->GetUsingActionSlot())
			return;

		if (pLocal->GetNextNoiseMakerTime() < g_Interfaces.GlobalVars->curtime)
		{
			if (const auto pKV = Utils::InitKeyValue())
			{
				InitSpamKV(pKV);
				g_Interfaces.Engine->ServerCmdKeyValues(pKV);
			}
		}
	}
}

void CMisc::BypassPure()
{
	if (Vars::Misc::BypassPure.m_Var)
	{
		static DWORD dwAddress = 0x0;

		while (!dwAddress)
			dwAddress = g_Pattern.Find(_(L"engine.dll"), _(L"A1 ? ? ? ? 56 33 F6 85 C0"));

		static DWORD* pPure = nullptr;

		while (!pPure)
		{
			if (reinterpret_cast<DWORD**>(dwAddress + 0x01))
				pPure = *reinterpret_cast<DWORD**>(dwAddress + 0x01);
		}

		if (*pPure)
			*pPure = 0x0;
	}
}

std::string GetSpam(const int nIndex) {
	std::string str;

	switch (nIndex)
	{
	case 0: str = XorStr("say CAM | Get good get ChadAlphaMales.club").str(); break;
	case 1: str = XorStr("say CAM | Way to the top!").str(); break;
	case 2: str = XorStr("say CAM | Co-op with fuk0ff.com").str(); break;
	case 3: str = XorStr("say CAM | Owning casual with ease").str(); break;
	default: str = XorStr("say CAM | ChadAlphaMales.club").str(); break;
	}

	return str;
}

void CMisc::ChatSpam()
{
	if (!Vars::Misc::ChatSpam.m_Var)
		return;

	float flCurTime = g_Interfaces.Engine->Time();
	static float flNextSend = 0.0f;

	if (flCurTime > flNextSend) {
		g_Interfaces.Engine->ClientCmd_Unrestricted(GetSpam(Utils::RandIntSimple(0, 5)).c_str());
		flNextSend = (flCurTime + 3.0f);
	}
}

void CMisc::AutoRocketJump(CUserCmd* pCmd)
{
	if (!Vars::Misc::AutoRocketJump.m_Var || !g_GlobalInfo.m_bWeaponCanAttack || !GetAsyncKeyState(VK_RBUTTON))
		return;

	if (g_Interfaces.EngineVGui->IsGameUIVisible() || g_Interfaces.Surface->IsCursorVisible())
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->GetClassNum() != CLASS_SOLDIER || !pLocal->IsOnGround() || pLocal->IsDucking())
			return;

		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
		{
			if (pWeapon->IsInReload()) {
				pCmd->buttons |= IN_ATTACK;
				return;
			}

			else
			{
				if (pCmd->buttons & IN_ATTACK)
					pCmd->buttons &= ~IN_ATTACK;
			}

			if (g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheBeggarsBazooka
				|| g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheCowMangler5000
				|| pWeapon->GetSlot() != SLOT_PRIMARY)
				return;

			if (pLocal->GetViewOffset().z < 60.05f)
			{
				pCmd->buttons |= IN_ATTACK | IN_JUMP;

				Vec3 vVelocity = pLocal->GetVelocity();
				Vec3 vAngles = { vVelocity.IsZero() ? 89.0f : 45.0f, Math::VelocityToAngles(vVelocity).y - 180.0f, 0.0f };

				if (g_GlobalInfo.m_nCurItemDefIndex != Soldier_m_TheOriginal && !vVelocity.IsZero()) {
					Vec3 vForward = {}, vRight = {}, vUp = {};
					Math::AngleVectors(vAngles, &vForward, &vRight, &vUp);
					Math::VectorAngles((vForward * 23.5f) + (vRight * -5.6f) + (vUp * -3.0f), vAngles);
				}

				Math::ClampAngles(vAngles);
				pCmd->viewangles = vAngles;
				g_GlobalInfo.m_bSilentTime = true;
			}

			else pCmd->buttons |= IN_DUCK;
		}
	}
}

static void(__thiscall* ob_SetConVar)(void*, const char*, const char*) = reinterpret_cast<void(__thiscall*)(void*, const char*, const char*)>(g_Pattern.Find(L"engine.dll", L"08 C6 47 04 01 C7") - 0x1);

void CMisc::ChangeName(std::string name)
{
	static char msg_SetConVar[0x250];
	memset(msg_SetConVar, 0, sizeof(msg_SetConVar));

	ob_SetConVar(msg_SetConVar, "name", name.c_str());

	g_Interfaces.Engine->GetNetChannelInfo()->SendFartMsg(msg_SetConVar);
}

void CMisc::NoPush() {
	ConVar* noPush = g_Interfaces.CVars->FindVar(_("tf_avoidteammates_pushaway"));
	if (Vars::Misc::NoPush.m_Var) {
		if (noPush->GetInt() == 1) noPush->SetValue(0);
	}
	else {
		if (noPush->GetInt() == 0) noPush->SetValue(1);
	}
}

void CMisc::AntiWarp(CUserCmd* pCmd)
{
	const auto& pLocal = g_EntityCache.m_pLocal;

	if (!pLocal || !pLocal->IsAlive()
		|| !Vars::Misc::CL_Move::AntiWarp.m_Var || !dt.shifting)
		return;

	Vector Velocity = pLocal->GetVelocity();
	float Speed = Velocity.Lenght();
	INetChannelInfo* pNetChannel = reinterpret_cast<INetChannelInfo*>(g_Interfaces.Engine->GetNetChannelInfo());
	if (!pNetChannel || !g_Interfaces.ClientState ||
		Speed < 23.5f)
		return;

	Vector Angles, Forward;
	VectorAngles(Velocity, Angles);
	Vector Viewangles;
	g_Interfaces.Engine->GetViewAngles(Viewangles);
	Angles.y = Viewangles.y - Angles.y;
	Math::AngleVectors(Angles, &Forward);
	Forward *= Speed;

	float Scale = (pLocal->GetMaxSpeed() / Speed) +
		pNetChannel->GetLatency(0);

	pCmd->forwardmove = -Forward.x * Scale;
	pCmd->sidemove = -Forward.y * Scale;
}