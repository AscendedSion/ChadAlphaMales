#include "ConfigManager.h"

#include <string>

#include "../Vars.h"
#include "../../SDK/SDK.h"
#include "../Radar/Radar.h"
#include "../SpectatorList/SpectatorList.h"

#define SAVE_VAR(x) Save(_(L#x), x.m_Var)
#define LOAD_VAR(x) Load(_(L#x), x.m_Var)

#define SAVE_STRING(x) Save(_(L#x), x)
#define LOAD_STRING(x) Load(_(L#x), x)

#define SAVE_OTHER(x) Save(_(L#x), x)
#define LOAD_OTHER(x) Load(_(L#x), x)

//stfu
#pragma warning (disable : 6328)
#pragma warning (disable : 6031)
#pragma warning (disable : 4477)

bool CConfigManager::Find(const wchar_t* name, std::wstring& output)
{
	m_Read.clear();
	m_Read.seekg(0, std::ios::beg);

	while (std::getline(m_Read, output))
	{
		if (output.find(name) != std::wstring::npos)
			return true;
	}

	return false;
}

void CConfigManager::Save(const wchar_t* name, bool val)
{
	char buffer[64];
	sprintf_s(buffer, _("%ls: %d"), name, val);
	m_Write << buffer << "\n";
}

void CConfigManager::Save(const wchar_t* name, int val)
{
	char buffer[64];
	sprintf_s(buffer, _("%ls: %d"), name, val);
	m_Write << buffer << "\n";
}

void CConfigManager::Save(const wchar_t* name, float val)
{
	char buffer[64];
	sprintf_s(buffer, _("%ls: %f"), name, val);
	m_Write << buffer << "\n";
}

void CConfigManager::Save(const wchar_t* name, Color_t val)
{
	char buffer[64];
	sprintf_s(buffer, _("%ls: %d %d %d %d"), name, val.r, val.g, val.b, val.a);
	m_Write << buffer << "\n";
}

void CConfigManager::Load(const wchar_t* name, bool& val)
{
	std::wstring line = {};

	if (Find(name, line))
		swscanf_s(line.c_str(), L"%*ls %d", &val);
}

void CConfigManager::Load(const wchar_t* name, int& val)
{
	std::wstring line = {};

	if (Find(name, line))
		swscanf_s(line.c_str(), L"%*ls %d", &val);
}

void CConfigManager::Load(const wchar_t* name, float& val)
{
	std::wstring line = {};

	if (Find(name, line))
		swscanf_s(line.c_str(), L"%*ls %f", &val);
}

void CConfigManager::Load(const wchar_t* name, Color_t& val)
{
	std::wstring line = {};

	if (Find(name, line)) {
		int r = 0, g = 0, b = 0, a = 0;
		swscanf_s(line.c_str(), L"%*ls %d %d %d %d", &r, &g, &b, &a);
		val = { static_cast<byte>(r), static_cast<byte>(g), static_cast<byte>(b), static_cast<byte>(a) };
	}
}

void CConfigManager::Save(const wchar_t* name, std::string val)
{
	char buffer[128];
	sprintf_s(buffer, "%ls: %s", name, val.c_str());
	m_Write << buffer << "\n";
}

void CConfigManager::Load(const wchar_t* name, std::string& val)
{
	std::wstring line = {};

	if (Find(name, line)) {
		//swscanf_s(line.c_str(), L"%*ls:");
		std::wstring delimiter = L": ";
		std::wstring svalue = line.substr(line.find(L": ") + 2, sizeof(line));
		std::string stringvalue(svalue.begin(), svalue.end());
		val = stringvalue;
	}
}

CConfigManager::CConfigManager()
{
	m_sConfigPath = std::filesystem::current_path().wstring() + _(L"\\CAMFigs");

	if (!std::filesystem::exists(m_sConfigPath))
		std::filesystem::create_directory(m_sConfigPath);

	//if (!std::filesystem::exists(m_sConfigPath + _(L"\\CAMCore")))
		//std::filesystem::create_directory(m_sConfigPath + _(L"\\CAMCore"));
}

void CConfigManager::Save(const wchar_t* name)
{
	m_Write = std::wofstream(m_sConfigPath + L"\\" + name + _(L".poop"));

	if (m_Write.is_open())
	{
		//Aimbot
		{
			//Global
			{
				SAVE_VAR(Vars::Aimbot::Global::Active);
				SAVE_VAR(Vars::Aimbot::Global::AimKey);
				SAVE_VAR(Vars::Aimbot::Global::AutoShoot);
				SAVE_VAR(Vars::Aimbot::Global::AimPlayers);
				SAVE_VAR(Vars::Aimbot::Global::AimBuildings);
				SAVE_VAR(Vars::Aimbot::Global::IgnoreInvlunerable);
				SAVE_VAR(Vars::Aimbot::Global::IgnoreCloaked);
				SAVE_VAR(Vars::Aimbot::Global::IgnoreFriends);
				SAVE_VAR(Vars::Aimbot::Global::IgnoreTaunting);
				SAVE_VAR(Vars::Aimbot::Global::IgnoreVaccinator);
			}

			//Hitscan
			{
				SAVE_VAR(Vars::Aimbot::Hitscan::Active);
				SAVE_VAR(Vars::Aimbot::Hitscan::SortMethod);
				SAVE_VAR(Vars::Aimbot::Hitscan::AimMethod);
				SAVE_VAR(Vars::Aimbot::Hitscan::AimHitbox);
				SAVE_VAR(Vars::Aimbot::Hitscan::AimFOV);
				SAVE_VAR(Vars::Aimbot::Hitscan::SmoothingAmount);
				SAVE_VAR(Vars::Aimbot::Hitscan::TapFire);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScanHitboxes);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScanHead);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScanBuildings);
				SAVE_VAR(Vars::Aimbot::Hitscan::WaitForHeadshot);
				SAVE_VAR(Vars::Aimbot::Hitscan::WaitForCharge);
				SAVE_VAR(Vars::Aimbot::Hitscan::SpectatedSmooth);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScopedOnly);
				SAVE_VAR(Vars::Aimbot::Hitscan::AutoScope);
				SAVE_VAR(Vars::Aimbot::Hitscan::AutoRev);
			}

			//Projectile
			{
				SAVE_VAR(Vars::Aimbot::Projectile::Active);
				SAVE_VAR(Vars::Aimbot::Projectile::PerformanceMode);
				//SAVE_VAR(Vars::Aimbot::Projectile::SortMethod);
				SAVE_VAR(Vars::Aimbot::Projectile::AimMethod);
				SAVE_VAR(Vars::Aimbot::Projectile::AimPosition);
				SAVE_VAR(Vars::Aimbot::Projectile::AimFOV);
				SAVE_VAR(Vars::Aimbot::Projectile::R8Method);
			}

			//Melee
			{
				SAVE_VAR(Vars::Aimbot::Melee::Active);
				//SAVE_VAR(Vars::Aimbot::Melee::SortMethod);
				SAVE_VAR(Vars::Aimbot::Melee::AimMethod);
				SAVE_VAR(Vars::Aimbot::Melee::AimFOV);
				SAVE_VAR(Vars::Aimbot::Melee::SmoothingAmount);
				SAVE_VAR(Vars::Aimbot::Melee::RangeCheck);
				SAVE_VAR(Vars::Aimbot::Melee::PredictSwing);
			}
		}

		//Triggerbot
		{
			//Global
			{
				SAVE_VAR(Vars::Triggerbot::Global::Active);
				SAVE_VAR(Vars::Triggerbot::Global::TriggerKey);
				SAVE_VAR(Vars::Triggerbot::Global::IgnoreInvlunerable);
				SAVE_VAR(Vars::Triggerbot::Global::IgnoreCloaked);
				SAVE_VAR(Vars::Triggerbot::Global::IgnoreFriends);
				SAVE_VAR(Vars::Triggerbot::Global::IgnoreTaunts);
			}

			//Shoot
			{
				SAVE_VAR(Vars::Triggerbot::Shoot::Active);
				SAVE_VAR(Vars::Triggerbot::Shoot::TriggerPlayers);
				SAVE_VAR(Vars::Triggerbot::Shoot::TriggerBuildings);
				SAVE_VAR(Vars::Triggerbot::Shoot::HeadOnly);
				SAVE_VAR(Vars::Triggerbot::Shoot::WaitForHeadshot);
				SAVE_VAR(Vars::Triggerbot::Shoot::ScopedOnly);
				SAVE_VAR(Vars::Triggerbot::Shoot::HeadScale);
			}

			//Stab
			{
				SAVE_VAR(Vars::Triggerbot::Stab::Active);
				SAVE_VAR(Vars::Triggerbot::Stab::RageMode);
				SAVE_VAR(Vars::Triggerbot::Stab::Silent);
				SAVE_VAR(Vars::Triggerbot::Stab::Disguise);
				SAVE_VAR(Vars::Triggerbot::Stab::IgnRazor);
				SAVE_VAR(Vars::Triggerbot::Stab::Range);
			}

			//Detonate
			{
				SAVE_VAR(Vars::Triggerbot::Detonate::Active);
				SAVE_VAR(Vars::Triggerbot::Detonate::Stickies);
				SAVE_VAR(Vars::Triggerbot::Detonate::Flares);
				SAVE_VAR(Vars::Triggerbot::Detonate::RadiusScale);
			}

			//Blast
			{
				SAVE_VAR(Vars::Triggerbot::Blast::Active);
				SAVE_VAR(Vars::Triggerbot::Blast::Rage);
				SAVE_VAR(Vars::Triggerbot::Blast::Silent);
				SAVE_VAR(Vars::Triggerbot::Blast::Fov);
			}

			//Uber
			{
				SAVE_VAR(Vars::Triggerbot::Uber::Active);
				SAVE_VAR(Vars::Triggerbot::Uber::OnlyFriends);
				SAVE_VAR(Vars::Triggerbot::Uber::PopLocal);
				SAVE_VAR(Vars::Triggerbot::Uber::HealthLeft);
			}

			//Vacc
			{
				SAVE_VAR(Vars::Triggerbot::AutoVaccinator::Active);
				SAVE_VAR(Vars::Triggerbot::AutoVaccinator::OnlyFriends);
				SAVE_VAR(Vars::Triggerbot::AutoVaccinator::UberMaxPercentage);
			}
		}

		//ESP
		{
			//Main
			{
				//SAVE_VAR(Vars::ESP::Main::Active);
				//SAVE_VAR(Vars::ESP::Main::Outline);
			}

			//Players
			{
				SAVE_VAR(Vars::ESP::Players::Active);
				SAVE_VAR(Vars::ESP::Players::ShowLocal);
				SAVE_VAR(Vars::ESP::Players::IgnoreTeammates);
				SAVE_VAR(Vars::ESP::Players::IgnoreCloaked);
				SAVE_VAR(Vars::ESP::Players::Name);
				SAVE_VAR(Vars::ESP::Players::NameCustom);
				SAVE_VAR(Vars::ESP::Players::Uber);
				SAVE_VAR(Vars::ESP::Players::Class);
				//SAVE_VAR(Vars::ESP::Players::Health);
				SAVE_VAR(Vars::ESP::Players::Cond);
				SAVE_VAR(Vars::ESP::Players::Healthbar::Enabled);
				SAVE_VAR(Vars::ESP::Players::Box);
				SAVE_VAR(Vars::ESP::Players::GUID);
				SAVE_VAR(Vars::ESP::Players::Alpha);
				SAVE_VAR(Vars::ESP::Players::Lines);
				SAVE_VAR(Vars::ESP::Players::Bones);
				SAVE_VAR(Vars::ESP::Players::Dlights);
				SAVE_VAR(Vars::ESP::Players::DlightRadius);
				//Arrows
				{
					SAVE_VAR(Vars::ESP::Players::Arrows::Active);
					SAVE_VAR(Vars::ESP::Players::Arrows::DistFromCenter);
					SAVE_VAR(Vars::ESP::Players::Arrows::ArrowSize);
				}
			}

			//Buildings
			{
				SAVE_VAR(Vars::ESP::Buildings::Active);
				SAVE_VAR(Vars::ESP::Buildings::IgnoreTeammates);
				SAVE_VAR(Vars::ESP::Buildings::Name);
				SAVE_VAR(Vars::ESP::Buildings::NameCustom);
				SAVE_VAR(Vars::ESP::Buildings::Health);
				SAVE_VAR(Vars::ESP::Buildings::Owner);
				SAVE_VAR(Vars::ESP::Buildings::Level);
				SAVE_VAR(Vars::ESP::Buildings::Cond);
				SAVE_VAR(Vars::ESP::Buildings::HealthBar);
				SAVE_VAR(Vars::ESP::Buildings::Box);
				SAVE_VAR(Vars::ESP::Buildings::Alpha);
				SAVE_VAR(Vars::ESP::Buildings::Dlights);
				SAVE_VAR(Vars::ESP::Buildings::DlightRadius);
			}

			//World
			{
				SAVE_VAR(Vars::ESP::World::Active);
				SAVE_VAR(Vars::ESP::World::HealthText);
				SAVE_VAR(Vars::ESP::World::AmmoText);
				SAVE_VAR(Vars::ESP::World::Projectiles);
				SAVE_VAR(Vars::ESP::World::Alpha);
			}
		}

		//Chams
		{
			//Main
			/*
			{
				SAVE_VAR(Vars::Chams::Main::Active);
			}
			*/
			//Players
			{
				SAVE_VAR(Vars::Chams::Players::Active);
				SAVE_VAR(Vars::Chams::Players::ShowLocal);
				SAVE_VAR(Vars::Chams::Players::IgnoreTeammates);
				SAVE_VAR(Vars::Chams::Players::Wearables);
				SAVE_VAR(Vars::Chams::Players::Weapons);
				SAVE_VAR(Vars::Chams::Players::Material);
				SAVE_VAR(Vars::Chams::Players::IgnoreZ);
				SAVE_VAR(Vars::Chams::Players::Alpha);
				SAVE_VAR(Vars::Chams::Players::HitboxThing);
				SAVE_VAR(Vars::Chams::Players::HitboxTimeThing);
			}

			//Buildings
			{
				SAVE_VAR(Vars::Chams::Buildings::Active);
				SAVE_VAR(Vars::Chams::Buildings::IgnoreTeammates);
				SAVE_VAR(Vars::Chams::Buildings::Material);
				SAVE_VAR(Vars::Chams::Buildings::IgnoreZ);
				SAVE_VAR(Vars::Chams::Buildings::Alpha);
			}

			//World
			{
				SAVE_VAR(Vars::Chams::World::Active);
				SAVE_VAR(Vars::Chams::World::Health);
				SAVE_VAR(Vars::Chams::World::Ammo);
				SAVE_VAR(Vars::Chams::World::Projectiles);
				SAVE_VAR(Vars::Chams::World::Material);
				SAVE_VAR(Vars::Chams::World::IgnoreZ);
				SAVE_VAR(Vars::Chams::World::Alpha);
			}

			//DME
			{
				SAVE_VAR(Vars::Chams::DME::Active);
				SAVE_VAR(Vars::Chams::DME::Hands);
				SAVE_VAR(Vars::Chams::DME::HandsAlpha);
				SAVE_VAR(Vars::Chams::DME::Weapon);
				SAVE_VAR(Vars::Chams::DME::WeaponAlpha);
			}
		}

		//Glow
		{
			//Main
			{
				//SAVE_VAR(Vars::Glow::Main::Active);
				SAVE_VAR(Vars::Glow::Main::Wireframe);
				SAVE_VAR(Vars::Glow::Main::Scale);
			}

			//Players
			{
				SAVE_VAR(Vars::Glow::Players::Active);
				SAVE_VAR(Vars::Glow::Players::ShowLocal);
				SAVE_VAR(Vars::Glow::Players::IgnoreTeammates);
				SAVE_VAR(Vars::Glow::Players::Wearables);
				SAVE_VAR(Vars::Glow::Players::Weapons);
				SAVE_VAR(Vars::Glow::Players::Alpha);
				SAVE_VAR(Vars::Glow::Players::Color);
			}

			//Buildings
			{
				SAVE_VAR(Vars::Glow::Buildings::Active);
				SAVE_VAR(Vars::Glow::Buildings::IgnoreTeammates);
				SAVE_VAR(Vars::Glow::Buildings::Alpha);
				SAVE_VAR(Vars::Glow::Buildings::Color);
			}

			//World
			{
				SAVE_VAR(Vars::Glow::World::Active);
				SAVE_VAR(Vars::Glow::World::Health);
				SAVE_VAR(Vars::Glow::World::Ammo);
				SAVE_VAR(Vars::Glow::World::Projectiles);
				SAVE_VAR(Vars::Glow::World::Alpha);
			}
		}

		//Radar
		{
			//Main
			{
				SAVE_VAR(Vars::Radar::Main::Active);
				SAVE_VAR(Vars::Radar::Main::BackAlpha);
				SAVE_VAR(Vars::Radar::Main::Size);
				SAVE_VAR(Vars::Radar::Main::Range);
			}

			//Players
			{
				SAVE_VAR(Vars::Radar::Players::Active);
				SAVE_VAR(Vars::Radar::Players::IconType);
				//SAVE_VAR(Vars::Radar::Players::BackGroundType);
				//SAVE_VAR(Vars::Radar::Players::Outline);
				SAVE_VAR(Vars::Radar::Players::IgnoreTeam);
				//SAVE_VAR(Vars::Radar::Players::IgnoreCloaked);
				SAVE_VAR(Vars::Radar::Players::Health);
			}

			//Buildings
			{
				SAVE_VAR(Vars::Radar::Buildings::Active);
				//SAVE_VAR(Vars::Radar::Buildings::Outline);
				SAVE_VAR(Vars::Radar::Buildings::IgnoreTeam);
				SAVE_VAR(Vars::Radar::Buildings::Health);
			}

			//World
			{
				//SAVE_VAR(Vars::Radar::World::Active);
				//SAVE_VAR(Vars::Radar::World::Health);
				//SAVE_VAR(Vars::Radar::World::Ammo);
			}
		}

		//Visuals
		{
			SAVE_VAR(Vars::Visuals::RemoveDisguises);
			SAVE_VAR(Vars::Visuals::RemoveTaunts);
			SAVE_VAR(Vars::Visuals::FieldOfView);
			SAVE_VAR(Vars::Visuals::AimFOVAlpha);
			SAVE_VAR(Vars::Visuals::Fullbright);
			SAVE_VAR(Vars::Visuals::AspectRatioValue);
			SAVE_VAR(Vars::Visuals::ViewModelX);
			SAVE_VAR(Vars::Visuals::ViewModelY);
			SAVE_VAR(Vars::Visuals::ViewModelZ);
			SAVE_VAR(Vars::Visuals::RemoveScope);
			SAVE_VAR(Vars::Visuals::RemoveZoom);
			SAVE_VAR(Vars::Visuals::RemovePunch);
			SAVE_VAR(Vars::Visuals::CrosshairAimPos);
			SAVE_VAR(Vars::Visuals::ChatInfo);

			SAVE_VAR(Vars::Visuals::SpectatorList);

			SAVE_VAR(Vars::Visuals::SpyWarning);
			SAVE_VAR(Vars::Visuals::SpyWarningAnnounce);
			SAVE_VAR(Vars::Visuals::SpyWarningStyle);
			SAVE_VAR(Vars::Visuals::SpyWarningVisibleOnly);
			SAVE_VAR(Vars::Visuals::SpyWarningIgnoreFriends);

			SAVE_VAR(Vars::Visuals::Snow);
			SAVE_VAR(Vars::Visuals::ToolTips);

			SAVE_VAR(Vars::Visuals::ThirdPerson);
			SAVE_VAR(Vars::Visuals::ThirdPersonKey);
			SAVE_VAR(Vars::Visuals::ThirdPersonSilentAngles);
			SAVE_VAR(Vars::Visuals::ThirdpersonOffsetX);
			SAVE_VAR(Vars::Visuals::ThirdpersonOffsetY);
			SAVE_VAR(Vars::Visuals::ThirdpersonOffsetZ);
			SAVE_VAR(Vars::Visuals::ThirdpersonCrosshair);

			SAVE_VAR(Vars::Visuals::WorldModulation);

			SAVE_VAR(Vars::Visuals::SkyboxChanger);
			SAVE_VAR(Vars::Visuals::SkyModulation);

			SAVE_VAR(Vars::Visuals::DevTextures);

			SAVE_VAR(Vars::Visuals::PlayerList);
			//#ifdef DEVELOPER_BUILD
			SAVE_VAR(Vars::Visuals::Skins::Enabled);
			//#endif
			SAVE_VAR(Vars::Visuals::TracerEffect);
			SAVE_VAR(Vars::Visuals::RagdollEffect);
			SAVE_VAR(Vars::Visuals::AimbotViewmodel);
		}

		//Misc
		{
			SAVE_VAR(Vars::Misc::AutoJump);
			SAVE_VAR(Vars::Misc::InstantRespawn);
			SAVE_VAR(Vars::Misc::TauntSlide);
			SAVE_VAR(Vars::Misc::TauntControl);
			SAVE_VAR(Vars::Misc::TauntControlMouse);
			SAVE_VAR(Vars::Misc::Freecam);
			SAVE_VAR(Vars::Misc::FreecamKey);
			SAVE_VAR(Vars::Misc::FreecamSpeed);
			SAVE_VAR(Vars::Misc::BypassPure);
			SAVE_VAR(Vars::Misc::NoisemakerSpam);
			SAVE_VAR(Vars::Misc::DisableInterpolation);
			SAVE_VAR(Vars::Misc::MedalFlip);
			SAVE_VAR(Vars::Misc::AutoRocketJump);
			SAVE_VAR(Vars::Misc::ChatSpam);
			SAVE_VAR(Vars::Misc::NoPush);
			SAVE_VAR(Vars::Misc::AutoStrafe);
			SAVE_VAR(Vars::Misc::EdgeJump);
			SAVE_VAR(Vars::Misc::AntiAFK);
			SAVE_VAR(Vars::Misc::VoteRevealer);
			SAVE_VAR(Vars::Misc::SteamRPC);
			// CL_Move
			{
				SAVE_VAR(Vars::Misc::CL_Move::Doubletap);// { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::WaitForDT);// { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::NotInAir);// { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::DoubletapKey);// { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::RechargeKey);// { 0x52, L"Recharge Key" }; //R
				SAVE_VAR(Vars::Misc::CL_Move::DoubletapKey);// { 0x52, L"Recharge Key" }; //R
				SAVE_VAR(Vars::Misc::CL_Move::TeleportKey);
				SAVE_VAR(Vars::Misc::CL_Move::DTTicks);
			}
		}

		//AntiHack
		{
			//AntiAim
			{
				SAVE_VAR(Vars::AntiHack::AntiAim::Active);
				SAVE_VAR(Vars::AntiHack::AntiAim::Pitch);
				SAVE_VAR(Vars::AntiHack::AntiAim::Yaw);
				SAVE_VAR(Vars::AntiHack::AntiAim::Fake);
				SAVE_VAR(Vars::AntiHack::AntiAim::Right);
				SAVE_VAR(Vars::AntiHack::AntiAim::Left);
			}
		}

		//Others
		{
			//SAVE_OTHER(Vars::Menu::Colors::WindowBackground);
			//SAVE_OTHER(Vars::Menu::Colors::TitleBar);
			SAVE_OTHER(Vars::Menu::Colors::Text);
			//SAVE_OTHER(Vars::Menu::Colors::Widget);
			SAVE_OTHER(Vars::Menu::Colors::WidgetActive);
			SAVE_OTHER(Vars::Menu::Colors::ShadowColor);
			SAVE_OTHER(Vars::Menu::ShadowSize);
			SAVE_OTHER(Vars::Menu::Colors::OutlineMenu);

			SAVE_OTHER(Colors::OutlineESP);
			SAVE_OTHER(Colors::Cond);
			SAVE_OTHER(Colors::Target);
			SAVE_OTHER(Colors::Invuln);
			SAVE_OTHER(Colors::Cloak);
			SAVE_OTHER(Colors::Friend);
			SAVE_OTHER(Colors::NameColor);
			SAVE_OTHER(Colors::BNameColor);
			SAVE_OTHER(Colors::Overheal);
			SAVE_OTHER(Colors::Health);
			SAVE_OTHER(Colors::Ammo);
			SAVE_OTHER(Colors::UberColor);
			SAVE_OTHER(Colors::TeamRed);
			SAVE_OTHER(Colors::TeamBlu);
			SAVE_OTHER(Colors::Hands);
			SAVE_OTHER(Colors::Weapon);
			SAVE_OTHER(Colors::WorldModulation);
			SAVE_OTHER(Colors::FresnelBase);
			SAVE_OTHER(Colors::StaticPropModulation);
			SAVE_OTHER(Colors::SkyModulation);
			SAVE_OTHER(Colors::FOVCircle);
			SAVE_OTHER(Colors::Bones);
			SAVE_OTHER(Colors::Healthbar);
			SAVE_OTHER(Colors::hitboxColor);


			SAVE_OTHER(g_Radar.m_nRadarX);
			SAVE_OTHER(g_Radar.m_nRadarY);

			SAVE_OTHER(g_SpectatorList.m_nSpecListX);
			SAVE_OTHER(g_SpectatorList.m_nSpecListY);

			SAVE_STRING(Vars::Skybox::SkyboxName);
			SAVE_STRING(Vars::Skybox::skyboxnum);
			SAVE_STRING(Vars::Fart::customFont);
		}

		m_Write.close();
	}
}

void CConfigManager::Load(const wchar_t* name)
{
	m_Read = std::wifstream(m_sConfigPath + L"\\" + name + _(L".poop"));

	if (m_Read.is_open())
	{
		//Aimbot
		{
			//Global
			{
				LOAD_VAR(Vars::Aimbot::Global::Active);
				LOAD_VAR(Vars::Aimbot::Global::AimKey);
				LOAD_VAR(Vars::Aimbot::Global::AutoShoot);
				LOAD_VAR(Vars::Aimbot::Global::AimPlayers);
				LOAD_VAR(Vars::Aimbot::Global::AimBuildings);
				LOAD_VAR(Vars::Aimbot::Global::IgnoreInvlunerable);
				LOAD_VAR(Vars::Aimbot::Global::IgnoreCloaked);
				LOAD_VAR(Vars::Aimbot::Global::IgnoreFriends);
				LOAD_VAR(Vars::Aimbot::Global::IgnoreTaunting);
				LOAD_VAR(Vars::Aimbot::Global::IgnoreVaccinator);
			}

			//Hitscan
			{
				LOAD_VAR(Vars::Aimbot::Hitscan::Active);
				LOAD_VAR(Vars::Aimbot::Hitscan::SortMethod);
				LOAD_VAR(Vars::Aimbot::Hitscan::AimMethod);
				LOAD_VAR(Vars::Aimbot::Hitscan::AimHitbox);
				LOAD_VAR(Vars::Aimbot::Hitscan::AimFOV);
				LOAD_VAR(Vars::Aimbot::Hitscan::SmoothingAmount);
				LOAD_VAR(Vars::Aimbot::Hitscan::TapFire);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScanHitboxes);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScanHead);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScanBuildings);
				LOAD_VAR(Vars::Aimbot::Hitscan::WaitForHeadshot);
				LOAD_VAR(Vars::Aimbot::Hitscan::WaitForCharge);
				LOAD_VAR(Vars::Aimbot::Hitscan::SpectatedSmooth);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScopedOnly);
				LOAD_VAR(Vars::Aimbot::Hitscan::AutoScope);
				LOAD_VAR(Vars::Aimbot::Hitscan::AutoRev);
			}

			//Projectile
			{
				LOAD_VAR(Vars::Aimbot::Projectile::Active);
				LOAD_VAR(Vars::Aimbot::Projectile::PerformanceMode);
				//LOAD_VAR(Vars::Aimbot::Projectile::SortMethod);
				LOAD_VAR(Vars::Aimbot::Projectile::AimMethod);
				LOAD_VAR(Vars::Aimbot::Projectile::AimPosition);
				LOAD_VAR(Vars::Aimbot::Projectile::AimFOV);
				LOAD_VAR(Vars::Aimbot::Projectile::R8Method);
			}

			//Melee
			{
				LOAD_VAR(Vars::Aimbot::Melee::Active);
				//LOAD_VAR(Vars::Aimbot::Melee::SortMethod);
				LOAD_VAR(Vars::Aimbot::Melee::AimMethod);
				LOAD_VAR(Vars::Aimbot::Melee::AimFOV);
				LOAD_VAR(Vars::Aimbot::Melee::SmoothingAmount);
				LOAD_VAR(Vars::Aimbot::Melee::RangeCheck);
				LOAD_VAR(Vars::Aimbot::Melee::PredictSwing);
			}
		}

		//Triggerbot
		{
			//Global
			{
				LOAD_VAR(Vars::Triggerbot::Global::Active);
				LOAD_VAR(Vars::Triggerbot::Global::TriggerKey);
				LOAD_VAR(Vars::Triggerbot::Global::IgnoreInvlunerable);
				LOAD_VAR(Vars::Triggerbot::Global::IgnoreCloaked);
				LOAD_VAR(Vars::Triggerbot::Global::IgnoreFriends);
				LOAD_VAR(Vars::Triggerbot::Global::IgnoreTaunts);
			}

			//Shoot
			{
				LOAD_VAR(Vars::Triggerbot::Shoot::Active);
				LOAD_VAR(Vars::Triggerbot::Shoot::TriggerPlayers);
				LOAD_VAR(Vars::Triggerbot::Shoot::TriggerBuildings);
				LOAD_VAR(Vars::Triggerbot::Shoot::HeadOnly);
				LOAD_VAR(Vars::Triggerbot::Shoot::WaitForHeadshot);
				LOAD_VAR(Vars::Triggerbot::Shoot::ScopedOnly);
				LOAD_VAR(Vars::Triggerbot::Shoot::HeadScale);
			}

			//Stab
			{
				LOAD_VAR(Vars::Triggerbot::Stab::Active);
				LOAD_VAR(Vars::Triggerbot::Stab::RageMode);
				LOAD_VAR(Vars::Triggerbot::Stab::Silent);
				LOAD_VAR(Vars::Triggerbot::Stab::Disguise);
				LOAD_VAR(Vars::Triggerbot::Stab::IgnRazor);
				LOAD_VAR(Vars::Triggerbot::Stab::Range);
			}

			//Detonate
			{
				LOAD_VAR(Vars::Triggerbot::Detonate::Active);
				LOAD_VAR(Vars::Triggerbot::Detonate::Stickies);
				LOAD_VAR(Vars::Triggerbot::Detonate::Flares);
				LOAD_VAR(Vars::Triggerbot::Detonate::RadiusScale);
			}

			//Blast
			{
				LOAD_VAR(Vars::Triggerbot::Blast::Active);
				LOAD_VAR(Vars::Triggerbot::Blast::Rage);
				LOAD_VAR(Vars::Triggerbot::Blast::Silent);
				LOAD_VAR(Vars::Triggerbot::Blast::Fov);
			}

			//Uber
			{
				LOAD_VAR(Vars::Triggerbot::Uber::Active);
				LOAD_VAR(Vars::Triggerbot::Uber::OnlyFriends);
				LOAD_VAR(Vars::Triggerbot::Uber::PopLocal);
				LOAD_VAR(Vars::Triggerbot::Uber::HealthLeft);
			}

			{
				LOAD_VAR(Vars::Triggerbot::AutoVaccinator::Active);
				LOAD_VAR(Vars::Triggerbot::AutoVaccinator::OnlyFriends);
				LOAD_VAR(Vars::Triggerbot::AutoVaccinator::UberMaxPercentage);
			}
		}

		//ESP
		{
			//Main
			{
				//LOAD_VAR(Vars::ESP::Main::Active);
				//LOAD_VAR(Vars::ESP::Main::Outline);
			}

			//Players
			{
				LOAD_VAR(Vars::ESP::Players::Active);
				LOAD_VAR(Vars::ESP::Players::ShowLocal);
				LOAD_VAR(Vars::ESP::Players::IgnoreTeammates);
				LOAD_VAR(Vars::ESP::Players::IgnoreCloaked);
				LOAD_VAR(Vars::ESP::Players::Name);
				LOAD_VAR(Vars::ESP::Players::NameCustom);
				LOAD_VAR(Vars::ESP::Players::Uber);
				LOAD_VAR(Vars::ESP::Players::Class);
				//LOAD_VAR(Vars::ESP::Players::Health);
				LOAD_VAR(Vars::ESP::Players::Cond);
				LOAD_VAR(Vars::ESP::Players::Healthbar::Enabled);
				LOAD_VAR(Vars::ESP::Players::Box);
				LOAD_VAR(Vars::ESP::Players::GUID);
				LOAD_VAR(Vars::ESP::Players::Alpha);
				LOAD_VAR(Vars::ESP::Players::Lines);
				LOAD_VAR(Vars::ESP::Players::Bones);
				LOAD_VAR(Vars::ESP::Players::Dlights);
				LOAD_VAR(Vars::ESP::Players::DlightRadius);
				//Arrows
				{
					LOAD_VAR(Vars::ESP::Players::Arrows::Active);
					LOAD_VAR(Vars::ESP::Players::Arrows::DistFromCenter);
					LOAD_VAR(Vars::ESP::Players::Arrows::ArrowSize);
				}
			}

			//Buildings
			{
				LOAD_VAR(Vars::ESP::Buildings::Active);
				LOAD_VAR(Vars::ESP::Buildings::IgnoreTeammates);
				LOAD_VAR(Vars::ESP::Buildings::Name);
				LOAD_VAR(Vars::ESP::Buildings::NameCustom);
				LOAD_VAR(Vars::ESP::Buildings::Health);
				LOAD_VAR(Vars::ESP::Buildings::Owner);
				LOAD_VAR(Vars::ESP::Buildings::Level);
				LOAD_VAR(Vars::ESP::Buildings::Cond);
				LOAD_VAR(Vars::ESP::Buildings::HealthBar);
				LOAD_VAR(Vars::ESP::Buildings::Box);
				LOAD_VAR(Vars::ESP::Buildings::Alpha);
				LOAD_VAR(Vars::ESP::Buildings::Dlights);
				LOAD_VAR(Vars::ESP::Buildings::DlightRadius);
			}

			//World
			{
				LOAD_VAR(Vars::ESP::World::Active);
				LOAD_VAR(Vars::ESP::World::HealthText);
				LOAD_VAR(Vars::ESP::World::AmmoText);
				LOAD_VAR(Vars::ESP::World::Projectiles);
				LOAD_VAR(Vars::ESP::World::Alpha);
			}
		}

		//Chams
		{
			//Main
			/*
			{
				LOAD_VAR(Vars::Chams::Main::Active);
			}
			*/

			//Players
			{
				LOAD_VAR(Vars::Chams::Players::Active);
				LOAD_VAR(Vars::Chams::Players::ShowLocal);
				LOAD_VAR(Vars::Chams::Players::IgnoreTeammates);
				LOAD_VAR(Vars::Chams::Players::Wearables);
				LOAD_VAR(Vars::Chams::Players::Weapons);
				LOAD_VAR(Vars::Chams::Players::Material);
				LOAD_VAR(Vars::Chams::Players::IgnoreZ);
				LOAD_VAR(Vars::Chams::Players::Alpha);
				LOAD_VAR(Vars::Chams::Players::HitboxThing);
				LOAD_VAR(Vars::Chams::Players::HitboxTimeThing);
			}

			//Buildings
			{
				LOAD_VAR(Vars::Chams::Buildings::Active);
				LOAD_VAR(Vars::Chams::Buildings::IgnoreTeammates);
				LOAD_VAR(Vars::Chams::Buildings::Material);
				LOAD_VAR(Vars::Chams::Buildings::IgnoreZ);
				LOAD_VAR(Vars::Chams::Buildings::Alpha);
			}

			//World
			{
				LOAD_VAR(Vars::Chams::World::Active);
				LOAD_VAR(Vars::Chams::World::Health);
				LOAD_VAR(Vars::Chams::World::Ammo);
				LOAD_VAR(Vars::Chams::World::Projectiles);
				LOAD_VAR(Vars::Chams::World::Material);
				LOAD_VAR(Vars::Chams::World::IgnoreZ);
				LOAD_VAR(Vars::Chams::World::Alpha);
			}

			//DME
			{
				LOAD_VAR(Vars::Chams::DME::Active);
				LOAD_VAR(Vars::Chams::DME::Hands);
				LOAD_VAR(Vars::Chams::DME::HandsAlpha);
				LOAD_VAR(Vars::Chams::DME::Weapon);
				LOAD_VAR(Vars::Chams::DME::WeaponAlpha);
			}
		}

		//Glow
		{
			//Main
			{
				//LOAD_VAR(Vars::Glow::Main::Active);
				LOAD_VAR(Vars::Glow::Main::Wireframe);
				LOAD_VAR(Vars::Glow::Main::Scale);
			}

			//Players
			{
				LOAD_VAR(Vars::Glow::Players::Active);
				LOAD_VAR(Vars::Glow::Players::ShowLocal);
				LOAD_VAR(Vars::Glow::Players::IgnoreTeammates);
				LOAD_VAR(Vars::Glow::Players::Wearables);
				LOAD_VAR(Vars::Glow::Players::Weapons);
				LOAD_VAR(Vars::Glow::Players::Alpha);
				LOAD_VAR(Vars::Glow::Players::Color);
			}

			//Buildings
			{
				LOAD_VAR(Vars::Glow::Buildings::Active);
				LOAD_VAR(Vars::Glow::Buildings::IgnoreTeammates);
				LOAD_VAR(Vars::Glow::Buildings::Alpha);
				LOAD_VAR(Vars::Glow::Buildings::Color);
			}

			//World
			{
				LOAD_VAR(Vars::Glow::World::Active);
				LOAD_VAR(Vars::Glow::World::Health);
				LOAD_VAR(Vars::Glow::World::Ammo);
				LOAD_VAR(Vars::Glow::World::Projectiles);
				LOAD_VAR(Vars::Glow::World::Alpha);
			}
		}

		//Radar
		{
			//Main
			{
				LOAD_VAR(Vars::Radar::Main::Active);
				LOAD_VAR(Vars::Radar::Main::BackAlpha);
				LOAD_VAR(Vars::Radar::Main::Size);
				LOAD_VAR(Vars::Radar::Main::Range);
			}

			//Players
			{
				LOAD_VAR(Vars::Radar::Players::Active);
				LOAD_VAR(Vars::Radar::Players::IconType);
				//LOAD_VAR(Vars::Radar::Players::BackGroundType);
				//LOAD_VAR(Vars::Radar::Players::Outline);
				LOAD_VAR(Vars::Radar::Players::IgnoreTeam);
				//LOAD_VAR(Vars::Radar::Players::IgnoreCloaked);
				LOAD_VAR(Vars::Radar::Players::Health);
			}

			//Buildings
			{
				LOAD_VAR(Vars::Radar::Buildings::Active);
				//LOAD_VAR(Vars::Radar::Buildings::Outline);
				LOAD_VAR(Vars::Radar::Buildings::IgnoreTeam);
				LOAD_VAR(Vars::Radar::Buildings::Health);
			}

			//World
			{
				//LOAD_VAR(Vars::Radar::World::Active);
				//LOAD_VAR(Vars::Radar::World::Health);
				//LOAD_VAR(Vars::Radar::World::Ammo);
			}
		}

		//Visuals
		{
			LOAD_VAR(Vars::Visuals::RemoveDisguises);
			LOAD_VAR(Vars::Visuals::RemoveTaunts);
			LOAD_VAR(Vars::Visuals::FieldOfView);
			LOAD_VAR(Vars::Visuals::AimFOVAlpha);
			LOAD_VAR(Vars::Visuals::Fullbright);
			LOAD_VAR(Vars::Visuals::AspectRatioValue);
			LOAD_VAR(Vars::Visuals::ViewModelX);
			LOAD_VAR(Vars::Visuals::ViewModelY);
			LOAD_VAR(Vars::Visuals::ViewModelZ);
			LOAD_VAR(Vars::Visuals::RemoveScope);
			LOAD_VAR(Vars::Visuals::RemoveZoom);
			LOAD_VAR(Vars::Visuals::RemovePunch);
			LOAD_VAR(Vars::Visuals::CrosshairAimPos);
			LOAD_VAR(Vars::Visuals::ChatInfo);

			LOAD_VAR(Vars::Visuals::SpectatorList);

			LOAD_VAR(Vars::Visuals::SpyWarning);
			LOAD_VAR(Vars::Visuals::SpyWarningAnnounce);
			LOAD_VAR(Vars::Visuals::SpyWarningStyle);
			LOAD_VAR(Vars::Visuals::SpyWarningVisibleOnly);
			LOAD_VAR(Vars::Visuals::SpyWarningIgnoreFriends);

			LOAD_VAR(Vars::Visuals::Snow);
			LOAD_VAR(Vars::Visuals::ToolTips);

			LOAD_VAR(Vars::Visuals::ThirdPerson);
			LOAD_VAR(Vars::Visuals::ThirdPersonKey);
			LOAD_VAR(Vars::Visuals::ThirdPersonSilentAngles);
			LOAD_VAR(Vars::Visuals::ThirdpersonOffsetX);
			LOAD_VAR(Vars::Visuals::ThirdpersonOffsetY);
			LOAD_VAR(Vars::Visuals::ThirdpersonOffsetZ);
			LOAD_VAR(Vars::Visuals::ThirdpersonCrosshair);

			LOAD_VAR(Vars::Visuals::WorldModulation);

			LOAD_VAR(Vars::Visuals::SkyboxChanger);
			LOAD_VAR(Vars::Visuals::SkyModulation);

			LOAD_VAR(Vars::Visuals::DevTextures);

			LOAD_VAR(Vars::Visuals::PlayerList);
			//#ifdef DEVELOPER_BUILD
			LOAD_VAR(Vars::Visuals::Skins::Enabled);
			//#endif
			LOAD_VAR(Vars::Visuals::TracerEffect);
			LOAD_VAR(Vars::Visuals::RagdollEffect);
			LOAD_VAR(Vars::Visuals::AimbotViewmodel);
		}

		//Misc
		{
			LOAD_VAR(Vars::Misc::AutoJump);
			LOAD_VAR(Vars::Misc::InstantRespawn);
			LOAD_VAR(Vars::Misc::TauntSlide);
			LOAD_VAR(Vars::Misc::TauntControl);
			LOAD_VAR(Vars::Misc::TauntControlMouse);
			LOAD_VAR(Vars::Misc::Freecam);
			LOAD_VAR(Vars::Misc::FreecamKey);
			LOAD_VAR(Vars::Misc::FreecamSpeed);
			LOAD_VAR(Vars::Misc::BypassPure);
			LOAD_VAR(Vars::Misc::NoisemakerSpam);
			LOAD_VAR(Vars::Misc::DisableInterpolation);
			LOAD_VAR(Vars::Misc::MedalFlip);
			LOAD_VAR(Vars::Misc::AutoRocketJump);
			LOAD_VAR(Vars::Misc::ChatSpam);
			LOAD_VAR(Vars::Misc::NoPush);
			LOAD_VAR(Vars::Misc::AutoStrafe);
			LOAD_VAR(Vars::Misc::EdgeJump);
			LOAD_VAR(Vars::Misc::AntiAFK);
			LOAD_VAR(Vars::Misc::VoteRevealer);
			LOAD_VAR(Vars::Misc::SteamRPC);

			// CL_Move
			{
				LOAD_VAR(Vars::Misc::CL_Move::Doubletap);// { true, L"Doubletap" };
				LOAD_VAR(Vars::Misc::CL_Move::WaitForDT);// { true, L"Doubletap" };
				LOAD_VAR(Vars::Misc::CL_Move::NotInAir);// { true, L"Doubletap" };
				LOAD_VAR(Vars::Misc::CL_Move::DoubletapKey);// { true, L"Doubletap" };
				LOAD_VAR(Vars::Misc::CL_Move::RechargeKey);// { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::DoubletapKey);// { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::TeleportKey);
				LOAD_VAR(Vars::Misc::CL_Move::DTTicks);
			}
		}

		//AntiHack
		{
			//AntiAim
			{
				LOAD_VAR(Vars::AntiHack::AntiAim::Active);
				LOAD_VAR(Vars::AntiHack::AntiAim::Pitch);
				LOAD_VAR(Vars::AntiHack::AntiAim::Yaw);
				LOAD_VAR(Vars::AntiHack::AntiAim::Fake);
				LOAD_VAR(Vars::AntiHack::AntiAim::Right);
				LOAD_VAR(Vars::AntiHack::AntiAim::Left);
			}
		}

		//Others
		{
			//LOAD_OTHER(Vars::Menu::Colors::WindowBackground);
			//LOAD_OTHER(Vars::Menu::Colors::TitleBar);
			LOAD_OTHER(Vars::Menu::Colors::Text);
			//LOAD_OTHER(Vars::Menu::Colors::Widget);
			LOAD_OTHER(Vars::Menu::Colors::WidgetActive);
			LOAD_OTHER(Vars::Menu::Colors::ShadowColor);
			LOAD_OTHER(Vars::Menu::ShadowSize);
			LOAD_OTHER(Vars::Menu::Colors::OutlineMenu);

			LOAD_OTHER(Colors::OutlineESP);
			LOAD_OTHER(Colors::Cond);
			LOAD_OTHER(Colors::Target);
			LOAD_OTHER(Colors::Invuln);
			LOAD_OTHER(Colors::Cloak);
			LOAD_OTHER(Colors::Friend);
			LOAD_OTHER(Colors::NameColor);
			LOAD_OTHER(Colors::BNameColor);
			LOAD_OTHER(Colors::Overheal);
			LOAD_OTHER(Colors::Health);
			LOAD_OTHER(Colors::Ammo);
			LOAD_OTHER(Colors::UberColor);
			LOAD_OTHER(Colors::TeamRed);
			LOAD_OTHER(Colors::TeamBlu);
			LOAD_OTHER(Colors::Hands);
			LOAD_OTHER(Colors::Weapon);
			LOAD_OTHER(Colors::WorldModulation);
			LOAD_OTHER(Colors::FresnelBase);
			LOAD_OTHER(Colors::StaticPropModulation);
			LOAD_OTHER(Colors::SkyModulation);
			LOAD_OTHER(Colors::FOVCircle);
			LOAD_OTHER(Colors::Bones);
			LOAD_OTHER(Colors::Healthbar);
			LOAD_OTHER(Colors::hitboxColor);

			LOAD_OTHER(g_Radar.m_nRadarX);
			LOAD_OTHER(g_Radar.m_nRadarY);

			LOAD_OTHER(g_SpectatorList.m_nSpecListX);
			LOAD_OTHER(g_SpectatorList.m_nSpecListY);

			LOAD_STRING(Vars::Skybox::SkyboxName);
			LOAD_STRING(Vars::Skybox::skyboxnum);
			LOAD_STRING(Vars::Fart::customFont);
		}

		m_Read.close();
	}
}
void CConfigManager::Remove(const wchar_t* name)
{
	std::filesystem::remove(m_sConfigPath + L"\\" + name + _(L".poop"));
}