#pragma once

#include "../SDK/SDK.h"
#include "../Features/Playerlist/Playerlist.h"
template <class T>
class CVar
{
public:
	T m_Var;
	const wchar_t* m_szDisplayName;
};

namespace Vars
{
	namespace Menu
	{
		inline bool Opened			= false;
		inline Rect_t Position		= { 100, 100, 870, 480 };
		inline const auto Title		= XorStr(L"Team Fortress 2");
		inline int TitleBarH		= 16;

		inline int ShadowSize		= 128;
		
		inline int SpacingX = 8;
		inline int SpacingY = 4;
		inline int SpacingText = 4;

		inline int CheckBoxW = 10;
		inline int CheckBoxH = 10;
		inline int CheckBoxFillSize = 4;

		inline int ButtonTabW = 198;
		inline int ButtonTabH = 25;

		inline int ButtonW = 90;
		inline int ButtonH = 20;

		inline int ButtonWSmall = 70;
		inline int ButtonHSmall = 16;

		inline int ComboBoxW = 90;
		inline int ComboBoxH = 14;

		inline int InputBoxW = 90;
		inline int InputBoxH = 14;
		inline int InputColorBoxW = 30;
		

		namespace Colors
		{
			//inline Color_t WindowBackground = { 30, 30, 30, 255 };
			inline Color_t Text = { 255, 255, 255, 190 };
			inline Color_t Widget = { 255, 255, 255, 190 };
			inline Color_t WidgetActive = { 150, 0, 255, 255 };
			inline Color_t ShadowColor = { 150, 0, 255, 255 };
			inline Color_t OutlineMenu = { 50, 50, 50, 255 };
		}
	}

	namespace Aimbot
	{
		namespace CritHack
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<int> CritKey{ VK_LSHIFT, L"Crit Key" };
			inline CVar<bool> SaveBucket{ false, L"Save Bucket" };
			inline CVar<bool> MeleeCrits{ false, L"Meele Crits" };
			inline CVar<bool> CritBar{ false, L"Crit Bar" };
		}

		namespace Global
		{
			inline CVar<bool> Active				{ true, L"Active" };
			inline CVar<int> AimKey					{ VK_LSHIFT, L"Aim Key" };
			inline CVar<bool> AutoShoot				{ true, L"Auto Shoot" };
			inline CVar<bool> AimPlayers			{ true, L"Aim Players" };
			inline CVar<bool> AimBuildings			{ true, L"Aim Buildings" };
			inline CVar<bool> AimStickies			{ true, L"Aim Buildings" };
			inline CVar<bool> IgnoreInvlunerable	{ true, L"Ignore Invulnerable" };
			inline CVar<bool> IgnoreCloaked			{ false, L"Ignore Cloaked" };
			inline CVar<bool> IgnoreFriends			{ true, L"Ignore Friends" };
			inline CVar<bool> IgnoreTaunting		{ true, L"Ignore Taunting" };
			inline CVar<bool> IgnoreVaccinator		{ false, L"Ignore Vaccinator" };
		}

		namespace Hitscan
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<int> SortMethod			{ 0, L"Sort Method" };	//0 - FOV,		1 - Distance
			inline CVar<int> AimMethod			{ 1, L"Aim Method" };	//0 - Normal,	1 - Smooth, 2 - Silent
			inline CVar<int> AimHitbox			{ 2, L"Aim Hitbox" };	//0 - Head,		1 - Body,	2 - Auto
			inline CVar<float> AimFOV			{ 15.0f, L"Aim FOV" };
			inline CVar<float> SmoothingAmount	{ 4.0f, L"Smoothing Amount" };
			inline CVar<int> TapFire			{ 1, L"Tap Fire" };	//0 - Off, 1 - Distance, 2 - Always
			inline CVar<bool> ScanHitboxes		{ true, L"Scan Body" };
			inline CVar<bool> ScanHead			{ true, L"Scan Head" };
			inline CVar<bool> ScanBuildings		{ true, L"Scan Buildings" };
			inline CVar<bool> WaitForHeadshot	{ true, L"Wait For Headshot" };
			inline CVar<bool> WaitForCharge     { true, L"Wait For Charge" };
			inline CVar<bool> SpectatedSmooth	{ false, L"Spectated Smooth" };
			inline CVar<bool> ScopedOnly		{ false, L"Scoped Only" };
			inline CVar<bool> AutoScope			{ false, L"Auto Scope" };
			inline CVar<bool> AutoRev			{ false, L"Auto Rev" };
		}

		namespace Projectile
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<bool> R8Method			{ true, L"sparkly cheat paste go brrr" };
			inline CVar<bool> PerformanceMode	{ true, L"Performance Mode" };
			inline CVar<int> SortMethod			{ 0, L"Sort Method" };	//0 - FOV,		1 - Distance
			inline CVar<int> AimMethod			{ 1, L"Aim Method" };	//0 - Normal,	1 - Silent
			inline CVar<int> AimPosition		{ 2, L"Aim Position" };	//0 - Body,		1 - Feet,	2 - Auto
			inline CVar<float> AimFOV			{ 25.0f, L"Aim FOV" };
			inline CVar<bool> FeetAimIfOnGround{ false, L"Aim at feet if target is on the ground." };
		}

		namespace Melee
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<int> SortMethod			{ 1, L"Sort Method" };	//0 - FOV,		1 - Distance
			inline CVar<int> AimMethod			{ 1, L"Aim Method" };	//0 - Normal,	1 - Smooth, 2 - Silent
			inline CVar<float> AimFOV			{ 45.0f, L"Aim FOV" };
			inline CVar<float> SmoothingAmount	{ 8.0f, L"Smoothing Amount" };
			inline CVar<bool> RangeCheck		{ true, L"Range Check" };
			inline CVar<bool> PredictSwing		{ true, L"Predict Swing" };
			inline CVar<bool> WhipTeam          { true, L"Whip Teammates" };
		}
	}

	namespace Triggerbot
	{
		namespace Global
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<int> TriggerKey{ VK_LSHIFT, L"Trigger Key" };
			inline CVar<bool> IgnoreInvlunerable{ true, L"Ignore Invulnerable" };
			inline CVar<bool> IgnoreCloaked{ false, L"Ignore Cloaked" };
			inline CVar<bool> IgnoreFriends{ false, L"Ignore Friends" };
			inline CVar<bool> IgnoreTaunts{ false, L"Ignore Taunts" };
		}

		namespace Shoot
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<bool> TriggerPlayers{ true, L"Trigger Players" };
			inline CVar<bool> TriggerBuildings{ true, L"Trigger Buildings" };
			inline CVar<bool> HeadOnly{ false, L"Head Only" };
			inline CVar<bool> WaitForHeadshot{ true, L"Wait For Charge" };
			inline CVar<float> HeadScale{ 0.7f, L"Head Scale" };
			inline CVar<bool> ScopedOnly{ false, L"Scoped Only" };
		}

		namespace Stab
		{
			inline CVar<bool> Active	{ true, L"Active" };
			inline CVar<bool> RageMode	{ false, L"Rage Mode" };
			inline CVar<bool> Silent	{ false, L"Silent" };
			inline CVar<bool> Disguise	{ false, L"Disguise" };
			inline CVar<bool> IgnRazor	{ false, L"Ignore Razorback" };
			inline CVar<float> Range	{ 0.9f, L"Range" };
		}

		namespace Detonate
		{
			inline CVar<bool> Active	    { true, L"Active" };
			inline CVar<bool> Stickies      { true, L"Detonate Stickies" };
			inline CVar<bool> Flares        { true, L"Detonate Flares" };
			inline CVar<float> RadiusScale	{ 1.0f, L"Radius Scale" };
		}

		namespace Blast
		{
			inline CVar<bool> Active    { false, L"Active" };
			inline CVar<bool> Rage      { false, L"Rage" };
			inline CVar<bool> Silent	{ false, L"Silent" };
			inline CVar<int> Fov	    { 55,    L"FOV" };
		}

		namespace Uber
		{
			inline CVar<bool> Active		{ false, L"Active" };
			inline CVar<bool> OnlyFriends	{ false, L"Only Friends" };
			inline CVar<bool> PopLocal      { false, L"Pop on local" };
			inline CVar<float> HealthLeft   { 35.0f, L"Health left" };
		}

		namespace AutoVaccinator
		{
			inline CVar<bool> Active{ false, L"nigger" };
			inline CVar<bool> OnlyFriends{ false, L"nigger" };
			inline CVar<float> UberMaxPercentage { 70.f, L"nigger" };
			
		}
	}

	namespace ESP
	{

		namespace Main
		{
			//inline CVar<bool> Active	{ true, L"Active" };
			inline CVar<int> Outline	{ 2, L"Outline" }; //0 - OFF, 1 - Text Only, 2 - All
		}

		namespace Players
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<bool> ShowLocal			{ true, L"Show Local" };
			inline CVar<int> IgnoreTeammates	{ 2, L"Ignore Teammates" };	//0 - OFF, 1 - All, 2 - Keep Friends
			inline CVar<int> IgnoreCloaked		{ 0, L"Ignore Cloaked" };	//0 - OFF, 1 - All, 2 - Enemies Only
			inline CVar<bool> Name				{ true, L"Name" };
			inline CVar<bool> NameCustom		{ false, L"Custom Color"};
			inline CVar<int> Uber				{ 2, L"Uber" }; //0 - Off, 1 - Text, 2 - Bar
			inline CVar<int> Class				{ 1, L"Class" }; // 0 - Off, 1 - Icon, 2 - Text, 3 - Both
			inline CVar<bool> Health			{ false, L"Health" };
			inline CVar<bool> Cond				{ true, L"Cond" };

			namespace Healthbar {
				inline CVar<bool> Enabled{ true, L"Health Bar" };
				inline CVar<bool> HealthBasedColor{ true, L"Health Based Color" };
				inline CVar<int> Style{ 0, L"Style" };	//0 - Solid, 1 - Gradient
			}

			inline CVar<int> Box				{ 0, L"Box" };	//0 - OFF, 1 - Simple, 2 - Corners
			inline CVar<int> Bones				{ 0, L"Bones" };
			inline CVar<bool> GUID              { false, L"GUID" };
			inline CVar<bool> Lines             { false, L"Lines" };
			inline CVar<bool> Dlights			{ false, L"Dlights" };
			inline CVar<float> DlightRadius		{ 200.0f, L"DLight Radius" };
			inline CVar<float> Alpha			{ 1.0f, L"Alpha" };

			namespace Arrows {
				inline CVar<bool> Active{ true, L"arrow" };
				inline CVar<float> DistFromCenter { 150.f , L"arrow" };
				inline CVar<float> ArrowSize { 20.f, L"arrow" };
			}
		}

		namespace Buildings
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<bool> IgnoreTeammates	{ true, L"Ignore Teammates" };
			inline CVar<bool> Name				{ true, L"Name" };
			inline CVar<bool> NameCustom		{ false, L"Custom Color"};
			inline CVar<bool> Health			{ false, L"Health" };
			inline CVar<bool> Owner             { false, L"Owner" };
			inline CVar<bool> Level				{ true, L"Level" };
			inline CVar<bool> Cond				{ true, L"Cond" };
			inline CVar<bool> HealthBar			{ true, L"Health Bar" };
			inline CVar<bool> Lines				{ false, L"Lines" };
			inline CVar<int> Box				{ 0, L"Box" };	//0 - OFF, 1 - Simple, 2 - Corners
			inline CVar<bool> Dlights			{ false, L"Dlights" };
			inline CVar<float> DlightRadius		{ 200.0f, L"DLight Radius" };
			inline CVar<float> Alpha			{ 1.0f, L"Alpha" };
		}

		namespace World
		{
			inline CVar<bool> Active		{ true, L"Active" };
			inline CVar<bool> HealthText	{ true, L"Health Text" };
			inline CVar<bool> AmmoText		{ true, L"Ammo Text" };
			inline CVar<int> Projectiles	{ 0, L"Projectiles" }; //0 - Off, 1 - All, 2 - Enemy Only
			inline CVar<float> Alpha		{ 1.0f, L"Alpha" };
		}
	}

	namespace Chams
	{
		/*
		namespace Main
		{
			inline CVar<bool> Active{ false , L"Active" };
		}
		*/

		namespace Players
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> ShowLocal{ true, L"Show Local" };
			inline CVar<int> IgnoreTeammates{ 2, L"Ignore Teammates" };	//0 - OFF, 1 - All, 2 - Keep Friends
			inline CVar<bool> Wearables{ true, L"Render Wearables" };
			inline CVar<bool> Weapons{ true, L"Render Weapons" };
			inline CVar<int> Material{ 1, L"Material" }; //0 - None, 1 - Shaded, 2 - Shiny, 3 - Flat
			inline CVar<bool> IgnoreZ{ false , L"IgnoreZ" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
			inline CVar<bool> HitboxThing{ false, L"xd" };
			inline CVar<float> HitboxTimeThing{ 5.0f, L"balls" };
		}

		namespace Buildings
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> IgnoreTeammates{ true, L"Ignore Teammates" };
			inline CVar<int> Material{ 1, L"Material" }; //0 - None, 1 - Shaded, 2 - Shiny, 3 - Flat
			inline CVar<bool> IgnoreZ{ false , L"IgnoreZ" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
		}

		namespace World
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<bool> Health{ true, L"Health" };
			inline CVar<bool> Ammo{ true, L"Ammo" };
			inline CVar<int> Projectiles{ 1, L"Projectiles" }; //0 - Off, 1 - All, 2 - Enemy Only
			inline CVar<int> Material{ 1, L"Material" }; //0 - None, 1 - Shaded, 2 - Shiny, 3 - Flat
			inline CVar<bool> IgnoreZ{ false , L"IgnoreZ" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
		}

		namespace DME
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<bool> SeeThru{ false, L"See Through" };

			inline CVar<int> Hands{ 0, L"Hands" }; //0 - Original, 1 - Shaded, 2 - Shiny, 3 - Flat, 4 - WF-Shaded, 5 - WF-Shiny, 6 - WF-Flat
			inline CVar<float> HandsAlpha{ 1.0f, L"Hands Alpha" };

			inline CVar<int> Weapon{ 0, L"Weapon" }; //0 - Original, 1 - Shaded, 2 - Shiny, 3 - Flat, 4 - WF-Shaded, 5 - WF-Shiny, 6 - WF-Flat
			inline CVar<float> WeaponAlpha{ 1.0f, L"Weapon Alpha" };
		}
	}

	namespace Glow
	{
		namespace Main
		{
			//inline CVar<bool> Active{ false, L"Active" };
			inline CVar<bool> Wireframe{ false, L"Wireframe" };
			inline CVar<int> Scale{ 5, L"Scale" };
		}

		namespace Players
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> ShowLocal{ true, L"Show Local" };
			inline CVar<int> IgnoreTeammates{ 2, L"Ignore Teammates" };	//0 - OFF, 1 - All, 2 - Keep Friends
			inline CVar<bool> Wearables{ true, L"Render Wearables" };
			inline CVar<bool> Weapons{ true, L"Render Weapons" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
			inline CVar<int> Color{ 0, L"Color" }; //0 - Team, 1 - Health
		}

		namespace Buildings
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> IgnoreTeammates{ true, L"Ignore Teammates" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
			inline CVar<int> Color{ 0, L"Color" }; //0 - Team, 1 - Health
		}

		namespace World
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<bool> Health{ true, L"Health" };
			inline CVar<bool> Ammo{ true, L"Ammo" };
			inline CVar<int> Projectiles{ 2, L"Projectiles" }; //0 - Off, 1 - All, 2 - Enemy Only
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
		}
	}

	namespace Radar
	{
		namespace Main
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<int>  BackAlpha{ 200, L"Back Alpha" };
			inline CVar<int>  Size{ 100, L"Size" };
			inline CVar<int>  Range{ 1500, L"Range" };
		}

		namespace Players
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<int>  IconType			{ 1, L"Icon" }; //0 - Scoreboard, 1 - Portraits, 2 - Avatars
			//inline CVar<int>  BackGroundType	{ 2, L"Background" }; //0 - Off, 1 - Rect, 2 - Texture
			//inline CVar<bool> Outline			{ true, L"Outline" };
			inline CVar<int>  IgnoreTeam		{ 2, L"Ignore Teammates" };  //0 - Off, 1 - All, 2 - Keep Friends
			//inline CVar<int>  IgnoreCloaked		{ 0, L"Ignore Cloaked" };	//0 - Off, 1 - All, 2 - Enemies Only
			inline CVar<bool> Health			{ false, L"Health Bar" };
			inline CVar<int>  IconSize			{ 24, L"Icon Size" };
		}

		namespace Buildings
		{
			inline CVar<bool> Active		{ true, L"Active" };
			//inline CVar<bool> Outline		{ false, L"Outline" };
			inline CVar<bool> IgnoreTeam	{ true, L"Ignore Team" };
			inline CVar<bool> Health		{ false, L"Health Bar" };
			inline CVar<int>  IconSize		{ 18, L"Icon Size" };
		}

		namespace World
		{
			//inline CVar<bool> Active     { false, L"Active" };
			inline CVar<bool> Health     { true, L"Health" };
			inline CVar<bool> Ammo       { true, L"Ammo" };
			inline CVar<int>  IconSize	 { 14, L"Icon Size" };
		}
	}

	namespace Visuals
	{
		inline CVar<bool> RemoveDisguises   { false, L"Remove Disguises" };
		inline CVar<bool> RemoveTaunts		{ false, L"Remove Taunts" };
		inline CVar<int> Fullbright			{ 0, L"Fullbright" };
		inline CVar<int> FieldOfView		{ 110, L"Field of View" };
		inline CVar<float> AspectRatioValue	{ 0, L"Aspect ratio" };
		inline CVar<int> AimFOVAlpha		{ 10, L"Aim FOV Alpha" };

		inline CVar<int> ViewModelX		{ 0, L"Viewmodel X" };
		inline CVar<int> ViewModelY		{ 0, L"Viewmodel Y" };
		inline CVar<int> ViewModelZ		{ 0, L"Viewmodel Z" };

		inline CVar<bool> RemoveScope		{ true, L"Remove Scope" };
		inline CVar<bool> RemoveZoom		{ true, L"Remove Zoom" };
		inline CVar<bool> RemovePunch		{ false, L"Remove Recoil" };
		inline CVar<bool> CrosshairAimPos	{ true, L"Crosshair At Aim Pos" };
		inline CVar<bool> ChatInfo			{ true, L"Show Class Changes" };
		inline CVar<bool> RemoveHats		{ false, L"Remove Hats" };

		inline CVar<bool> SpectatorList{ true, L"Spectator List" };
		//inline CVar<int> SpectatorList { 3, L"Spectator List" }; //0 - Off, 1 - Default, 2 - Classic, 3 - Classic Avatars

		inline CVar<bool> SpyWarning				{ true, L"Active" };
		inline CVar<bool> SpyWarningAnnounce		{ true, L"Announce" };
		inline CVar<int> SpyWarningStyle			{ 0, L"Style" }; //0 - Indicator, 1 - Flash
		inline CVar<bool> SpyWarningVisibleOnly		{ true, L"Visible Only" };
		inline CVar<bool> SpyWarningIgnoreFriends	{ true, L"Ignore Friends" };

		inline CVar<bool> Snow{ false, L"Menu Snow" };
		inline CVar<bool> ToolTips{ false, L"Menu tooltips" };

		inline CVar<bool> ThirdPerson				{ false, L"Active" };
		inline CVar<int> ThirdPersonKey				{ 0x0, L"Toggle Key"};
		inline CVar<bool> ThirdPersonSilentAngles	{ true, L"Silent Angles" };
		inline CVar<bool> ThirdPersonInstantYaw		{ true, L"Instant Yaw" };
		inline CVar<bool> ThirdpersonCrosshair{ 0.f, L"cum" };
		inline CVar<float> ThirdpersonOffsetX{ 0.f, L"cum" };
		inline CVar<float> ThirdpersonOffsetY{ 0.f, L"cum" };
		inline CVar<float> ThirdpersonOffsetZ{ 0.f, L"cum" };

		inline CVar<bool> WorldModulation			{ false, L"World Modulation" };
		inline CVar<bool> SkyModulation			{ false, L"Sky Modulation" };
		inline CVar<bool> SkyboxChanger				{ true, L"Skybox changer" };

		inline CVar<bool> DevTextures				{ false, L"Dev Textures" };

		inline CVar<bool> PlayerList				{ false, L"Playerlist" };
		inline CVar<bool> AimbotViewmodel				{ true, L"aimbot viewmodel lodaslkpadfsopksdfaopfadsjpkasfdofsapofjop" };

inline CVar<int> TracerEffect{ 0, L"Tracer style" };
   inline CVar<int> RagdollEffect{ 0, L"Ragdoll effect" };

		namespace Skins
		{
			inline CVar<bool> Enabled				{ false, L"Active" };
			inline CVar<int> Sheen					{ 0, L"Sheen" };
			inline CVar<int> Effect					{ 0, L"Effect" };
			inline CVar<int> Particle				{ 0, L"Particle" };
			inline CVar<bool> Acient				{ false, L"Ancient" };
			inline CVar<bool> Override				{ false, L"Style Override" };
		}
	}

	namespace Skybox
	{
		inline int skyboxnum = 0;
		inline std::string SkyboxName = "";
	}

	namespace Fart {
		inline std::string customFont = "";
	}

	namespace Misc
	{
		inline CVar<bool> AutoJump				{ true, L"Auto Jump" };
		inline CVar<bool> AutoStrafe			{ true, L"Auto Strafe" };
		inline CVar<bool> TauntSlide			{ false, L"Taunt Slide" };
		inline CVar<bool> TauntControl			{ false, L"Taunt Control" };
		inline CVar<bool> TauntControlMouse		{ false, L"Taunt control mouse"};
		inline CVar<bool> Freecam				{ false, L"freecam" };
		inline CVar<int>  FreecamKey			{ 0x48, L"freecam key" };
		inline CVar<float> FreecamSpeed			{ 3.f, L"freecam speed" };
		inline CVar<bool> BypassPure            { false, L"Bypass Pure" };
		inline CVar<bool> NoisemakerSpam        { false, L"Noisemaker Spam" };
		inline CVar<bool> DisableInterpolation	{ true, L"Disable Interpolation" };
		inline CVar<bool> MedalFlip				{ true, L"Medal Flip" };
		inline CVar<bool> AutoRocketJump		{ false, L"Auto RocketJump" };
		inline CVar<bool> ChatSpam              { false, L"Chat Spam" };
		inline CVar<bool> NoPush				{ false, L"No Push" };
		inline CVar<bool> InstantRespawn		{ false, L"MvM Instant Respawn" };
		inline CVar<bool> EdgeJump				{ false, L"Edge Jump" };
		inline CVar<bool> AntiAFK				{ false, L"Anti AFK" };
		inline CVar<bool> VoteRevealer			{ false, L"Reveal votes" };
		inline CVar<bool> VotesInChat			{ false, L"Reveal votes to party" };
		inline CVar<bool> CleanScreenshot		{ true, L"l0l" };
		inline CVar<bool> SteamRPC				{ true, L"Self explanatory" };
		inline CVar<bool> CheatsBypass			{ false, L"cum" };
		inline std::string SteamRPCText	= "ChadAlphaMales.club";
		namespace CL_Move
		{
			inline CVar<int> DTTicks{ 20, L"DT Ticks"};
			inline CVar<bool> Doubletap{ true, L"Doubletap" };
			inline CVar<bool> NotInAir{ true, L"Dont DT in air" };
			//inline CVar<int> TeleportKey{ 0x46, L"Teleport Key" }; //F
			inline CVar<int> RechargeKey{ 0x52, L"Recharge Key" }; //R
			inline CVar<int> DoubletapKey{ 0x46, L"Doubletap Key" }; //V

			inline CVar<bool> WaitForDT{ true, L"Wait for DT" };

			inline CVar<bool> Fakelag{ true, L"Fakelag" };
			inline CVar<bool> FakelagOnKey{ true, L"Fakelag On Key" };
			inline CVar<int> FakelagKey{ 0x54, L"Fakelag Key" }; //T
			inline CVar<int> FakelagValue{ 1, L"Fakelag value" };
		}
	}

	namespace AntiHack
	{
		namespace AntiAim
		{
			inline CVar<bool> Active	{ false, L"Active" };
			inline CVar<int> Pitch		{ 0, L"Pitch" };		//0 - None, 1 - Up, 2 - Down, 3 - Fake Up, 4 - Fake Down
			inline CVar<int> SpinSpeed	{ 0, L"Spin Speed" };
			inline CVar<int> YawReal	{ 0, L"Yaw Real" };	//0 - None, 1 - Left, 2 - Right, 3 - Backwards
			inline CVar<int> YawFake	{ 0, L"Yaw Fake" };	//0 - None, 1 - Left, 2 - Right, 3 - Backwards
		}
	}
}