#include "PredictionHook.h"

void __stdcall PredictionHook::RunCommand::Hook(CBaseEntity* pEntity, CUserCmd* pCmd, CMoveHelper* pMoveHelper)
{
	Table.Original<fn>(index)(g_Interfaces.Prediction, pEntity, pCmd, pMoveHelper);

	if (pMoveHelper && !g_Interfaces.MoveHelper)
		g_Interfaces.MoveHelper = pMoveHelper;
}


int CalculateTick(int simTicks, CBaseEntity* player)
{
	int clockcorrect = TIME_TO_TICKS(0.06f); //sv_clockcorrectmsecs
	//int nIdealFinalTick = g_Interfaces.GlobalVars->tickcount + TIME_TO_TICKS(g_Interfaces.Engine->GetNetChannelInfo()->GetLatency(0)) + clockcorrect;
	int nIdealFinalTick = g_Interfaces.GlobalVars->tickcount + g_Interfaces.ClientState->m_ClockDriftMgr.m_nServerTick + clockcorrect;
	int EstimatedFinal = player->GetTickBase() + simTicks;
	int fast = nIdealFinalTick + clockcorrect;
	int slow = nIdealFinalTick - clockcorrect;
	if (EstimatedFinal > fast || EstimatedFinal < slow)
		return nIdealFinalTick - simTicks;
}


void __stdcall PredictionHook::RunCommand2::Hook(CBaseEntity* pEntity, CUserCmd* pCmd, CMoveHelper* pMoveHelper)
{
	if (pMoveHelper && !g_Interfaces.MoveHelper)
		g_Interfaces.MoveHelper = pMoveHelper;

	/*
	if (!Vars::Misc::CL_Move::Doubletap.m_Var || !dt.Shifting) {
		return Table.Original<fn>(index)(g_Interfaces.Prediction, pEntity, pCmd, pMoveHelper);
	}

	auto& pLocal = g_EntityCache.m_pLocal;

	if (!pEntity || !pEntity->IsAlive() || !pLocal || !pLocal->IsAlive() || pLocal != pEntity) {
		return Table.Original<fn>(index)(g_Interfaces.Prediction, pEntity, pCmd, pMoveHelper);
	}

	if (!pCmd || !g_GlobalInfo.shiftedCmd) {
		return Table.Original<fn>(index)(g_Interfaces.Prediction, pEntity, pCmd, pMoveHelper);
	}

	int BackupTick = pEntity->GetTickBase();
	float CurtimeBackup = g_Interfaces.GlobalVars->curtime;

	if (pCmd->command_number == g_GlobalInfo.shiftedCmd->command_number)
	{
		pEntity->SetTickBase(CalculateTick(g_Interfaces.ClientState->chokedcommands + dt.Charged + 1, pEntity));
		g_Interfaces.GlobalVars->curtime = TICKS_TO_TIME(pEntity->GetTickBase());
	}

	Table.Original<fn>(index)(g_Interfaces.Prediction, pEntity, pCmd, pMoveHelper);

	if (pCmd->command_number == g_GlobalInfo.shiftedCmd->command_number) {
		pEntity->SetTickBase(BackupTick);
		g_Interfaces.GlobalVars->curtime = CurtimeBackup;
	}
	*/ // dumb nigger, we have cl_sendmove rebuild now. We dont need this niggerlicious activities.
}