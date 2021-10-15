#pragma once

#include "../BaseEntity/BaseEntity.h"

#define MULTIPLAYER_BACKUP 90

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
	bool m_bRollExploiting			    = false;
	bool m_bAttacking			 	    = false;
	bool m_bModulateWorld				= true;
	bool Unload							= false;
	float m_flCurAimFOV					= 0.0f;
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
};

#define DT_WAIT_CALLS 26

struct DoubletapInfo {
	bool Shifting = false;
	bool Recharging = false;
	bool FastStop = false;
	int Charged = 0;
	int ToShift = 20;
	int ToWait = 0;
	BYTE barAlpha = 255;
};

inline GlobalInfo_t g_GlobalInfo;

inline DoubletapInfo dt;