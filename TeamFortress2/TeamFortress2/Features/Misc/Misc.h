#pragma once

#include "../../SDK/SDK.h"

#pragma once

#include <optional>
#include <string>
/*
namespace hacks::tf2::steamidstealer
{

    class IDStorage
    {
    public:
        unsigned steamid;
        std::string reason;
        std::string name;
        IDStorage()
        {
            steamid = 0;
            reason = "";
            name = "";
        }
        IDStorage(unsigned steamid, std::string reason, std::string name = "")
        {
            this->steamid = steamid;
            this->reason = reason;
            this->name = name;
        }
    };

    std::optional<IDStorage> GetSteamID();
    void SetSteamID(std::optional<IDStorage> steamid);
    void SendNetMessage(INetMessage& msg);
}
*/
class CMisc
{
private:
	void AutoJump(CUserCmd *pCmd);
	void AutoStrafe(CUserCmd* pCmd);
	void NoiseMakerSpam();
	void InitSpamKV(void* pKV);
	void ChatSpam();
    void InstantRespawn();
    void NoPush();
public:
    void AntiWarp(CUserCmd* pCmd);
    Vec3 m_strafe_angles = { };
    bool m_pressing_move = false;

	void Run(CUserCmd *pCmd);
    Vector predPosAt(float flTime, CBaseEntity* target);
    void CheatsBypass();
    void EdgeJump(CUserCmd* pCmd, const int nOldFlags);
	void BypassPure();
	void AutoRocketJump(CUserCmd *pCmd);
    void ChangeName(std::string name);
};

inline CMisc g_Misc;