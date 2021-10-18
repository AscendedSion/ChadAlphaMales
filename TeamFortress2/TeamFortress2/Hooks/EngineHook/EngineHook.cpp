#include "EngineHook.h"

#include "../../Features/Vars.h"

#include "../../Features/Misc/Misc.h"

void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
{
	if (Vars::Misc::CL_Move::Doubletap.m_Var)
	{
		dt.FastStop = false;
		/*
		if (Vars::Misc::CL_Move::TeleportKey.m_Var && (GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var)) && !g_GlobalInfo.m_nShifted) //teleport
		{
			g_GlobalInfo.fast_stop = false;
			while (g_GlobalInfo.m_nShifted < MAX_NEW_COMMANDS_HEAVY)87
			{
				g_GlobalInfo.fast_stop = false;
				g_GlobalInfo.m_nShifted++;
				Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (MAX_NEW_COMMANDS_HEAVY - 1))); //this teleports you
				//g_GlobalInfo.m_nShifted++;
			}
			return;
		}
		*/
		if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) {//recharge key
			dt.FastStop = false;
			dt.Recharging = true;
		}
	}

	if (dt.Recharging && dt.Charged != dt.ToShift) //recharge
	{
		dt.barAlpha = 255;
		dt.FastStop = false;
		dt.Charged++;
		dt.ChargedReverse--;
		dt.ToWait = DT_WAIT_CALLS;
		return;
	}
	else {
		dt.FastStop = false;
		dt.Recharging = false;
	}

	Func.Original<fn>()(accumulated_extra_samples, (dt.Shifting && !dt.ToWait) ? true : bFinalTick);

	if (dt.ToWait)
	{
		dt.FastStop = false;
		dt.ToWait--;
		return;
	}
	if (dt.Shifting)
	{
		const auto& pLocal = g_EntityCache.m_pLocal;
		if (!pLocal) // lol.
			return;

		if (GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) {
			dt.FastStop = true;
			while (dt.Charged != 0)
			{
				if (!dt.Shifting) {
					return;
				}
				if (!Vars::Misc::CL_Move::NotInAir.m_Var) {
					Func.Original<fn>()(accumulated_extra_samples, (dt.Charged == 1)); //this doubletaps
					dt.ChargedReverse++;
					dt.Charged--;
				}
				if (Vars::Misc::CL_Move::NotInAir.m_Var) {

					if (pLocal->IsOnGround()) {
						Func.Original<fn>()(accumulated_extra_samples, (dt.Charged == 1)); //this doubletaps
						dt.ChargedReverse++;
						dt.Charged--;
					}
					else {
						dt.FastStop = false;
						return;
					}
				}
			}
			dt.FastStop = false;
		}
		dt.FastStop = false;

		dt.Shifting = false;
	}

}