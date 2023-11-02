#include "DMEChams.h"

#include "../Vars.h"
#include "../../Hooks/ModelRenderHook/ModelRenderHook.h"

bool CDMEChams::ShouldRun()
{
	if (!Vars::Chams::DME::Active.m_Var || g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	return true;
}

void CDMEChams::Init()
{
	//2 Fresnel materials, because creating a copy of the material and setting the $phongfresnelranges didn't seem to work.  LMK if there's a better way to do this lol
//Thanks spook :)
	m_pMatFresnelHDR0 = Utils::CreateMaterial({
	_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"models/player/shared/shared_normal\"\
		\n\t\"$envmap\" \"skybox/sky_dustbowl_01\"\
		\n\t\"$envmapfresnel\" \"1\"\
		\n\t\"$additive\" \"0\"\
		\n\t\"$phong\" \"1\"\
		\n\t\"$phongfresnelranges\" \"[0 1.5 2]\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[0.5 0.5 0]\"\
		\n\t\"$selfillumtint\" \"[0 0 0]\"\
		\n\t\"$envmaptint\" \"[0 1 0]\"\
		\n}\n")
		});

	m_pMatFresnelHDR1 = Utils::CreateMaterial({
		_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"models/player/shared/shared_normal\"\
		\n\t\"$envmap\" \"skybox/sky_dustbowl_01\"\
		\n\t\"$envmapfresnel\" \"1\"\
		\n\t\"$additive\" \"0\"\
		\n\t\"$phong\" \"1\"\
		\n\t\"$phongfresnelranges\" \"[0 0.05 0.1]\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[0.5 0.5 0]\"\
		\n\t\"$selfillumtint\" \"[0 0 0]\"\
		\n\t\"$envmaptint\" \"[0 1 0]\"\
		\n}\n")
		});

	m_pMatShaded = Utils::CreateMaterial({
	_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"vgui/white_additive\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[-0.25 1 1]\"\
		\n}\n")
	});

	m_pMatShiny = Utils::CreateMaterial({
		_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"vgui/white_additive\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[0 0 1.85]\"\
		\n\t\"$selfIllumtint\" \"[1 0.5 0.6]\"\
		\n}\n")
	});

	m_pMatFlat = Utils::CreateMaterial({
		_("\"UnlitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
                \n\t\"$ignorez\" \"1\"\
		\n}\n")
	});

	m_pMatWFShaded = Utils::CreateMaterial({
	_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$wireframe\" \"1\"\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"vgui/white_additive\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[-0.25 1 1]\"\
		\n}\n")
	});


	m_pMatWFShiny = Utils::CreateMaterial({
		_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$wireframe\" \"1\"\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"vgui/white_additive\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[0 0 1.85]\"\
		\n\t\"$selfIllumtint\" \"[1 0.5 0.6]\"\
		\n}\n")
	});

	m_pMatWFFlat = Utils::CreateMaterial({
	_("\"UnlitGeneric\"\
		\n{\
		\n\t\"$wireframe\" \"1\"\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n}\n")
	});
}

bool CDMEChams::Render(const DrawModelState_t &pState, const ModelRenderInfo_t &pInfo, matrix3x4 *pBoneToWorld)
{
	m_bRendering = false;

	if (ShouldRun())
	{
		m_bRendering = true;

		CBaseEntity *pEntity = g_Interfaces.EntityList->GetClientEntity(pInfo.m_nEntIndex);

		if (pEntity && pEntity->GetClassID() == ETFClassID::CTFViewModel)
		{
			bool bMatWasForced = false;

			if (Vars::Chams::DME::Hands.m_Var)
			{
				g_Interfaces.ModelRender->ForcedMaterialOverride([&]() -> IMaterial*
				{
					switch (Vars::Chams::DME::Hands.m_Var) {
						case 1: { bMatWasForced = true; return m_pMatShaded; }
						case 2: { bMatWasForced = true; return m_pMatShiny; }
						case 3: { bMatWasForced = true; return m_pMatFlat; }
						case 4: { bMatWasForced = true; return m_pMatWFShaded; }
						case 5: { bMatWasForced = true; return m_pMatWFShiny; }
						case 6: { bMatWasForced = true; return m_pMatWFFlat; }
						case 7: {
							// Unsure if this is the right way to make this material adapt to the HDR level, but it works :)
							bMatWasForced = true;
							if (g_Interfaces.CVars->FindVar("mat_hdr_level")->GetInt() > 1) {
								m_pMatFresnel = m_pMatFresnelHDR1;
								return m_pMatFresnel;
							}
							else {
								m_pMatFresnel = m_pMatFresnelHDR1;
								return m_pMatFresnel;
							}
						}
						default: return nullptr;
					}
				}());
			}

			//if (bMatWasForced)
				//g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Colors::Hands.r), Color::TOFLOAT(Colors::Hands.g), Color::TOFLOAT(Colors::Hands.b));

			if (bMatWasForced) {
				if (Vars::Chams::DME::Hands.m_Var != 7) {
					g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Colors::Hands.r), Color::TOFLOAT(Colors::Hands.g), Color::TOFLOAT(Colors::Hands.b));
				}
				else {
					g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				}

				bool found = false;
				bool found2 = false;
				bool found3 = false;
				if (Vars::Chams::DME::Hands.m_Var == 7) {
					/*
					$selfillumtint	[0 0 0] base
					$envmaptint		[0 1 0] top
					*/
					IMaterialVar* pVar = m_pMatFresnel->FindVar(_("$selfillumtint"), &found);
					if (found) {
						pVar->SetVecValue(Color::TOFLOAT(Colors::FresnelBase.r), Color::TOFLOAT(Colors::FresnelBase.g), Color::TOFLOAT(Colors::FresnelBase.b));
					}
					IMaterialVar* pVar2 = m_pMatFresnel->FindVar(_("$envmaptint"), &found2);
					if (found2) {
						pVar2->SetVecValue(Color::TOFLOAT(Colors::Hands.r) * 4, Color::TOFLOAT(Colors::Hands.g) * 4, Color::TOFLOAT(Colors::Hands.b) * 4);
					}
					IMaterialVar* pVar3 = m_pMatFresnel->FindVar(_("$additive"), &found3);
					if (found3) {
						if(Vars::Chams::DME::SeeThru.m_Var)
							pVar3->SetIntValue(1);
						else
							pVar3->SetIntValue(0);
					}
				}
			}
			if (Vars::Chams::DME::HandsAlpha.m_Var < 1.0f)
				g_Interfaces.RenderView->SetBlend(Vars::Chams::DME::HandsAlpha.m_Var);

			ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(ModelRenderHook::DrawModelExecute::index)
				(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);

			if (bMatWasForced) {
				g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
				g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
			}

			if (Vars::Chams::DME::HandsAlpha.m_Var < 1.0f)
				g_Interfaces.RenderView->SetBlend(1.0f);

			return true;
		}

		if (!pEntity && pInfo.m_pModel)
		{
			std::string_view szModelName(g_Interfaces.ModelInfo->GetModelName(pInfo.m_pModel));

			if (szModelName.find(_("weapon")) != std::string_view::npos
				&& szModelName.find(_("arrow")) == std::string_view::npos
				&& szModelName.find(_("w_syringe")) == std::string_view::npos
				&& szModelName.find(_("nail")) == std::string_view::npos
				&& szModelName.find(_("shell")) == std::string_view::npos
				&& szModelName.find(_("parachute")) == std::string_view::npos
				&& szModelName.find(_("buffbanner")) == std::string_view::npos
				&& szModelName.find(_("shogun_warbanner")) == std::string_view::npos
				&& szModelName.find(_("targe")) == std::string_view::npos //same as world model, can't filter
				&& szModelName.find(_("shield")) == std::string_view::npos //same as world model, can't filter
				&& szModelName.find(_("repair_claw")) == std::string_view::npos)
			{
				//g_Interfaces.DebugOverlay->AddTextOverlay(pInfo.m_vOrigin, 0.003f, "%hs", szModelName);

				bool bMatWasForced = false;

				if (Vars::Chams::DME::Weapon.m_Var)
				{
					g_Interfaces.ModelRender->ForcedMaterialOverride([&]() -> IMaterial *
					{
						switch (Vars::Chams::DME::Weapon.m_Var) {
							case 1: { bMatWasForced = true; return m_pMatShaded; }
							case 2: { bMatWasForced = true; return m_pMatShiny; }
							case 3: { bMatWasForced = true; return m_pMatFlat; }
							case 4: { bMatWasForced = true; return m_pMatWFShaded; }
							case 5: { bMatWasForced = true; return m_pMatWFShiny; }
							case 6: { bMatWasForced = true; return m_pMatWFFlat; }
							case 7: {
								// Unsure if this is the right way to make this material adapt to the HDR level, but it works :)
								bMatWasForced = true;
								if (g_Interfaces.CVars->FindVar("mat_hdr_level")->GetInt() > 1) {
									m_pMatFresnel = m_pMatFresnelHDR1;
									return m_pMatFresnel;
								}
								else {
									m_pMatFresnel = m_pMatFresnelHDR1;
									return m_pMatFresnel;
								}
							}
							default: return nullptr;
						}
					}());
				}

				if (bMatWasForced) {
					//g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Colors::Weapon.r), Color::TOFLOAT(Colors::Weapon.g), Color::TOFLOAT(Colors::Weapon.b));
					if (Vars::Chams::DME::Weapon.m_Var != 7) {
						g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Colors::Weapon.r), Color::TOFLOAT(Colors::Weapon.g), Color::TOFLOAT(Colors::Weapon.b));
					}
					else {
						g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
					}


					bool found = false;
					bool found2 = false;
					if (Vars::Chams::DME::Weapon.m_Var == 7) {
						/*
						$selfillumtint	[0 0 0] base
						$envmaptint		[0 1 0] top
						*/
						IMaterialVar* pVar = m_pMatFresnel->FindVar(_("$selfillumtint"), &found);
						if (found) {
							pVar->SetVecValue(Color::TOFLOAT(Colors::FresnelBase.r), Color::TOFLOAT(Colors::FresnelBase.g), Color::TOFLOAT(Colors::FresnelBase.b));
						}
						IMaterialVar* pVar2 = m_pMatFresnel->FindVar(_("$envmaptint"), &found2);
						if (found2) {
							pVar2->SetVecValue(Color::TOFLOAT(Colors::Weapon.r) * 4, Color::TOFLOAT(Colors::Weapon.g) * 4, Color::TOFLOAT(Colors::Weapon.b) * 4);
						}
					}
				}

				if (Vars::Chams::DME::WeaponAlpha.m_Var < 1.0f)
					g_Interfaces.RenderView->SetBlend(Vars::Chams::DME::WeaponAlpha.m_Var);

				ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(ModelRenderHook::DrawModelExecute::index)
					(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);

				if (bMatWasForced) {
					g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
					g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				}

				if (Vars::Chams::DME::WeaponAlpha.m_Var < 1.0f)
					g_Interfaces.RenderView->SetBlend(1.0f);

				return true;
			}
		}

		m_bRendering = false;
	}

	return false;
}
