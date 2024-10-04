#pragma once

#include "../BaseEntity/BaseEntity.h"

#define MULTIPLAYER_BACKUP 90
#define MAX_NEW_COMMANDS_HEAVY 20
struct GlobalInfo_t
{
	int m_net_sendto					= 0;
	int m_nCurrentTargetIdx				= 0;
	int m_nCurItemDefIndex              = 0;
	bool m_bWeaponCanHeadShot			= false;
	bool m_bWeaponCanAttack				= false;
	bool m_bWeaponCanSecondaryAttack	= false;
	bool m_bAAActive					= false;
	bool m_bHitscanSilentActive			= false;
	bool m_bProjectileSilentActive		= false; //flamethrower
	bool m_bAutoBackstabRunning			= false;
	bool m_bHitscanRunning				= false;
	bool m_bSilentTime					= false;
	bool m_bLocalSpectated				= false;
	bool m_bAttacking			 	    = false;
	bool m_bModulateWorld				= true;
	bool Unload							= false;
	float m_flCurAimFOV					= 0.0f;
	int m_nShifted = MAX_NEW_COMMANDS_HEAVY;
	float m_Latency = 0;
	VMatrix m_WorldToProjection = {};
	Vec3 m_vPredictedPos				= {};
	Vec3 m_vAimPos						= {};
	Vec3 m_vViewAngles					= {};
	Vec3 m_vRealViewAngles				= {};
	Vec3 m_vFakeViewAngles				= {};
	Vec3 m_vPunchAngles					= {};
	EWeaponType m_WeaponType			= {};
	std::vector<int> storedmis			{ 0 };
	CUserCmd* shiftedCmd{};
	int lastChlTick = 0;
};

#define DT_WAIT_CALLS 26

struct DoubletapInfo {
	bool Recharging;
	bool Warp;
	bool DoubleTap;
	bool AntiWarp;
	bool shifting = false;
	int ticks = 0;
	int Shifted = 0;
	int DTBarStyle = 1;
	CUserCmd* shift_user_cmd = nullptr; // retarded!!!!!!
	BYTE barAlpha = 127;
};

inline GlobalInfo_t g_GlobalInfo;

inline DoubletapInfo dt;