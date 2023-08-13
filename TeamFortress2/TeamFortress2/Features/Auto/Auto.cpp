#include "Auto.h"

#include "../Vars.h"

#include "AutoShoot/AutoShoot.h"
#include "AutoStab/AutoStab.h"
#include "AutoDetonate/AutoDetonate.h"
#include "AutoBlast/AutoBlast.h"
#include "AutoUber/AutoUber.h"
#include "AutoVaccinator/AutoVaccinator.h"

bool CAuto::ShouldRun(CBaseEntity *pLocal)
{

	if (g_Interfaces.EngineVGui->IsGameUIVisible() || g_Interfaces.Surface->IsCursorVisible())
		return false;

	return true;
}

void CAuto::Run(CUserCmd* pCmd)
{
	if (Vars::Triggerbot::Stab::Disguise.m_Var && g_AutoStab.m_bShouldDisguise)
		g_Interfaces.Engine->ClientCmd_Unrestricted(_("lastdisguise"));

	g_GlobalInfo.m_bAutoBackstabRunning = false;
	g_AutoStab.m_bShouldDisguise = false;

	auto pLocal = g_EntityCache.m_pLocal;
	auto pWeapon = g_EntityCache.m_pLocalWeapon;

	if (pLocal && pWeapon)
	{
		if (ShouldRun(pLocal))
		{
			g_AutoVaccinator.Run(pCmd);
			if (g_AutoGlobal.IsKeyDown()) { // This looks dumb but oh well fuck it
				if (Vars::Triggerbot::Global::Active.m_Var) { // I had to put the fucking global triggerbot shit here because it's unironically fucking annoying and the auto detonate thingie doesn't want to work because of it
					g_AutoShoot.Run(pLocal, pWeapon, pCmd);
				}
				g_AutoStab.Run(pLocal, pWeapon, pCmd);
				g_AutoDetonate.Run(pLocal, pWeapon, pCmd);
				g_AutoAirblast.Run(pLocal, pWeapon, pCmd);
				g_AutoUber.Run(pLocal, pWeapon, pCmd);
			}
		}
	}
}