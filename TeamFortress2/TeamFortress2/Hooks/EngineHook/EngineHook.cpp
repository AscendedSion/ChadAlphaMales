#include "EngineHook.h"

#include "../../Features/Vars.h"

#include "../../Features/Misc/Misc.h"
#define NUM_NEW_COMMAND_BITS		4
#define MAX_NEW_COMMANDS			((1 << NUM_NEW_COMMAND_BITS)-1)
#define NUM_BACKUP_COMMAND_BITS		3
#define MAX_BACKUP_COMMANDS			((1 << NUM_BACKUP_COMMAND_BITS)-1)
#define MAX_PLAYER_NAME_LENGTH		32
#define MAX_CUSTOM_FILES		4		// max 4 files
#define NETMSG_TYPE_BITS	6	// must be 2^NETMSG_TYPE_BITS > SVC_LASTMSG
void __fastcall EngineHook::CL_SendMove::Hook(void* ecx, void* edx)
{
	if (!g_Interfaces.ClientState || !g_Interfaces.ClientState->m_NetChannel) //|| G->DetachSystem->WantsDetach)
		return Func.Original<fn>()(ecx, edx);

	byte data[4000];
	const int Chocked = ((CNetChannel*)g_Interfaces.ClientState->m_NetChannel)->m_nChokedPackets;
	const int NextCommandNumber = g_Interfaces.ClientState->lastoutgoingcommand + Chocked + 1;

	CLC_Move Message;
	Message.m_DataOut.StartWriting(data, sizeof(data));

	Message.m_nNewCommands = 1 + Chocked;
	Message.m_nNewCommands = std::clamp(Message.m_nNewCommands, 0, MAX_NEW_COMMANDS);
	const int ExtraCommands = (Chocked + 1) - Message.m_nNewCommands;
	const int BackupCommands = std::max(2, ExtraCommands);
	Message.m_nBackupCommands = std::clamp(BackupCommands, 0, MAX_BACKUP_COMMANDS);

	const int numcmds = Message.m_nNewCommands + Message.m_nBackupCommands;

	int from = -1;
	bool bOK = true;
	for (int to = NextCommandNumber - numcmds + 1; to <= NextCommandNumber; to++) {
		const bool isnewcmd = to >= NextCommandNumber - Message.m_nNewCommands + 1;
		bOK = bOK && g_Interfaces.Client->WriteUsercmdDeltaToBuffer(&Message.m_DataOut, from, to, isnewcmd);
		from = to;
	}

	if (bOK) {
		if (ExtraCommands)
			((CNetChannel*)g_Interfaces.ClientState->m_NetChannel)->m_nChokedPackets -= ExtraCommands;

		GetVFunc<bool(__thiscall*)(void*, INetMessage*, bool, bool)>(g_Interfaces.ClientState->m_NetChannel, 37)(g_Interfaces.ClientState->m_NetChannel, &Message, false, false);
	}

	//NoSpread->CL_SendMovePostHandler();
}

void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
{
	/*
	if (Vars::Misc::CL_Move::TeleportKey.m_Var && (GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var)) && g_GlobalInfo.m_nShifted)
	{   // warp
		while (g_GlobalInfo.m_nShifted);
		{
			//Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted));
			g_GlobalInfo.m_nShifted++;
			dt.Charged--;
		}
		return;
	}

	if (Vars::Misc::CL_Move::Doubletap.m_Var)
	{
		dt.FastStop = true;
		
		if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) {//recharge key
			dt.FastStop = false;
			dt.Recharging = true;
		}
	}

	if (dt.Recharging && dt.Charged < Vars::Misc::CL_Move::DTTicks.m_Var) //recharge
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
					g_Interfaces.Engine->FireEvents();
				}
				if (Vars::Misc::CL_Move::NotInAir.m_Var) {

					if (pLocal->IsOnGround()) {
						Func.Original<fn>()(accumulated_extra_samples, (dt.Charged == 1)); //this doubletaps
						dt.FastStop = true;  	
						dt.ChargedReverse++;
						dt.Charged--;
						g_Interfaces.Engine->FireEvents();
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
	*/

	if (!Vars::Misc::CL_Move::Doubletap.m_Var || !g_Interfaces.Engine->IsConnected())
	{
		return Func.Original<fn>()(accumulated_extra_samples, bFinalTick);
	}
	if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var))
	{
		dt.Recharging = true;
	}
	if (dt.Shifted < 22 && dt.Recharging)
	{
		dt.Shifted++;
		return;
	}
	else
	{
		dt.Recharging = false;
	}

	//oCLMove(accumulated_extra_samples, final_tick);
	Func.Original<fn>()(accumulated_extra_samples, bFinalTick);

	if (Vars::Misc::CL_Move::Doubletap.m_Var)
	{

		if (dt.shifting)
		{
			while (dt.Shifted > dt.ticks)
			{
				//accumulated_extra_samples = 0.0f;
				//oCLMove(accumulated_extra_samples, Vars::DT::Shifted == Vars::DT::ticks + 1);
				Func.Original<fn>()(accumulated_extra_samples, dt.Shifted == dt.ticks + 1);
				//i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "[Doubletap] CL_Move( %i - %.8f, %d )\n", global_info.shifted, accumulated_extra_samples, global_info.shifted == global_info.ticks + 1 );
				dt.Shifted--;
				g_Interfaces.Engine->FireEvents();
			}

			dt.shifting = false;
		}
	}
}