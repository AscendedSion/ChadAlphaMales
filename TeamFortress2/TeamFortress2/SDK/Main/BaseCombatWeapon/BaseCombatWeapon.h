
#pragma once

#include "../BaseEntity/BaseEntity.h"
#define OFFSETThing( name, type, offset ) inline type& name( ) { \
	return *reinterpret_cast< type* >( reinterpret_cast< uint32_t >( this ) + offset ); \
}
class CBaseCombatWeapon : public CBaseEntity
{
public: //Netvars
	M_DYNVARGET(Clip1, int, this, _("DT_BaseCombatWeapon"), _("LocalWeaponData"), _("m_iClip1"))
		M_DYNVARGET(Clip2, int, this, _("DT_BaseCombatWeapon"), _("LocalWeaponData"), _("m_iClip2"))
		M_DYNVARGET(ItemDefIndex, int, this, _("DT_EconEntity"), _("m_AttributeManager"), _("m_Item"), _("m_iItemDefinitionIndex"))
		M_DYNVARGET(ChargeBeginTime, float, this, _("DT_WeaponPipebombLauncher"), _("PipebombLauncherLocalData"), _("m_flChargeBeginTime"))
		M_DYNVARGET(ChargeDamage, float, this, _("DT_TFSniperRifle"), _("SniperRifleLocalData"), _("m_flChargedDamage"))
		M_DYNVARGET(LastFireTime, float, this, _("DT_TFWeaponBase"), _("LocalActiveTFWeaponData"), _("m_flLastFireTime"))
		M_DYNVARGET(NextSecondaryAttack, float, this, _("DT_BaseCombatWeapon"), _("LocalActiveWeaponData"), _("m_flNextSecondaryAttack"))
		M_DYNVARGET(NextPrimaryAttack, float, this, _("DT_BaseCombatWeapon"), _("LocalActiveWeaponData"), _("m_flNextPrimaryAttack"))

		M_OFFSETGET(UberCharge, float, 0xC6C) //DT_WeaponMedigun -> NonLocalTFWeaponMedigundata -> m_flChargeLevel
		//M_OFFSETGET(HealingTarget, int, 0xC48) //DT_WeaponMedigun -> m_hHealingTarget
		M_OFFSETGET(Healing, int, 0xC51) //DT_WeaponMedigun -> m_bHealing
		/*
		OFFSETThing(m_crit_token_bucket, float, 0xa54);
	OFFSETThing(m_weapon_mode, int, 0xb24);
	OFFSETThing(m_last_crit_check_time, float, 0xb54);
	OFFSETThing(m_last_rapid_fire_crit_check_time, float, 0xb60);
		*/
		OFFSETThing(CritTokenBucket, float, 0xa54);
		OFFSETThing(WeaponMode, int, 0xb24);
		OFFSETThing(LastCritCheckTime, float, 0xb54);
		OFFSETThing(LastRapidFireCritCheckTime, float, 0xb60);

public: //Virtuals
	M_VIRTUALGET(WeaponID, int, this, int(__thiscall*)(void*), 381)
	M_VIRTUALGET(Slot, int, this, int(__thiscall*)(void*), 330)
	M_VIRTUALGET(DamageType, int, this, int(__thiscall*)(void*), 340)	// old one was 378, but comparing to fedoraware, it decreased???
	M_VIRTUALGET(FinishReload, void, this, void(__thiscall*)(void*), 275)
	M_VIRTUALGET(BulletSpread, Vec3&, this, Vec3&(__thiscall*)(void*), 286)
	//M_VIRTUALGET(MuzzleAttachmenetFP, int, this, int(__thiscall*)(void*), 468)
	//M_VIRTUALGET(MuzzleAttachmenetTP, int, this, int(__thiscall*)(void*), 469)

public: //Everything else, lol
	__inline float GetSmackTime() {
		//credits to KGB
		static auto dwOffset = g_NetVars.get_offset(_("DT_TFWeaponBase"), _("m_nInspectStage")) + 0x1C;
		return *reinterpret_cast<float*>(this + dwOffset);
	}

	__inline CAttributeList* GetAttributeList() {
		static auto dwOff = g_NetVars.get_offset(_("DT_EconEntity"), _("m_AttributeManager"), _("m_AttributeList"));
		return reinterpret_cast<CAttributeList*>(this + dwOff);
	}

	__inline void SetItemDefIndex(const int nIndex) {
		static auto dwOff = g_NetVars.get_offset(_("DT_EconEntity"), _("m_AttributeManager"), _("m_Item"), _("m_iItemDefinitionIndex"));
		*reinterpret_cast<int*>(this + dwOff) = nIndex;
	}

	__inline bool GetAttachment(int number, Vec3& origin) {
		return GetVFunc<bool(__thiscall*)(void*, int, Vec3&)>(this, 71)(this, number, origin);
	}

	__inline CBaseEntity* GetHealingTarget() {
		return g_Interfaces.EntityList->GetClientEntityFromHandle(GetHealingTargetHandle());
	}

	__inline int GetHealingTargetHandle() {
		return *reinterpret_cast<int*>(this + 0xC48);
	}

	__inline WeaponData_t GetWeaponData() {
		static int offset = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 66 8B ? ? 66 3B 05 ? ? ? ? 73"));
		static auto get_tf_weapon_data_fn = reinterpret_cast<CTFWeaponInfo * (__cdecl*)(int)>(offset);
		return get_tf_weapon_data_fn(GetWeaponID())->m_WeaponData[0];
	}

	__inline float GetSwingRange(CBaseEntity* pLocal) {
		return static_cast<float>(GetVFunc<int(__thiscall*)(CBaseEntity*)>(this, 455)(pLocal));
	}

	__inline float GetWeaponSpread() {
		static auto GetWeaponSpreadFn = reinterpret_cast<float(__thiscall*)(decltype(this))>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 08 56 8B F1 57 6A 01 6A 00 8B 96 ? ? ? ? 8B 86 ? ? ? ? C1 E2 06 56 68 ? ? ? ? 51")));
		return GetWeaponSpreadFn(this);
	}

	__inline bool WillCrit() {
		static auto dwCalcIsAttackCritical = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 89 75 EC"));
		return reinterpret_cast<bool(__thiscall*)(decltype(this))>(dwCalcIsAttackCritical);
	}

	__inline bool DoSwingTrace(CGameTrace& Trace) {
		return GetVFunc<int(__thiscall*)(CGameTrace&)>(this, 454)(Trace);
	}

	__inline bool CanFireCriticalShot(const bool bHeadShot) {
		bool bResult = false;
		if (const auto& pOwner = g_Interfaces.EntityList->GetClientEntityFromHandle(GethOwner())) {
			const int nOldFov = pOwner->GetFov(); pOwner->SetFov(70);
			bResult = GetVFunc<bool(__thiscall*)(decltype(this), bool, CBaseEntity*)>(this, 425)(this, bHeadShot, nullptr);
			pOwner->SetFov(nOldFov);
		} return bResult;
	}

	__inline bool CanFireRandomCriticalShot(const float flCritChance) {
		return GetVFunc<bool(__thiscall*)(decltype(this), float)>(this, 424)(this, flCritChance);
	}

	__inline bool CanWeaponHeadShot() {
		return ((GetDamageType() & DMG_USE_HITLOCATIONS) && CanFireCriticalShot(true)); //credits to bertti
	}

	__inline bool CanShoot(CBaseEntity* pLocal) {
		if (!pLocal->IsAlive() || pLocal->IsTaunting() || pLocal->IsBonked() || pLocal->IsAGhost() || pLocal->IsInBumperKart())
			return false;

		if (pLocal->GetClassNum() == CLASS_SPY)
		{
			{ //DR
				static float flTimer = 0.0f;

				if (pLocal->GetFeignDeathReady()) {
					flTimer = 0.0f;
					return false;
				}
				else {
					if (!flTimer)
						flTimer = g_Interfaces.GlobalVars->curtime;

					if ((g_Interfaces.GlobalVars->curtime - flTimer) < 0.4f)
						return false;
				}
			}

			{ //Invis
				static float flTimer = 0.0f;

				if (pLocal->IsCloaked()) {
					flTimer = 0.0f;
					return false;
				}
				else {
					if (!flTimer)
						flTimer = g_Interfaces.GlobalVars->curtime;

					if ((g_Interfaces.GlobalVars->curtime - flTimer) < 2.0f)
						return false;
				}
			}
		}

		return (GetNextPrimaryAttack() < pLocal->GetTickBase() * g_Interfaces.GlobalVars->interval_per_tick);
	}

	__inline bool CanSecondaryAttack(CBaseEntity* pLocal) {
		if (!pLocal->IsAlive() || pLocal->IsTaunting() || pLocal->IsBonked() || pLocal->IsAGhost() || pLocal->IsInBumperKart())
			return false;

		return (GetNextSecondaryAttack() < (static_cast<float>(pLocal->GetTickBase()) * g_Interfaces.GlobalVars->interval_per_tick));
	}

	__inline bool IsInReload() {
		static DWORD dwNextPrimaryAttack = g_NetVars.get_offset(_("DT_BaseCombatWeapon"), _("LocalActiveWeaponData"), _("m_flNextPrimaryAttack"));
		bool m_bInReload = *reinterpret_cast<bool*>(this + (dwNextPrimaryAttack + 0xC));
		int m_iReloadMode = *reinterpret_cast<int*>(this + 0xB28);
		return (m_bInReload || m_iReloadMode != 0);
	}

	__inline bool CalcIsAttackCriticalHelper() {
		return false;
	}

	__inline void GetSpreadAngles(Vec3& vOut) {
		static auto GetSpreadAnglesFn = reinterpret_cast<void(__thiscall*)(decltype(this), Vec3&)>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 85 F6 74 10 8B 06 8B CE 8B 80 ? ? ? ? FF D0 84 C0 75 02")));
		GetSpreadAnglesFn(this, vOut);
	}

	__inline void GetProjectileFireSetup(CBaseEntity* pPlayer, Vec3 vOffset, Vec3* vSrc, Vec3* vForward, bool bHitTeam, float flEndDist) {
		typedef void(__thiscall* EstimateAbsVelocityFn)(decltype(this), CBaseEntity*, Vec3, Vec3*, Vec3*, bool, float);
		static DWORD dwFn = g_Pattern.Find(_(L"client.dll"), _(L"E8 ? ? ? ? F3 0F 10 46 ? F3 0F 5C 05 ? ? ? ? F3 0F 11 46 ? 5E 5D C2 20 00")) + 0x1;
		static DWORD dwOffset = ((*(PDWORD)(dwFn)) + dwFn + 0x4);
		EstimateAbsVelocityFn fn = (EstimateAbsVelocityFn)dwOffset;
		fn(this, pPlayer, vOffset, vSrc, vForward, bHitTeam, flEndDist);
	}

	__inline Vec3 GetSpreadAngles() {
		Vec3 vOut; GetSpreadAngles(vOut); return vOut;
	}

	__inline int GetMinigunState() {
		return *reinterpret_cast<int *>(this + 0xC48);
	}
	bool CalcIsAttackCritical() {
		typedef bool(__thiscall* calc_is_attack_critical_fn)(CBaseCombatWeapon*);
		return GetVFunc< calc_is_attack_critical_fn >(this, 396)(this);
	}
};
