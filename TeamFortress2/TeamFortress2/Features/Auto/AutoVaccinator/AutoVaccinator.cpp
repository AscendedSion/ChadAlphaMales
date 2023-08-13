/*
	Mad spoonfeed
*/
#include "AutoVaccinator.h"

void CAutoVaccinator::Run(CUserCmd* pCmd)
{
	if (!Vars::Triggerbot::AutoVaccinator::Active.m_Var)
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		// Player checks
		if (!pLocal->IsAlive() || pLocal->GetClassNum() != ETFClass::CLASS_MEDIC)
			return;

		auto Weapon = pLocal->GetActiveWeapon();

		// Weapon checks
		if (!Weapon || Weapon->GetSlot() != EWeaponSlots::SLOT_SECONDARY || !Weapon->CanSecondaryAttack(pLocal) || Weapon->GetItemDefIndex() == Medic_s_TheVaccinator)
			return;

		// Acquire healing target
		const auto HealingTarget = Weapon->GetHealingTarget();

		/* 
		* 0 - Bullet resistance
		* 1 - Blast resistance
		* 2 - Fire resistance
		*/
		int Resistance = 0;

		//Save best enemy
		CBaseEntity* BestEnemy{ nullptr };
		// Save Entity owner, used later for resistance checks
		CBaseEntity* EntityOwner{ nullptr };

		//Is the enemy a projectile?
		bool IsProjectile = false;
		// Max distance for auto vacc to work
		float BestEnemyDistance = 1800.f;

		for (const auto& Player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			if (Vars::Triggerbot::Uber::OnlyFriends.m_Var && !g_EntityCache.Friends[Player->GetIndex()])
				continue;
			
			if (!Player || !Player->IsAlive() || Player->GetDormant())
				continue;

			float Distance = Player->GetVecOrigin().DistTo(HealingTarget != nullptr ? HealingTarget->GetVecOrigin() : pLocal->GetVecOrigin());
			//g_Interfaces.DebugOverlay->AddTextOverlay(Player->GetVecOrigin(), 0.1f, "Distance: %.0f", Distance);

			// If the player is a sniper, completly ignore distance checks
			if (Player->GetClassNum() == ETFClass::CLASS_SNIPER)
			{
				// if the sniper can see us, welp use that as best enemy and the player is scoped
				if (Utils::VisPos(Player, HealingTarget ? HealingTarget : pLocal, Player->GetWorldSpaceCenter(),
					(HealingTarget ? HealingTarget->GetWorldSpaceCenter() : pLocal->GetWorldSpaceCenter())) && Player->GetCond() & TFCond_Zoomed)
				{
					BestEnemy = Player;
					BestEnemyDistance = 1800.f;
					IsProjectile = false;
				}
			}
			else if (Distance < BestEnemyDistance)
			{
				BestEnemy = Player;
				BestEnemyDistance = Distance;
				IsProjectile = false;
			}
		}

		for (const auto& Projectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
		{
			if (!Projectile)
				continue;

			auto Owner = g_Interfaces.EntityList->GetClientEntityFromHandle(Projectile->GethOwner());
			if (!Owner || Owner->GetTeamNum() == pLocal->GetTeamNum() || Owner == pLocal)
				continue;

			float Distance = Projectile->GetVecOrigin().DistTo(HealingTarget != nullptr ? HealingTarget->GetVecOrigin() : pLocal->GetVecOrigin());

			if (Distance < BestEnemyDistance)
			{
				BestEnemy = Projectile;
				EntityOwner = Owner;
				BestEnemyDistance = Distance;
				IsProjectile = true;
			}
		}

		if (BestEnemy)
		{
			// Get resistance type needed
			if (!IsProjectile)
			{
				if (BestEnemy->GetClassNum() == ETFClass::CLASS_ENGINEER || BestEnemy->GetClassNum() == CLASS_HEAVY || BestEnemy->GetClassNum() == CLASS_SNIPER
					|| BestEnemy->GetClassNum() == CLASS_SCOUT)
					Resistance = 0;
				else if (BestEnemy->GetClassNum() == ETFClass::CLASS_SOLDIER || BestEnemy->GetClassNum() == ETFClass::CLASS_DEMOMAN)
					Resistance = 1;
				else Resistance = 2;
			}
			else
			{
				if (EntityOwner)
				{
					if (EntityOwner->GetClassNum() == ETFClass::CLASS_SOLDIER || EntityOwner->GetClassNum() == ETFClass::CLASS_DEMOMAN)
						Resistance = 1;
					else if (EntityOwner->GetClassNum() == ETFClass::CLASS_PYRO)
						Resistance = 2;
					else Resistance = 0;
					// Draw line to entity owner
					g_Interfaces.DebugOverlay->AddLineOverlay(pLocal->GetVecOrigin(), EntityOwner->GetVecOrigin(), 255, 0, 0, true, 0.1f);
				}
			}

			if (BestEnemy->GetCond() & TFCond_OnFire)
				Resistance = 2;

			// Draw line to entity we are targetting
			//g_Interfaces.DebugOverlay->AddLineOverlay(pLocal->GetVecOrigin(), BestEnemy->GetVecOrigin(), 0, 255, 0, true, 0.1f);
		}

		// m_nChargeResistType
		int ResistanceType = *reinterpret_cast<int*>(Weapon + 0xC70); // mad is so smelly, why not just use netvars...

		if (Resistance != ResistanceType)
		{
			// Delay is needed to not infinitly swap between charges
			static int Delay = 0;
			if (Delay > 8)
			{
				Delay = 0;
				pCmd->buttons |= IN_RELOAD;
			}
			else
			{
				Delay++;
				pCmd->buttons &= ~(IN_RELOAD);
			}
		}

		int Charges = *reinterpret_cast<float*>(Weapon + 0xC6C) / 25;
		if (Resistance == ResistanceType && Charges)
		{
			if (HealingTarget)
			{
				float Ratio = HealingTarget->GetHealth() / HealingTarget->GetMaxHealth();
				if (Ratio <= Vars::Triggerbot::AutoVaccinator::UberMaxPercentage.m_Var/*70.f*/)
				{
					static int Delay = 0;
					if (Delay > 9)
					{
						Delay = 0;
						pCmd->buttons |= IN_ATTACK2;
					}
					else
					{
						Delay++;
						pCmd->buttons &= ~(IN_ATTACK2);
					}
				}
			}
		}

		// TODO: handle low health (nearby enemies, afterburn etc.)
	}
}