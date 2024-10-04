#pragma once

#include "../../SDK/SDK.h"

namespace EngineHook
{
	namespace CL_Move
	{
		inline SEOHook::Func Func;

		using fn = void(__cdecl*)(float, bool);
		void __cdecl Hook(float accumulated_extra_samples, bool bFinalTick);
	}

	namespace sendto {

	}
	namespace CL_SendMove
	{
		inline SEOHook::Func Func;

		using fn = void(__fastcall*)(void*, void*);
		void __fastcall Hook(void* ecx, void* edx);
	}
}