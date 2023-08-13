#include "ChatInfo.h"

#include "../Vars.h"
#include "../Misc/Misc.h"
#include "../Visuals/Visuals.h"
#include "../../Utils/TFM/tfm.h"
#define GET_PLAYER_USERID(userid) g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(userid))
#define GET_INDEX_USERID(userid) g_Interfaces.Engine->GetPlayerForUserID(userid)

constexpr int CAT_IDENTIFY = 0xCA7;
constexpr int CAT_REPLY = 0xCA8;

using namespace std::chrono; //lol??? 

void CChatInfo::Event(CGameEvent* pEvent, const FNV1A_t uNameHash) {
	if (!g_Interfaces.Engine->IsConnected() || !g_Interfaces.Engine->IsInGame())
		return;

	if (const auto pLocal = g_EntityCache.m_pLocal) {
		if (Vars::Visuals::ChatInfo.m_Var) {
			if (Vars::Misc::VoteRevealer.m_Var && uNameHash == FNV1A::HashConst(_("vote_cast"))) {
				const auto pEntity = g_Interfaces.EntityList->GetClientEntity(pEvent->GetInt(_("entityid")));
				if (pEntity && pEntity->IsPlayer()) {
					const bool bVotedYes = pEvent->GetInt(_("vote_option")) == 0;
					PlayerInfo_t pi;
					g_Interfaces.Engine->GetPlayerInfo(pEntity->GetIndex(), &pi);
					g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format(_("\x4[CAM] \x3%s \x1voted \x3%s"), pi.name, bVotedYes ? "YES" : "NO").c_str());
				}
			}
			
			if (uNameHash == FNV1A::HashConst(_("player_changeclass"))) {
				if (const auto& pEntity = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt(_("userid"))))) {
					int nIndex = pEntity->GetIndex();

					PlayerInfo_t pi;
					g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi);

					g_Visuals.AddToEventLog(_("%s is %s"), pi.name, Utils::GetClassByIndex(pEvent->GetInt(XorStr("class").c_str())));
					//g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(nIndex, tfm::format("\x4[CAM] \x1%s is now %s", pi.name, Utils::GetClassByIndex(pEvent->GetInt("class"))).c_str());
				}
			}
			if (uNameHash == FNV1A::HashConst(_("player_connect"))) {
				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), tfm::format(_("\x3%s\x1 connected."), pEvent->GetString(_("name")), pEvent->GetString(_("address"))).c_str());
			}
		}

		if (uNameHash == FNV1A::HashConst(_("player_hurt")))
		{
			const auto pAttacker = (CBaseEntity*)g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt(_("attacker"))));
			if (!pAttacker)
				return;

			const auto pVictim = (CBaseEntity*)g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt(_("userid"))));
			if (!pVictim)
				return;

			if (pAttacker == pLocal && pVictim != pLocal)
			{
				PlayerInfo_t player_info;
				//CUserCmd pCmd;
				if (!g_Interfaces.Engine->GetPlayerInfo(pVictim->GetIndex(), &player_info))
					return;
				const auto crit = pEvent->GetBool(_("crit"));

				if (Vars::Chams::Players::HitboxThing.m_Var) {
					g_Visuals.DrawHitboxMatrix(pVictim, Colors::hitboxColor, Vars::Chams::Players::HitboxTimeThing.m_Var);
				}

				if (crit) {
					g_Interfaces.CVars->ConsolePrintf(_("n: %s | dmg: %i | remain: %i (crit)\n"), player_info.name, pEvent->GetInt(_("damageamount")), pEvent->GetInt(_("health")));
					//sprintf_s(buffer, _("Hurt %s for %i (%i health remaining) (crit)\n"), player_info.name, pEvent->GetInt(_("damageamount")), pEvent->GetInt(_("health")));
				}
				else {
					g_Interfaces.CVars->ConsolePrintf(_("n: %s | dmg: %i | remain: %i \n"), player_info.name, pEvent->GetInt(_("damageamount")), pEvent->GetInt(_("health")));
					//sprintf_s(buffer, _("Hurt %s for %i (%i health remaining)\n"), player_info.name, pEvent->GetInt(_("damageamount")), pEvent->GetInt(_("health")));
				}
				//g_Visuals.vecEventVector.push_back(EventLogging_t{ buffer });
			}
			return;
		}
	}
}