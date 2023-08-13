#pragma once
#include "../../Vars.h"

class CAutoVaccinator
{
public:
	void Run(CUserCmd* pCmd);

private:
	CBaseEntity* HealingTarget;
	bool CanPop = false;
};


inline CAutoVaccinator g_AutoVaccinator;