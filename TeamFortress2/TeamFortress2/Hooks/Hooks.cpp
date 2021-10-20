#include "Hooks.h"
#include "../SDK/SDK.h"
inline uintptr_t GetVFuncPtr(void* pBaseClass, unsigned int nIndex) {
	return static_cast<uintptr_t>((*static_cast<int**>(pBaseClass))[nIndex]);
}

void CHooks::Init()
{
	MH_Initialize();
	{
		EndSceneHook::Init();

		//ChatPrintfHook::Init();
		Scoreboard::IsPlayerDominated::Init();
	}

	if (g_Interfaces.Client)
	{
		using namespace ClientHook;

		Table.Init(g_Interfaces.Client);
		Table.Hook(PreEntity::index, &PreEntity::Hook);
		Table.Hook(PostEntity::index, &PostEntity::Hook);
		Table.Hook(ShutDown::index, &ShutDown::Hook);
		Table.Hook(FrameStageNotify::index, &FrameStageNotify::Hook);
		//Table.Hook(DispatchUserMessage::index, &DispatchUserMessage::Hook);
	}

	if (g_Interfaces.ViewRender)
	{
		using namespace ViewRenderHook;

		Table.Init(g_Interfaces.ViewRender);
		Table.Hook(LevelInit::index, &LevelInit::Hook);
		Table.Hook(LevelShutdown::index, &LevelShutdown::Hook);

	}

	if (g_Interfaces.ClientMode)
	{
		using namespace ClientModeHook;

		Table.Init(g_Interfaces.ClientMode);
		Table.Hook(OverrideView::index, &OverrideView::Hook);
		Table.Hook(ShouldDrawViewModel::index, &ShouldDrawViewModel::Hook);
		Table.Hook(CreateMove::index, &CreateMove::Hook);
		Table.Hook(DoPostScreenSpaceEffects::index, &DoPostScreenSpaceEffects::Hook);
	}

	if (g_Interfaces.Prediction)
	{
		using namespace PredictionHook;

		Table.Init(g_Interfaces.Prediction);
		Table.Hook(RunCommand::index, &RunCommand::Hook);
	}

	if (g_Interfaces.Surface)
	{
		using namespace SurfaceHook;

		Table.Init(g_Interfaces.Surface);
		Table.Hook(OnScreenSizeChanged::index, &OnScreenSizeChanged::Hook);
		Table.Hook(LockCursor::index, &LockCursor::Hook);
	}

	if (g_Interfaces.Panel)
	{
		using namespace PanelHook;

		Table.Init(g_Interfaces.Panel);
		Table.Hook(PaintTraverse::index, &PaintTraverse::Hook);
	}

	if (g_Interfaces.EngineVGui)
	{
		using namespace EngineVGuiHook;

		Table.Init(g_Interfaces.EngineVGui);
		Table.Hook(Paint::index, &Paint::Hook);
	}

	if (g_Interfaces.Engine)
	{
		using namespace EngineClientHook;

		Table.Init(g_Interfaces.Engine);
		Table.Hook(IsPlayingTimeDemo::index, &IsPlayingTimeDemo::Hook);
	}

	if (g_Interfaces.ModelRender)
	{
		using namespace ModelRenderHook;

		Table.Init(g_Interfaces.ModelRender);
		Table.Hook(DrawModelExecute::index, &DrawModelExecute::Hook);
		Table.Hook(ForcedMaterialOverride::index, &ForcedMaterialOverride::Hook);
	}

	if (g_Interfaces.UniformRandomStream)
	{
		using namespace UniformRandomStreamHook;

		Table.Init(g_Interfaces.UniformRandomStream);
		Table.Hook(RandInt::index, &RandInt::Hook);
	}

	while (!m_hwWindow) {
		m_hwWindow = WinAPI::FindWindowW(0, _(L"Team Fortress 2"));
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	WndProcHook::WndProc = (WNDPROC)SetWindowLongPtr(m_hwWindow, GWL_WNDPROC, (LONG_PTR)WndProcHook::Hook);

	//Inventory expander
	{
		CTFInventoryManager* InventoryManager = nullptr;
		InventoryManager = CTFInventoryManager::Get();

		CTFPlayerInventory* playerinventory = nullptr;
		playerinventory = InventoryManager->GetPlayerInventory();

		uintptr_t maxItems = GetVFuncPtr(playerinventory, 9);

		using namespace InventoryExpander;

		Func.Hook((reinterpret_cast<void*>(maxItems)), reinterpret_cast<void*>(Expand));
	}

	//EngineHook
	{
		using namespace EngineHook;

		//CL_Move
		{
			using namespace CL_Move;

			fn CLMove = reinterpret_cast<fn>(g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0"));
			Func.Hook(reinterpret_cast<void*>(CLMove), reinterpret_cast<void*>(Hook));
		}

		//CL_SendMove
		{

		}
	}

	//EndSceneHook::Init();

	//GetDrawPosition
	{
		using namespace GetDrawPositionHook;

		fn GetDrawPosition = reinterpret_cast<fn>(
			g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 81 EC ? ? ? ? 53 57 E8 ? ? ? ? 8B 08 89 4D C8 8B 48 04 8B 40 08")));

		Func.Hook(reinterpret_cast<void*>(GetDrawPosition), reinterpret_cast<void*>(Hook));
	}

	//PerformScreenSpaceEffects
	{
		using namespace PerformScreenSpaceEffects;

		DWORD dwAddy = g_Pattern.Find(
			_(L"client.dll"),
			_(L"E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 83 F8 01 75 64 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 85 FF 74 0E 8B 17")) + 0x1;

		fn PerformScreenSpaceEffects = reinterpret_cast<fn>(((*(PDWORD)(dwAddy)) + dwAddy + 0x4));

		Func.Hook(reinterpret_cast<void*>(PerformScreenSpaceEffects), reinterpret_cast<void*>(Hook));
	}

	//InCond
	{
		using namespace InCondHook;

		fn InCond = reinterpret_cast<fn>(
			g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 08 56 57 8B 7D 08 8B F1 83 FF 20")));

		Func.Hook(reinterpret_cast<void*>(InCond), reinterpret_cast<void*>(Hook));
	}

	//DrawStaticProps
	{
		using namespace DrawStaticPropsHook;

		fn DrawStaticProps = reinterpret_cast<fn>(
			g_Pattern.Find(_(L"engine.dll"), _(L"55 8B EC 83 EC 10 8B 15 ? ? ? ? 53 56 57 33 F6 89 4D FC 33 FF 89 75 F0 89 7D F4 8B 42 08")));

		Func.Hook(reinterpret_cast<void*>(DrawStaticProps), reinterpret_cast<void*>(Hook));
	}

	//SetColorModulation
	{
		using namespace SetColorModulationHook;

		fn SetColorModulation = reinterpret_cast<fn>(GetVFuncPtr(g_Interfaces.StudioRender, 27));

		Func.Hook(reinterpret_cast<void*>(SetColorModulation), reinterpret_cast<void*>(Hook));
	}
	
	{
		using namespace BulletTracers::FireBullet;

		//fn FN = reinterpret_cast<fn>(g_Pattern.Find(_(L"client.dll"), L"E8 ? ? ? ? 8B 45 20 47")) + 0x1;
		DWORD FireBulletAddress = g_Pattern.Find(_(L"client.dll"), _(L"E8 ? ? ? ? 8B 45 20 47")) + 1;
		fn FireBulletHook = reinterpret_cast<fn>(((*(PDWORD)(FireBulletAddress)) + FireBulletAddress + 0x4));
		UTIL_ParticleTracer = reinterpret_cast<UTIL_ParticleTracer_Fn>(g_Pattern.Find(L"client.dll", _(L"55 8B EC FF 75 08 E8 ? ? ? ? D9 EE 83")));
		Func.Hook(reinterpret_cast<void*>(FireBulletHook), reinterpret_cast<void*>(Hook));
		//fn FN = reinterpret_cast<fn>(g_Pattern.Find(_(L"client.dll"), _(L"E8 ? ? ? ? 8B 45 20 47")));
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_STATUS::MH_OK)
		WinAPI::MessageBoxW(0, _(L"MH failed to enable all hooks!"), _(L"ERROR!"), MB_ICONERROR);
}

void CHooks::Release()
{
	MH_Uninitialize();
	SetWindowLongPtr(m_hwWindow, GWL_WNDPROC, (LONG_PTR)WndProcHook::WndProc);
}