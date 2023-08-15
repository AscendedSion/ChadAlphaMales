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

	/*g_Interfaces.CVars->ConsoleColorPrintf({0, 155, 255, 255}, tfm::format("[!] Message ID: %d", type).c_str());

	if (buf.IsOverflowed()) {
		return false;
	}

	int s, i;
	char c;
	const char* buf_data = reinterpret_cast<const char*>(buf.m_pData);

	std::string data;
	switch (type) {
	case 4:
		s = buf.GetNumBytesLeft();
		if (s >= 256 || !g_EntityCache.m_pLocal) {
			break;
		}
		for (i = 0; i < s; i++) {
			data.push_back(buf_data[i]);
		}

		const char* p = data.c_str() + 2;
		std::string event(p), name((p += event.size() + 1)), message(p + name.size() + 1);
		g_Interfaces.CVars->ConsoleColorPrintf({ 0, 155, 255, 255 }, tfm::format("[!] Event: %s", event).c_str());
		g_Interfaces.CVars->ConsoleColorPrintf({ 0, 155, 255, 255 }, tfm::format("[!] Name: %s", name).c_str());
		g_Interfaces.CVars->ConsoleColorPrintf({ 0, 155, 255, 255 }, tfm::format("[!] Message: %s", message).c_str());
		buf.Seek(0);
		break;
	}

	return true;//oDispatchUserMessage(g_Interfaces.Client, type, buf);
		/*if (event.find("TF_Chat") == 0) {
			PlayerInfo_t pi;
			if (g_Interfaces.Engine->GetPlayerInfo(g_Interfaces.Engine->GetLocalPlayer()))
		}*/

	if (type == 25) {
		return true;
	}
	return oDispatchUserMessage(g_Interfaces.Client, type, buf);
}
	/*g_Interfaces.CVars->ConsoleColorPrintf({0, 155, 255, 255}, tfm::format("[!] Message ID: %d", iMessageID).c_str());

	if (pBFMessage.IsOverflowed()) {
		return oDispatchUserMessage(g_Interfaces.Client, pDumbArg, iMessageID, pBFMessage);
	}

	bool bCallOriginal = true;2

	switch (iMessageID) {
	case 45:
	{
		int iReason = pBFMessage.ReadByte();
		int iSeconds = pBFMessage.ReadShort();
		std::string const str({ '\x7', 'F', 'F', '0', '0', 'F', 'F' });
		g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%sReason: %d", str, iReason).c_str());
		g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%sSeconds: %d", str, iSeconds).c_str());
		pBFMessage.Seek(0);
		break;
	}
	case 46:
	{
		int iTeam = pBFMessage.ReadByte();
		int iCaller = pBFMessage.ReadByte();
		char cReason[64], cDummyName[64];
		pBFMessage.ReadString(cReason, 64, false, nullptr);
		pBFMessage.ReadString(cDummyName, 64, false, nullptr);
		int iTarget = (int)(((unsigned char)pBFMessage.ReadByte()) >> 1);
		//CCatConnect::OnPotentialVoteKickStarted(iTeam, iCaller, iTarget, cReason);
		std::string const str({ '\x7', 'F', 'F', '0', '0', 'F', 'F' });
		g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%sTeam: %d", str, iTeam).c_str());
		g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%sCaller: %d", str, iCaller).c_str());
		g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%sReason: %s", str, cReason).c_str());
		g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%sDummyName: %s", str, cDummyName).c_str());
		g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%sTarget: %d", str, iTarget).c_str());
		pBFMessage.Seek(0);
		break;
	}
	}
	return oDispatchUserMessage(g_Interfaces.Client, pDumbArg, iMessageID, pBFMessage);*/



//using WriteUserCmd_t = void(__fastcall*)(void*, CUserCmd*, CUserCmd*);
//
//const int MAX_USERCMD_LOSS = 10;
//const int MAX_USERCMDS_SEND = 62;
//
//uintptr_t WriteUserCmd_offset = g_Pattern.Find(L"client.dll", L"55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");
//
//void WriteUserCmd(bf_write* buffer, CUserCmd* to, CUserCmd* from) {
//	static auto WriteUserCmdFn = reinterpret_cast<WriteUserCmd_t>(WriteUserCmd_offset);
//	__asm
//	{
//		mov ecx, buffer;
//		mov edx, to;
//		push from;
//		call WriteUserCmdFn;
//		add esp, 4h;
//	}
//}
//bool __stdcall ClientHook::WriteUserCmdDeltaToBuffer::Hook(bf_write* buffer, int from, int to, bool isNewCmd) {
//	/*std::cout << from << std::endl;
//	std::cout << buffer << std::endl;
//	std::cout << to << std::endl;
//	std::cout << isNewCmd << std::endl;
//	static auto oWriteUserCmdDeltaToBuffer = Table.Original<fn>(index);
//	if (g_GlobalInfo.m_nShifted >= 0) {
//		return oWriteUserCmdDeltaToBuffer(g_Interfaces.Client, buffer, from, to, isNewCmd);
//	}
//
//	if (from != -1) {
//		return true;
//	}
//
//	int tickbase = g_GlobalInfo.m_nShifted;
//
//	g_GlobalInfo.m_nShifted = 0;
//	//auto state = g_Interfaces.ClientState;
//	int* pNewCmds			= (int*)((uintptr_t)buffer - 0x2C);
//	int* pBackupCmds		= (int*)((uintptr_t)buffer - 0x30);
//	auto newCmds			= pNewCmds;
//
//	*pBackupCmds = 0;
//
//	int iNewCmds			= *pNewCmds;
//	int iNextCmds			= g_Interfaces.ClientState->chokedcommands + g_Interfaces.ClientState->lastoutgoingcommand + 1;
//	int iTotalNewCmds =		std::min(iNewCmds + abs(tickbase), 22);
//
//	*pNewCmds = iTotalNewCmds;
//	for (to = iNextCmds - iNewCmds + 1; to <= iNextCmds; to++)
//	{
//		if (!oWriteUserCmdDeltaToBuffer(g_Interfaces.Client, buffer, from, to, true)) {
//			return false;
//		}
//		from = to;
//	}
//	CUserCmd* pCmd = g_Interfaces.Input->GetUserCmd(from);
//	if (!pCmd) {
//		return true;
//	}
//	CUserCmd toCmd = *pCmd, fromCmd = *pCmd;
//	toCmd.command_number++;
//	toCmd.tick_count += 3 * g_Interfaces.GlobalVars->tickcount;
//	for (int i = 0; i <= abs(tickbase); i++) {
//		WriteUserCmd(buffer, &toCmd, &fromCmd);
//		fromCmd = toCmd;
//		toCmd.command_number++;
//		toCmd.tick_count++;
//	}
//
//	return true;*/
//	return Table.Original<fn>(index)(g_Interfaces.Client, buffer, from, to, isNewCmd);
//}

