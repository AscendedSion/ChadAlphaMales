#include "CritHack.h"
#include "../Vars.h"
// jengaware crithack. 

void c_crithack::run(CUserCmd* cmd) {
	const auto& pLocal = g_EntityCache.m_pLocal;
	const auto& pWeapon = g_EntityCache.m_pLocalWeapon;
	if (!pLocal || !pLocal->IsAlive() || !pWeapon)//|| tf2::is_non_aimable(ctx->m_local_weapon))
		return;
	auto uh = pWeapon->GetItemDefIndex();
	if (uh == Pyro_s_GasPasser
		|| uh == TF_WEAPON_JAR
		|| uh == TF_WEAPON_JAR_MILK)
		return;

	//run crit indicator here since its related to the crithack now
	//run_crit_indicator_calc();

	if (!Vars::Aimbot::CritHack::Active.m_Var) // no idea why but its in aimbot. too lazy to move it out from there.
		return;

	if (!GetAsyncKeyState(Vars::Aimbot::CritHack::CritKey.m_Var))
	{
		if (!pLocal->IsCritBoosted())
			skip_random_crit(cmd);

		return;
	}

	if (cmd->command_number <= m_crit_num && m_crit_num - cmd->command_number <= m_max_attempts) {
		cmd->command_number = m_crit_num;
		return;
	}

	if (!find_crit_command(pWeapon, cmd))
		m_crit_num = cmd->command_number;

	cmd->command_number = m_crit_num;
	m_random_seed_force = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;
}

void c_crithack::skip_random_crit(CUserCmd* cmd) {
	//skip random crits, and find out if weapon can randomly crit.
	const auto& pWeapon = g_EntityCache.m_pLocalWeapon;
	auto old_crit_token_bucket = pWeapon->CritTokenBucket();
	auto old_seed = *g_Interfaces.RandomSeed;
	int old_weapon_mode = pWeapon->WeaponMode();
	for (int i = 0; i < 10; i++) {
		*g_Interfaces.RandomSeed = MD5_PseudoRandom(cmd->command_number + i) & INT_MAX;
		if (!pWeapon->CalcIsAttackCritical()) {
			cmd->command_number += i;
			break;
		}
	}
	pWeapon->WeaponMode() = old_weapon_mode;
	*g_Interfaces.RandomSeed = old_seed;
	pWeapon->CritTokenBucket() = old_crit_token_bucket;
}

bool c_crithack::find_crit_command(CBaseCombatWeapon* weapon, CUserCmd* cmd) {
	int old_seed = *g_Interfaces.RandomSeed;
	int old_weapon_mode = weapon->WeaponMode();
	float old_crit_token_bucket = weapon->CritTokenBucket(); //we only need this stable.
	m_is_going_through_crit_cmds = true; //need this var to fix addtocritbucket.

	for (auto i = 0; i < m_max_attempts; i++) {
		*g_Interfaces.RandomSeed = MD5_PseudoRandom(cmd->command_number + i) & INT_MAX;
		weapon->WeaponMode() = 0;
		weapon->CritTokenBucket() = 1000.0f;
		weapon->LastCritCheckTime() = 0.0f;
		weapon->LastRapidFireCritCheckTime() = 0.0f;

		if (weapon->CalcIsAttackCritical()) {
			m_crit_num = cmd->command_number + i;
			*g_Interfaces.RandomSeed = old_seed;
			weapon->WeaponMode() = old_weapon_mode;
			m_is_going_through_crit_cmds = false;
			weapon->CritTokenBucket() = old_crit_token_bucket;
			return true;
		}
		weapon->WeaponMode() = old_weapon_mode;
		weapon->CritTokenBucket() = old_crit_token_bucket;
	}

	*g_Interfaces.RandomSeed = old_seed;
	weapon->WeaponMode() = old_weapon_mode;
	weapon->CritTokenBucket() = old_crit_token_bucket;
	m_is_going_through_crit_cmds = false;

	return false;
}