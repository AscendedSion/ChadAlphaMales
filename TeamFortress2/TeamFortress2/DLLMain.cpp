#include "Hooks/Hooks.h"

#include "Features/Glow/Glow.h"
#include "Features/Chams/Chams.h"
#include "Features/Chams/DMEChams.h"
#include "Features/Visuals/Visuals.h"
#include "Utils/DiscordRPC/Discord.h"
#include "Utils/Events/Events.h"
#include "Features/Menu/Menu.h"
#include "Features/Playerlist/Playerlist.h"

Discord* g_DiscordRPC;

// I know that this is a pretty bad idea because we're basically leaking the CAM user's accounts, but atm this is our only auth check in the dll itself
UINT64 steamids[] = {
	76561199203165083, // Sten Testing alt
	76561198300153627, // Sten Main
	76561197999062092, // Legacy Alt
	76561198125167280, // Legacy Main
	76561198839612863, // M-Fed main...? (correct me if i'm wrong)
	76561199039632223, // Kanna
	76561198887997678, // Mount 
	76561199185135294, // Null
	76561198346749036, // c1R
	76561199205346811, // Talon / umbuku
	76561198304525476, // s0yi
	//76561199010783922, // Bendy
	//76561198445705497, // Stav (maybe wrong)
	76561198865445928, // Stav
	76561198213922667, // jor
    76561197960570183, // c1R new account 
	76561199116719515, // legacy F2P alt for testing uwu uwu uwu uwu 
	76561198304525476, // mis 1
	76561198399908684, // mis 2 s0yi
	76561199064409522,// lolnoenemy
	76561198144626197,// Timing 
	76561197960692665,// Azure main he might ever inject on
	76561198838798126,// Azure cheating account
	76561198868137670,// Nakori (old old friend of legacy and some others)
	76561199086788469,// rico
	
};
// this isn't even used for anything other than storing

using namespace std::chrono; //lol??? 

int StringToWString(std::wstring& ws, const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws = wsTmp;

	return 0;
}


DWORD WINAPI MainThread(LPVOID lpParam)
{
	/*
		"mss32.dll" being one of the last modules to be loaded
		So wait for that before proceeding, after it's up everything else should be too
		Allows us to correctly use autoinject and just start the game.
		Adding "ntdll.dll" seems to fix SOME of the crashing issues which is kind of funny
	*/

	while (!WinAPI::GetModuleHandleW(_(L"mss32.dll")) || !WinAPI::GetModuleHandleW(_(L"ntdll.dll")) || !WinAPI::GetModuleHandleW(_(L"stdshader_dx9.dll")))
		std::this_thread::sleep_for(std::chrono::seconds(5));
	
	/*
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	SetConsoleTitleA("nig");
	*/
	g_SteamInterfaces.Init();

	static CSteamID steamid = g_SteamInterfaces.User->GetSteamID();
	static UINT64 steamid64 = steamid.ConvertToUint64();
	size_t myArraySize = sizeof(steamids) / sizeof(int);
	UINT64* end = steamids + myArraySize;
	if (std::find(steamids, end, steamid64) == end) {
		//MessageBoxW(NULL,(LPCWSTR)L"fuck you",(LPCWSTR)L"fuck you", MB_ICONSTOP);
		//std::exit(EXIT_FAILURE);
		//return 0;
	}

	g_Interfaces.Init();
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("unbind f7"));
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("cl_vote_ui_active_after_voting 1"));
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("cl_timeout 99999"));
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("clear"));
	g_Interfaces.CVars->ConsoleColorPrintf({ 0, 155, 255, 255 }, _("[!] Initializing stuff...\n"));
	g_NetVars.Init();
	g_Glow.Init();
	g_Chams.Init();
	g_DMEChams.Init();
	g_dwDirectXDevice = **reinterpret_cast<DWORD**>(g_Pattern.Find(_(L"shaderapidx9.dll"), _(L"A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1);
	static HWND aWnd = FindWindowA(_("Valve001"), nullptr);
	//pOldWindowProc = reinterpret_cast<WNDPROC>((SetWindowLongPtr(aWnd, (-4), reinterpret_cast<LONG_PTR>(WndProcHook::Hook))));
	g_Hooks.Init();
	g_ConVars.Init();
	//EFonts
	g_Draw.InitFonts
	({
		//FONT_ESP
		{ 0x0, _("Small Fonts"), 8, 0, FONTFLAG_OUTLINE },
		//FONT_ESP_OUTLINED
		{ 0x0, _("Small Fonts"), 8, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_NAME
		{ 0x0, _("Verdana"), 12, 0, FONTFLAG_DROPSHADOW },
		//FONT_ESP_NAME_OUTLINED
		{ 0x0, _("Verdana"), 12, 600, FONTFLAG_DROPSHADOW },

		//FONT_ESP_COND
		{ 0x0, _("runescape uf"), 12, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },
		//FONT_ESP_COND_OUTLINED
		{ 0x0, _("Small Fonts"), 8, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_PICKUPS
		{ 0x0, _("Small Fonts"), 8, 0, FONTFLAG_OUTLINE },
		//FONT_ESP_PICKUPS_OUTLINED
		{ 0x0, _("Small Fonts"), 8, 0, FONTFLAG_OUTLINE },

		//FONT_MENU
		{ 0x0, _("runescape uf"), 12, 0, FONTFLAG_NONE | FONTFLAG_DROPSHADOW },
		//FONT_MENU_OUTLINED
		{ 0x0, _("runescape uf"), 12, 0, FONTFLAG_OUTLINE },

		/*FONT_ICONS*/
		{ 0x0, _("Tf2weaponicons Regular"), 20, 0, FONTFLAG_NONE},
	});

	//Initialize ignored set
	g_Playerlist.GetIgnoredPlayers();
	g_Events.Setup({ "vote_cast", "player_changeclass", "player_connect", "player_hurt", "localplayer_respawn", "achievement_earned" });

	g_DiscordRPC->init();
	g_DiscordRPC->update();

	g_Visuals.AddToEventLog(_("Cheat injected successfully!"));
	g_Visuals.AddToEventLog(_("Press \"Insert\" to open menu!"));

	g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 0, 255 }, _("[!] CAM Loaded!\n"));
	g_Interfaces.Surface->PlaySound(_("misc//boring_applause_1.wav"));
	
	std::wstring defaultConfig = _(L"Default"); // Thank you Mr.Fedora lmao
	if (!std::filesystem::exists(g_CFG.m_sConfigPath + L"\\" + defaultConfig)) {

		std::wstring s;
		StringToWString(s, _("Default"));
		g_CFG.Load(s.c_str());
		std::string fart = Utils::str_tolower(Vars::Fart::customFont);
		//std::transform(fart.begin(), fart.end(), fart.begin(), std::tolower);
		if (Vars::Fart::customFont.c_str() == _("") || fart == _("default")) {
			g_Draw.ReInitFonts(
				{
					//FONT_ESP
					{ 0x0, Vars::Fart::customFont.c_str(), 12, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },
					//FONT_ESP_OUTLINED
					{ 0x0, Vars::Fart::customFont.c_str(), 12, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },

					//FONT_ESP_NAME
					{ 0x0, Vars::Fart::customFont.c_str(), 12, 0, FONTFLAG_DROPSHADOW },
					//FONT_ESP_NAME_OUTLINED

					{ 0x0, Vars::Fart::customFont.c_str(), 13, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS},

					//FONT_ESP_COND
					{ 0x0, Vars::Fart::customFont.c_str(), 12, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },
					//FONT_ESP_COND_OUTLINED
					{ 0x0, Vars::Fart::customFont.c_str(), 10, 0, FONTFLAG_OUTLINE },

					//FONT_ESP_PICKUPS
					{ 0x0, Vars::Fart::customFont.c_str(), 13, 0, FONTFLAG_NONE },
					//FONT_ESP_PICKUPS_OUTLINED
					{ 0x0, Vars::Fart::customFont.c_str(), 13, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS},

					//FONT_MENU
					{ 0x0, _("Verdana"), 12, 0, FONTFLAG_NONE | FONTFLAG_DROPSHADOW },
					//FONT_MENU_OUTLINED
					{ 0x0, _("Verdana"), 12, 0, FONTFLAG_OUTLINE },
				}
			);
		}
		g_Visuals.AddToEventLog(_("Default config loaded!")); 
	}

	while (!GetAsyncKeyState(VK_F11)) {
		
		std::this_thread::sleep_for(1500ms);
	}

	/*
	fclose(static_cast<FILE*>(stdin));
	fclose(static_cast<FILE*>(stdout));
	FreeConsole();
	*/

	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[-] Unloading CAM...\n"));
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[-] Stopping Discord RPC\n"));
	Discord_Shutdown();

	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 200, 0, 255 }, _("[-] Stopping Steam RPC\n"));
	

	g_Events.Destroy();

	g_Interfaces.Surface->SetCursorAlwaysVisible(false);
	std::this_thread::sleep_for(100ms);
	
	g_Hooks.Release();
	g_Visuals.RestoreWorldModulation(); //needs to do this after hooks are released cuz UpdateWorldMod in FSN will override it
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 0, 255 }, _("[!] CAM Unloaded!\n"));

	WinAPI::FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		Utils::RemovePEH(hinstDLL);

		if (auto hMainThread = WinAPI::CreateThread(0, 0, MainThread, hinstDLL, 0, 0))
			WinAPI::CloseHandle(hMainThread);
	}

	return TRUE;
}
