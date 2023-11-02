#include "Visuals.h"
#include "../Vars.h"
#include <format>

char buffer[256];


typedef bool(_cdecl* LoadNamedSkiesFn)(const char*);
static LoadNamedSkiesFn LoadSkies = (LoadNamedSkiesFn)g_Pattern.Find(_(L"engine.dll"), _(L"55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 56 57 8B 01 C7 45"));


static bool viewanglesCopied = false;
static Vec3 viewangles;
static Vec3 pViewangles;
static Vec3 vForward{}, vRight{}, vUp{};
Vec3 originBackup{};
Vec3 pCmdViewangles;

void CVisuals::FreecamCM(CUserCmd* pCmd) {
	if (Vars::Misc::Freecam.m_Var) {
		if (GetAsyncKeyState(Vars::Misc::FreecamKey.m_Var)) {
			pCmd->forwardmove = pCmd->sidemove = 0.0f;
			if (!g_GlobalInfo.m_bAttacking) {
				pCmd->viewangles = pCmdViewangles;
			}
		}
		else {
			pCmdViewangles = pCmd->viewangles;
		}
	}
}

void CVisuals::DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colour, float time) {
	g_Interfaces.DebugOverlay->ClearAllOverlays();
	const model_t* model;
	studiohdr_t* hdr;
	mstudiohitboxset_t* set;
	mstudiobbox_t* bbox;
	Vec3 mins{}, maxs{}, origin{};
	Vec3 angle;

	model = pEntity->GetModel();
	/*if (!model)
		return;*/

	hdr = g_Interfaces.ModelInfo->GetStudioModel(model);
	/*if (!hdr)
		return;*/

	set = hdr->GetHitboxSet(pEntity->GetHitboxSet());
	/*if (!set)
		return;*/

	for (int i{}; i < set->numhitboxes; ++i) {
		bbox = set->hitbox(i);
		if (!bbox)
			continue;

		//nigga balls
		/*if (bbox->m_radius <= 0.f) {*/
		matrix3x4 rot_matrix;
		Math::AngleMatrix(bbox->angle, rot_matrix);

		matrix3x4 matrix;
		matrix3x4 boneees[128];
		pEntity->SetupBones(boneees, 128, BONE_USED_BY_ANYTHING, g_Interfaces.GlobalVars->curtime);
		Math::ConcatTransforms(boneees[bbox->bone], rot_matrix, matrix);

		Vec3 bbox_angle;
		Math::MatrixAngles(matrix, bbox_angle);

		Vec3 matrix_origin;
		Math::GetMatrixOrigin(matrix, matrix_origin);

		g_Interfaces.DebugOverlay->AddBoxOverlay(matrix_origin, bbox->bbmin, bbox->bbmax, bbox_angle, colour.r, colour.g, colour.b, colour.a, time);
	}
}

void CVisuals::Freecam(CViewSetup* pView)
{
	float moveSpeed = Vars::Misc::FreecamSpeed.m_Var;
	if (Vars::Misc::Freecam.m_Var) {
		if (pView) {
			if (auto pLocal = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer())) {
				if (GetAsyncKeyState(Vars::Misc::FreecamKey.m_Var)) {
					if (!viewanglesCopied) {
						viewangles = pLocal->GetEyeAngles();

						originBackup = pView->origin;
						viewanglesCopied = true;

					}
					pViewangles = g_Interfaces.Engine->GetViewAngles();
					Math::AngleVectors(pViewangles, &vForward, &vRight, &vUp);
					pLocal->SetEyeAngles(viewangles);
					pView->origin = originBackup;
					if (GetAsyncKeyState(VK_W)) {
						pView->origin += vForward * moveSpeed;
					}
					if (GetAsyncKeyState(VK_S)) {
						pView->origin -= vForward * moveSpeed;
					}
					if (GetAsyncKeyState(VK_A)) {
						pView->origin -= vRight * moveSpeed;
					}
					if (GetAsyncKeyState(VK_D)) {
						pView->origin += vRight * moveSpeed;
					}
					if (GetAsyncKeyState(VK_SPACE)) {
						pView->origin += vUp * moveSpeed;
					}
					if (GetAsyncKeyState(VK_CONTROL)) {
						pView->origin -= vUp * moveSpeed;
					}
					originBackup = pView->origin;
				}
				else {
					viewanglesCopied = false;
				}
			}
		}
	}
}


void CVisuals::SkyboxChanger() {
	const char* skybNames[] = {
		"Custom",
		"sky_tf2_04",
		"sky_upward",
		"sky_dustbowl_01",
		"sky_goldrush_01",
		"sky_granary_01",
		"sky_well_01",
		"sky_gravel_01",
		"sky_badlands_01",
		"sky_hydro_01",
		"sky_night_01",
		"sky_nightfall_01",
		"sky_trainyard_01",
		"sky_stormfront_01",
		"sky_morningsnow_01",
		"sky_alpinestorm_01",
		"sky_harvest_01",
		"sky_harvest_night_01",
		"sky_halloween",
		"sky_halloween_night_01",
		"sky_halloween_night2014_01",
		"sky_island_01",
		"sky_rainbow_01"
	};
	if (Vars::Visuals::SkyboxChanger.m_Var) {
		if (Vars::Skybox::skyboxnum == 0) {
			if (Vars::Misc::BypassPure.m_Var) {
				LoadSkies(Vars::Skybox::SkyboxName.c_str());
			}
			else {
				LoadSkies(g_Interfaces.CVars->FindVar(_("sv_skyname"))->GetString());
			}
		}
		else {
			LoadSkies(skybNames[Vars::Skybox::skyboxnum]);
		}
	}
	else {
		LoadSkies(g_Interfaces.CVars->FindVar(_("sv_skyname"))->GetString());
	}
}

void CVisuals::DevTextures()
{
	if (!Vars::Visuals::DevTextures.m_Var)
		return;

	static KeyValues* KV = nullptr;

	if (!KV)
	{
		KV = new KeyValues("LightmappedGeneric");
		KV->SetString("$basetexture", "dev/dev_measuregeneric01b");
	}

	for (MaterialHandle_t i = g_Interfaces.MatSystem->First(); i != g_Interfaces.MatSystem->Invalid(); i = g_Interfaces.MatSystem->Next(i))
	{
		IMaterial* pMaterial = g_Interfaces.MatSystem->Get(i);

		if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached() || pMaterial->IsTranslucent() || pMaterial->IsSpriteCard() || std::string(pMaterial->GetTextureGroupName()).find("World") == std::string::npos)
			continue;

		std::string sName = std::string(pMaterial->GetName());

		if (sName.find("water") != std::string::npos || sName.find("glass") != std::string::npos
			|| sName.find("door") != std::string::npos || sName.find("tools") != std::string::npos
			|| sName.find("player") != std::string::npos || sName.find("wall28") != std::string::npos
			|| sName.find("wall26") != std::string::npos || sName.find("decal") != std::string::npos
			|| sName.find("overlay") != std::string::npos || sName.find("hay") != std::string::npos)
			continue;

		pMaterial->SetShaderAndParams(KV);
	}
}

void CVisuals::AddToEventLog(const char* string...) {
	va_list va_alist;
	char cbuffer[1024] = { '\0' };

	va_start(va_alist, string);
	vsprintf_s(cbuffer, string, va_alist);
	va_end(va_alist);

	g_Visuals.vecEventVector.emplace_back(EventLogging_t{ cbuffer });
}

void CVisuals::RunEventLogs()
{
	for (auto i = 0; i < (int)vecEventVector.size(); i++) {
		auto& log = vecEventVector[i];
		auto time_delta = fabs(g_Interfaces.GlobalVars->realtime - log.flTime);

		if (vecEventVector.size() > 12 || time_delta > flTextTime) {
			vecEventVector.erase(vecEventVector.begin() + i);
			continue;
		}

		auto height = flIdealHeight + (20 * i),
			width = flIdealWidth;

		auto w2 = 0;

		if (time_delta < flTextFadeInTime) {
			log.flAlpha = ((time_delta / flTextFadeInTime) * 255.f);
			width = (time_delta / flTextFadeInTime) * (float)(flSlideInDistance)+(flIdealWidth - flSlideInDistance);
			w2 = width;
		}

		if (time_delta > flTextTime - flTextFadeOutTime) {
			log.flAlpha = (255 - (((time_delta - (flTextTime - flTextFadeOutTime)) / flTextFadeOutTime) * 255.f));
			width = flIdealWidth - (((time_delta - (flTextTime - flTextFadeOutTime)) / flTextFadeOutTime) * (float)(flSlideOutDistance));
		}
		// Idk i put flAlpha on everything, looks cooler lol
		static int w, h;
		g_Interfaces.Surface->GetTextSize(FONT_MENU_OUTLINED, Utils::ConvertUtf8ToWide(log.szText.c_str()).data(), w, h);
		Color_t color = Vars::Menu::Colors::WidgetActive;

		Color_t logclr;
		logclr.a = (time_delta < flTextFadeInTime) ? (time_delta / flTextFadeInTime) * 255.f : 255 - (((time_delta - (flTextTime - flTextFadeOutTime)) / flTextFadeOutTime) * 255.f);
		// I have to do it the ghetto way because woohooo gradients suck ass on seowned for some reason!!!
		g_Interfaces.Surface->SetDrawColor(0, 0, 0, log.flAlpha);
		g_Interfaces.Surface->DrawFilledRectFade(w2 - 5, height - 2, w - 30 + width, height + 15, log.flAlpha, 0, true);
		g_Draw.Rect(w2, height - 2, 2, 17, Vars::Menu::Colors::WidgetActive);
		g_Draw.String(FONT_MENU_OUTLINED, width + 5, height, { 255, 255, 255, 255}, ALIGN_DEFAULT, Utils::ConvertUtf8ToWide(log.szText.c_str()).data());
	}
}

bool CVisuals::RemoveScope(int nPanel)
{
	if (!m_nHudZoom && Hash::IsHudScope(g_Interfaces.Panel->GetName(nPanel)))
		m_nHudZoom = nPanel;

	return (Vars::Visuals::RemoveScope.m_Var && nPanel == m_nHudZoom);
}

void CVisuals::OffsetCamera(CViewSetup* pView) {
	CBaseEntity* pLocal = g_EntityCache.m_pLocal;

	if (pLocal && pView)
	{
		if (g_Interfaces.Input->CAM_IsThirdPerson()) {
			Vec3 pViewangles = g_Interfaces.Engine->GetViewAngles();
			Vec3 vForward{}, vRight{}, vUp{};
			Math::AngleVectors(pViewangles, &vForward, &vRight, &vUp);

			pView->origin += vForward * Vars::Visuals::ThirdpersonOffsetX.m_Var;
			pView->origin += vRight * Vars::Visuals::ThirdpersonOffsetY.m_Var;
			pView->origin += vUp * Vars::Visuals::ThirdpersonOffsetZ.m_Var;
		}
	}
}

void CVisuals::FOV(CViewSetup *pView)
{
	CBaseEntity *pLocal = g_EntityCache.m_pLocal;

	if (pLocal && pView)
	{
		if (pLocal->GetObserverMode() == OBS_MODE_FIRSTPERSON || (pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.m_Var))
			return;

		pView->fov = Vars::Visuals::FieldOfView.m_Var;
		
		if (pLocal->IsAlive())
			pLocal->SetFov(Vars::Visuals::FieldOfView.m_Var);
	}
}

/*
void CVisuals::Fullbright() {
	static ConVar* fullbright = g_Interfaces.CVars->FindVar(_("mat_fullbright"));
	fullbright->SetValue(Vars::Visuals::Fullbright.m_Var); // Probably isn't the best idea to set it like 500x a second :woozy_face:
}
*/

void CVisuals::ARatio() {
	float ratio = (Vars::Visuals::AspectRatioValue.m_Var * 0.1) / 2;
	static ConVar* RatioVar = g_Interfaces.CVars->FindVar(_("r_aspectratio"));
	if (ratio > 0.001)
		RatioVar->SetValue(ratio);
	else
		RatioVar->SetValue((0,1.f) / 2);

}

void CVisuals::ViewmodelXYZ() {
	static char buff[12];
	snprintf(buff, sizeof(buff), _("%d %d %d"), (int)Vars::Visuals::ViewModelX.m_Var, (int)Vars::Visuals::ViewModelY.m_Var, (int)Vars::Visuals::ViewModelZ.m_Var);

	static auto customview = g_Interfaces.CVars->FindVar(_("tf_viewmodels_offset_override"));
	customview->m_fnChangeCallback = 0;
	if (!(Vars::Visuals::ViewModelX.m_Var == 0 && Vars::Visuals::ViewModelY.m_Var == 0 && Vars::Visuals::ViewModelZ.m_Var == 0)) {
		customview->SetValue(buff);
	}
}

void CVisuals::ThirdPerson()
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (Vars::Visuals::ThirdPersonKey.m_Var)
		{
			if (!g_Interfaces.EngineVGui->IsGameUIVisible() && !g_Interfaces.Surface->IsCursorVisible())
			{
				static float flPressedTime = g_Interfaces.Engine->Time();
				float flElapsed = g_Interfaces.Engine->Time() - flPressedTime;

				if ((GetAsyncKeyState(Vars::Visuals::ThirdPersonKey.m_Var) & 0x8000) && flElapsed > 0.f) {
					Vars::Visuals::ThirdPerson.m_Var = !Vars::Visuals::ThirdPerson.m_Var;
					flPressedTime = g_Interfaces.Engine->Time();
				}
			}
		}

		bool bIsInThirdPerson = g_Interfaces.Input->CAM_IsThirdPerson();

		if (!Vars::Visuals::ThirdPerson.m_Var || ((!Vars::Visuals::RemoveScope.m_Var || !Vars::Visuals::RemoveZoom.m_Var) && pLocal->IsScoped()))
		{
			if (bIsInThirdPerson)
				pLocal->ForceTauntCam(0);

			return;
		}

		if (!bIsInThirdPerson)
			pLocal->ForceTauntCam(1);

		if (bIsInThirdPerson && Vars::Visuals::ThirdPersonSilentAngles.m_Var)
		{
			g_Interfaces.Prediction->SetLocalViewAngles(g_GlobalInfo.m_vRealViewAngles);
			if (const auto& pAnimState = pLocal->GetAnimState())
			{
				pAnimState->m_flCurrentFeetYaw = g_GlobalInfo.m_vRealViewAngles.y;
			}
		}
	}
}

bool bWorldIsModulated = false;


void ApplyModulation(const Color_t& clr)
{
	for (MaterialHandle_t h = g_Interfaces.MatSystem->First(); h != g_Interfaces.MatSystem->Invalid(); h = g_Interfaces.MatSystem->Next(h))
	{
		if (const auto& pMaterial = g_Interfaces.MatSystem->Get(h))
		{
			//bool bFound2 = false;
			//IMaterialVar* rain = pMaterial->FindVar(_("env_global"), &bFound2);

			if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
				continue;

			std::string_view group(pMaterial->GetTextureGroupName());

			if (group.find(_(TEXTURE_GROUP_WORLD)) != group.npos/* || group.find(_(TEXTURE_GROUP_SKYBOX)) != group.npos*/)
			{
				bool bFound = false;
				IMaterialVar* pVar = pMaterial->FindVar(_("$color2"), &bFound);

				if (bFound && pVar)
					pVar->SetVecValue(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));

				else pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
			}


		}
	}

	bWorldIsModulated = true;
}


void CVisuals::ModulateWorld()
{
	if (!Vars::Visuals::WorldModulation.m_Var)
		return;

	ApplyModulation(Colors::WorldModulation);
}

void CVisuals::UpdateWorldModulation()
{
	if (!Vars::Visuals::WorldModulation.m_Var) {
		RestoreWorldModulation();
		return;
	}

	auto ColorChanged = [&]() -> bool
	{
		static Color_t old = Colors::WorldModulation;
		Color_t cur = Colors::WorldModulation;

		if (cur.r != old.r || cur.g != old.g || cur.b != old.b) {
			old = cur;
			return true;
		}

		return false;
	};

	if (!bWorldIsModulated || ColorChanged())
		ApplyModulation(Colors::WorldModulation);
}

bool bSkyIsModulated = false;

void ApplySkyboxModulation(const Color_t& clr)
{
	for (MaterialHandle_t h = g_Interfaces.MatSystem->First(); h != g_Interfaces.MatSystem->Invalid(); h = g_Interfaces.MatSystem->Next(h))
	{
		const auto& pMaterial = g_Interfaces.MatSystem->Get(h);

		if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
			continue;

		std::string_view group(pMaterial->GetTextureGroupName());

		if (group._Starts_with("SkyBox"))
		{
			pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
		}
	}
	bSkyIsModulated = true;
}

void CVisuals::ModulateSky()
{
	if (!Vars::Visuals::SkyModulation.m_Var)
		return;

	ApplySkyboxModulation(Colors::SkyModulation);
}

void CVisuals::UpdateSkyModulation()
{
	if (!Vars::Visuals::SkyModulation.m_Var) {
		RestoreSkyModulation();
		return;
	}

	auto ColorChanged = [&]() -> bool
	{
		static Color_t old = Colors::SkyModulation;
		Color_t cur = Colors::SkyModulation;

		if (cur.r != old.r || cur.g != old.g || cur.b != old.b) {
			old = cur;
			return true;
		}

		return false;
	};

	if (!bSkyIsModulated || ColorChanged())
		ApplySkyboxModulation(Colors::SkyModulation);
}

void CVisuals::RestoreWorldModulation()
{
	if (!bWorldIsModulated)
		return;

	ApplyModulation({ 255, 255, 255, 255 });
	bWorldIsModulated = false;
}

void CVisuals::RestoreSkyModulation()
{
	if (!bSkyIsModulated)
		return;

	ApplySkyboxModulation({ 255, 255, 255, 255 });
	bSkyIsModulated = false;
}