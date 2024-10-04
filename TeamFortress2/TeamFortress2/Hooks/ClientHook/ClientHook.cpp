#include "ClientHook.h"

#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AttributeChanger/AttributeChanger.h"
#include "../../SDK/Includes/bitbuf.h"
#include "../../Utils/TFM/tfm.h"

void __stdcall ClientHook::PreEntity::Hook(char const* szMapName)
{
	Table.Original<fn>(index)(g_Interfaces.Client, szMapName);
}

void __stdcall ClientHook::PostEntity::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Client);

	//g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_maxdlights 69420"));
	//g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_dynamic 1"));
	g_Visuals.ModulateWorld();
	g_Visuals.ModulateSky();
}

void __stdcall ClientHook::ShutDown::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Client);
	g_EntityCache.Clear();
}
/*
m_bGib (0xC91)
m_bBurning (0xC92)
m_bElectrocuted (0xC93)
m_bFeignDeath (0xC96)
m_bBecomeAsh (0xC99)
m_bGoldRagdoll (0xCA0)
m_bIceRagdoll (0xCA1)
m_vecRagdollOrigin (0xC7C)
*/
void disableRagdollEffects(CBaseEntity* Ent) {
	*reinterpret_cast<bool*>(Ent + 0xC91) = false;
	*reinterpret_cast<bool*>(Ent + 0xC92) = false;
	*reinterpret_cast<bool*>(Ent + 0xC93) = false;
	*reinterpret_cast<bool*>(Ent + 0xC96) = false;
	*reinterpret_cast<bool*>(Ent + 0xC99) = false;
	*reinterpret_cast<bool*>(Ent + 0xCA0) = false;
	*reinterpret_cast<bool*>(Ent + 0xCA1) = false;
}

void __stdcall ClientHook::FrameStageNotify::Hook(EClientFrameStage FrameStage)
{
	switch (FrameStage)
	{
	case EClientFrameStage::FRAME_RENDER_START:
	{
		g_GlobalInfo.m_vPunchAngles = Vec3();

		if (Vars::Visuals::RemovePunch.m_Var) {
			if (const auto& pLocal = g_EntityCache.m_pLocal) {
				g_GlobalInfo.m_vPunchAngles = pLocal->GetPunchAngles();	//Store punch angles to be compesnsated for in aim
				pLocal->ClearPunchAngle();								//Clear punch angles for visual no-recoil
			}
		}
		g_Visuals.ViewmodelXYZ();
		g_Visuals.ARatio();
		g_Visuals.ThirdPerson();
		break;
	}

	default: break;
	}

	Table.Original<fn>(index)(g_Interfaces.Client, FrameStage);

	switch (FrameStage)
	{
	case EClientFrameStage::FRAME_NET_UPDATE_START: {
		g_EntityCache.Clear();
		break;
	}
		case EClientFrameStage::FRAME_NET_UPDATE_POSTDATAUPDATE_START: {
			g_AttributeChanger.Run();

			if (Vars::Visuals::RagdollEffect.m_Var) {
				for (int i = 1; i < g_Interfaces.EntityList->GetHighestEntityIndex(); i++)
				{
					auto Ent = g_Interfaces.EntityList->GetClientEntity(i);
					if (!Ent)
						continue;
					if (Ent->GetClientClass()->iClassID == 282)
					{
						if (Vars::Visuals::RagdollEffect.m_Var == 1) {
							disableRagdollEffects(Ent);
							*reinterpret_cast<bool*>(Ent + 0xC91) = true;
						}
						if (Vars::Visuals::RagdollEffect.m_Var == 2) {
							disableRagdollEffects(Ent);
							*reinterpret_cast<bool*>(Ent + 0xC92) = true;
						}
						if (Vars::Visuals::RagdollEffect.m_Var == 3) {
							disableRagdollEffects(Ent);
							*reinterpret_cast<bool*>(Ent + 0xC93) = true;
						}
						if (Vars::Visuals::RagdollEffect.m_Var == 4) {
							disableRagdollEffects(Ent);
							*reinterpret_cast<bool*>(Ent + 0xC99) = true;
						}
						if (Vars::Visuals::RagdollEffect.m_Var == 5) {
							disableRagdollEffects(Ent);
							*reinterpret_cast<bool*>(Ent + 0xCA0) = true;
						}
						if (Vars::Visuals::RagdollEffect.m_Var == 6) {
							disableRagdollEffects(Ent);
							*reinterpret_cast<bool*>(Ent + 0xCA1) = true;
						}
						/*switch (Vars::Visuals::RagdollEffect.m_Var) {
						case 1:
							// Gib
							*reinterpret_cast<bool*>(Ent + 0xC91) = true;
						case 2:
							// Burning
							*reinterpret_cast<bool*>(Ent + 0xC92) = true;
						case 3:
							// Electrocuted
							*reinterpret_cast<bool*>(Ent + 0xC93) = true;
						case 4:
							// Feign Death???
							*reinterpret_cast<bool*>(Ent + 0xC96) = true;
						case 5:
							// Become ash
							*reinterpret_cast<bool*>(Ent + 0xC99) = true;
						case 6:
							// Become gold
							*reinterpret_cast<bool*>(Ent + 0xCA0) = true;
						case 7:
							// Become ice
							*reinterpret_cast<bool*>(Ent + 0xCA1) = true;
						}

						//*reinterpret_cast<bool*>(Ent + 0xC93) = true;*/
					}
				}
			}
			break;
		}

	case EClientFrameStage::FRAME_NET_UPDATE_END:
	{
		g_EntityCache.Fill();
		g_Playerlist.GetPlayers();

		g_GlobalInfo.m_bLocalSpectated = false;

		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			for (const auto& Teammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
			{
				if (Teammate->IsAlive() || g_EntityCache.Friends[Teammate->GetIndex()])
					continue;

				CBaseEntity* pObservedPlayer = g_Interfaces.EntityList->GetClientEntityFromHandle(Teammate->GetObserverTarget());

				if (pObservedPlayer == pLocal)
				{
					switch (Teammate->GetObserverMode()) {
					case OBS_MODE_FIRSTPERSON: break;
					case OBS_MODE_THIRDPERSON: break;
					default: continue;
					}

					g_GlobalInfo.m_bLocalSpectated = true;
					break;
				}
			}
		}

		break;
	}

	case EClientFrameStage::FRAME_RENDER_START: {
		g_Visuals.UpdateWorldModulation();
		g_Visuals.UpdateSkyModulation();
		break;
	}

	default: break;
	}
}

bool __stdcall ClientHook::DispatchUserMessage::Hook(int type, bf_read& buf)
{
	static auto oDispatchUserMessage = Table.Original<fn>(index);
	if (type == 25) {
		return true;
	}
	return oDispatchUserMessage(g_Interfaces.Client, type, buf);
}

void __fastcall ClientHook::CalcIsAttackCritical::Hook(CBaseCombatWeapon* this_, void* edx)
{
	const auto& pLocal = g_EntityCache.m_pLocal;
	const auto& pWeapon = g_EntityCache.m_pLocalWeapon;
	if (!pLocal || !pWeapon || pWeapon != this_)
		return Func.Original<fn>()(this_, edx);

	Func.Original<fn>()(this_, edx);
}
#include "../../Features/CritHack/CritHack.h"
void __fastcall ClientHook::AddToCritBucket::Hook(CBaseCombatWeapon* this_, void* edx, float damage)
{
	const auto& pLocal = g_EntityCache.m_pLocal;
	const auto& pWeapon = g_EntityCache.m_pLocalWeapon;
	if (this_ == pWeapon && !g_CritHack.m_is_going_through_crit_cmds)
		if (g_CritHack.finished_last_shot) //easy fix for non-melee.
			return Func.Original<fn>()(this_, edx, damage);
}

CUserCmd* __fastcall ClientHook::GetUserCmd::Hooked_GetUserCmd(void* ecx, void* edx, int sequence_number)
{
	static auto oUserCmd = TableInput.Original<fn>(index);
	if (CUserCmd* cmd_list = *reinterpret_cast<CUserCmd**>(reinterpret_cast<uint32_t>(ecx) + 0x0fc)) {
		if (CUserCmd* cmd = &cmd_list[sequence_number % 90])
			return cmd;
	}

	return oUserCmd(ecx, edx, sequence_number);
}