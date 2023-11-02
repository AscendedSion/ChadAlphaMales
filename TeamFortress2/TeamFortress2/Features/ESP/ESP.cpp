#include "ESP.h"
#include "../Vars.h"
#include "../ChatInfo/ChatInfo.h"
#include "../Visuals/Visuals.h"
#include "../Backtrack/Backtrack.h"
#include "../Chams/Chams.h"
#include "../Cache/Cache.h"
bool CESP::ShouldRun()
{
	if (g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	return true;
}

void CESP::AAIndicator() {
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		// TODO: color pickers
		Color_t real_color = { 255, 0, 0, 255 };
		Color_t fake_color = { 0, 0, 255, 255 };
		Color_t lby_color = { 0, 255, 0, 255 };	// lby in tf2!?!? NO WAY

		// TODO: slider
		constexpr auto distance = 50.f;

		const auto& origin = pLocal->GetAbsOrigin();

		Vec3 screen1, screen2;
		if (Utils::W2S(origin, screen1))
		{
			if (Utils::W2S(Math::GetRotatedPosition(origin, g_GlobalInfo.m_vRealViewAngles.y, distance), screen2))
				g_Draw.Line(screen1.x, screen1.y, screen2.x, screen2.y, real_color);

			if (Utils::W2S(Math::GetRotatedPosition(origin, g_GlobalInfo.m_vFakeViewAngles.y, distance), screen2))
				g_Draw.Line(screen1.x, screen1.y, screen2.x, screen2.y, fake_color);
		}
	}
}

void CESP::Run()
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (ShouldRun())
		{
			if(Vars::AntiHack::AntiAim::Active.m_Var)
				AAIndicator();

			DrawWorld();
			DrawBuildings(pLocal);
			DrawPlayers(pLocal);
		}
	}
}

bool CESP::GetDrawBounds(CBaseEntity *pEntity, Vec3* vTrans, int &x, int &y, int &w, int &h)
{
	int n;
	bool bIsPlayer = false;
	Vec3 vMins, vMaxs;

	if (pEntity->IsPlayer())
	{
		bIsPlayer = true;
		bool bIsDucking = pEntity->IsDucking();
		vMins = g_Interfaces.GameMovement->GetPlayerMins(bIsDucking);
		vMaxs = g_Interfaces.GameMovement->GetPlayerMaxs(bIsDucking);
	}

	else
	{
		vMins = pEntity->GetCollideableMins();
		vMaxs = pEntity->GetCollideableMaxs();
	}

	const matrix3x4& transform = pEntity->GetRgflCoordinateFrame();

	Vec3 vPoints[] = 
	{
		Vec3(vMins.x, vMins.y, vMins.z),
		Vec3(vMins.x, vMaxs.y, vMins.z),
		Vec3(vMaxs.x, vMaxs.y, vMins.z),
		Vec3(vMaxs.x, vMins.y, vMins.z),
		Vec3(vMaxs.x, vMaxs.y, vMaxs.z),
		Vec3(vMins.x, vMaxs.y, vMaxs.z),
		Vec3(vMins.x, vMins.y, vMaxs.z),
		Vec3(vMaxs.x, vMins.y, vMaxs.z)
	};

	for (n = 0; n < 8; n++)
		Math::VectorTransform(vPoints[n], transform, vTrans[n]);

	Vec3 flb, brt, blb, frt, frb, brb, blt, flt;

	if (Utils::W2S(vTrans[3], flb) && Utils::W2S(vTrans[5], brt)
		&& Utils::W2S(vTrans[0], blb) && Utils::W2S(vTrans[4], frt)
		&& Utils::W2S(vTrans[2], frb) && Utils::W2S(vTrans[1], brb)
		&& Utils::W2S(vTrans[6], blt) && Utils::W2S(vTrans[7], flt)
		&& Utils::W2S(vTrans[6], blt) && Utils::W2S(vTrans[7], flt))
	{
		Vec3 arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

		float left = flb.x;
		float top = flb.y;
		float righ = flb.x;
		float bottom = flb.y;

		for (int n = 1; n < 8; n++)
		{
			if (left > arr[n].x)
				left = arr[n].x;

			if (top < arr[n].y)
				top = arr[n].y;

			if (righ < arr[n].x)
				righ = arr[n].x;

			if (bottom > arr[n].y)
				bottom = arr[n].y;
		}

		float x_ = left;
		float y_ = bottom;
		float w_ = (righ - left);
		float h_ = (top - bottom);

		if (bIsPlayer && Vars::ESP::Players::Box.m_Var != 3)
		{
			x_ += ((righ - left) / 8.0f);
			w_ -= (((righ - left) / 8.0f) * 2.0f);
		}

		x = static_cast<int>(x_);
		y = static_cast<int>(y_);
		w = static_cast<int>(w_);
		h = static_cast<int>(h_);

		return !(x > g_ScreenSize.w || (x + w) < 0 || y > g_ScreenSize.h || (y + h) < 0);
	}

	return false;
}

Vec3 predPosAt(float flTime, CBaseEntity* target)
{
	/*if (!flTime)
		return target->GetAbsOrigin();*/

	static ConVar* sv_gravity = g_Interfaces.CVars->FindVar("sv_gravity");

	Vector startPos = target->GetAbsOrigin(), velocity = target->GetVelocity();
	float zdrop;
	if (target->GetFlags() & FL_ONGROUND) {
		zdrop = velocity.z * flTime;
	}
	else {
		zdrop = 0.5 * -sv_gravity->GetInt() * pow(flTime, 2) + velocity.z * flTime;
	}

	Vector result(
		startPos.x + (velocity.x * flTime),
		startPos.y + (velocity.y * flTime),
		startPos.z + zdrop);

	float endZ = result.z;

	float angleY = 0;
	PlayerCache* cache = g_Cache.FindPlayer(target);
	if (cache && cache->Full()) // Determine player strafe by averaging cache data
	{
		// A = Last, B = Mid, C = Current (startPos)
		int last = (MAX_CACHE - 1) / 3, mid = (MAX_CACHE - 1) / 6;
		Vector vLast, vMid;
		if (HitboxData* data = cache->FindTick(g_Interfaces.GlobalVars->tickcount - last))
		{
			vLast = data->vCenter;
			if (data = cache->FindTick(g_Interfaces.GlobalVars->tickcount - mid))
				vMid = data->vCenter;
		}
		// Approximate angle
		Vector forward, strafe;
		Math::VectorAngles(vMid - vLast, forward);
		Math::VectorAngles(startPos - vMid, strafe);
		// Divide our angle to measure by distance
		angleY = (strafe.y - forward.y) / ((vMid - vLast).Lenght2D() + (startPos - vMid).Lenght2D());
	}
	Math::RotateVec2(*(Vec2*)&result, *(Vec2*)&startPos, DEG2RAD(angleY * (result - startPos).Lenght2D()));

	result.z = endZ;

	return result;
}

bool AssumeVis(CBaseEntity* you, Vector vStart, Vector vEnd, CGameTrace* result)
{
	CGameTrace Trace;
	Ray_t Ray;
	CTraceFilterHitscan Filter;
	Filter.pSkip = you;

	Ray.Init(*(Vector*)&vStart, *(Vector*)&vEnd);
	g_Interfaces.EngineTrace->TraceRay(Ray, MASK_SHOT, &Filter, &Trace);

	if (result)
		*result = Trace;

	return !Trace.DidHit();
}

class SColor
{
public:
	byte rgba[4];

	inline byte& operator[](int i)
	{
		return rgba[i];
	}
	inline bool operator==(SColor& other)
	{
		return *(size_t*)&rgba == *(size_t*)&other.rgba;
	}
	inline bool operator!=(SColor& other)
	{
		return *(size_t*)&rgba != *(size_t*)&other.rgba;
	}

	SColor(byte red, byte green, byte blue, byte alpha = 255)
	{
		rgba[0] = red, rgba[1] = green, rgba[2] = blue, rgba[3] = alpha;
	}
	SColor(byte bright, byte alpha = 255)
	{
		rgba[0] = bright, rgba[1] = bright, rgba[2] = bright, rgba[3] = alpha;
	}
	SColor() {}
};
void ClampFl(float& fl)
{
	if (fl > 1)
		fl = 1;
	else if (fl < 0)
		fl = 0;
}
SColor hsv2rgb(float hue, float saturation, float brightness, int alpha = 255) {
	while (hue >= 1)
		hue -= 1;
	while (hue <= 0)
		hue += 1;

	ClampFl(saturation);
	ClampFl(brightness);

	float h = hue == 1.0f ? 0 : hue * 6.0f;
	float f = h - (int)h;
	float p = brightness * (1.0f - saturation);
	float q = brightness * (1.0f - saturation * f);
	float t = brightness * (1.0f - (saturation * (1.0f - f)));

	if (h < 1)
	{
		return SColor(
			(unsigned char)(brightness * 255),
			(unsigned char)(t * 255),
			(unsigned char)(p * 255),
			alpha
		);
	}
	else if (h < 2)
	{
		return SColor(
			(unsigned char)(q * 255),
			(unsigned char)(brightness * 255),
			(unsigned char)(p * 255),
			alpha
		);
	}
	else if (h < 3)
	{
		return SColor(
			(unsigned char)(p * 255),
			(unsigned char)(brightness * 255),
			(unsigned char)(t * 255),
			alpha
		);
	}
	else if (h < 4)
	{
		return SColor(
			(unsigned char)(p * 255),
			(unsigned char)(q * 255),
			(unsigned char)(brightness * 255),
			alpha
		);
	}
	else if (h < 5)
	{
		return SColor(
			(unsigned char)(t * 255),
			(unsigned char)(p * 255),
			(unsigned char)(brightness * 255),
			alpha
		);
	}
	else
	{
		return SColor(
			(unsigned char)(brightness * 255),
			(unsigned char)(p * 255),
			(unsigned char)(q * 255),
			alpha
		);
	}
}

void CESP::DrawPaths()
{
	/*for (auto& Player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!(Player->GetIndex() == g_GlobalInfo.m_nCurrentTargetIdx)) {
			return;
		}*/

	if (!Vars::Aimbot::Projectile::R8Method.m_Var)
		return;

	if (g_GlobalInfo.m_WeaponType != EWeaponType::PROJECTILE)
		return;

	if (g_GlobalInfo.m_nCurrentTargetIdx) {
		CBaseEntity* aimTarget = g_Interfaces.EntityList->GetClientEntity(g_GlobalInfo.m_nCurrentTargetIdx);
		if (!aimTarget) {
			return;
		}
		Vector velocity = aimTarget->GetVelocity() / 10, predPos[101];
		int flags = aimTarget->GetFlags();

		int max = (flags & FL_ONGROUND) ? 21 : 101;
		for (int i = 0; i < max; i++)
			predPos[i] = predPosAt(i * 0.05, aimTarget);

		SColor lineColor;
		for (int i = 0; i < max - 1; i++)
		{
			CGameTrace result; // Check to see if we're about to hit a surface
			if (!AssumeVis(aimTarget, predPos[i], predPos[i + 1], &result))
			{
				// Draw a circle to show our landing position/angle
				if (!(flags & FL_ONGROUND))
				{
					const Vector relSquare[] = { Vector(30, 30, 0), Vector(30, -30, 0), Vector(-30, -30, 0), Vector(-30, 30, 0) };
					Vector scrPos[4];
					bool visible = true;
					for (size_t j = 0; j < sizeof(relSquare) / sizeof(Vector); j++)
					{
						if (!Utils::W2S(relSquare[j] + result.vEndPos, scrPos[j]))
						{
							visible = false;
							break;
						}
					}
					if (visible)
					{
						Vector start, end;
						if (Utils::W2S(result.vEndPos, start) && Utils::W2S(result.vEndPos + result.Plane.normal * 30, end)) {
							SColor color(255);
							g_Draw.Line(start.x, start.y, end.x, end.y, { color.rgba[0], color.rgba[1], color.rgba[2], color.rgba[3] });
						}
						for (size_t j = 0; j < sizeof(scrPos) / sizeof(Vector); j++)
						{
							int last = j - 1;
							if (j == 0)
								last = 3;
							SColor color(255);
							g_Draw.Line(scrPos[j].x, scrPos[j].y, scrPos[last].x, scrPos[last].y, { color.rgba[0], color.rgba[1], color.rgba[2], color.rgba[3] });
						}
					}
				}
				break;
			}

			if (i < 21) // Make a gradiant from green to red
				lineColor = hsv2rgb((100.f - (i * 5)) / 255, 1, 1, 255);

			Vector screenPos[3];
			if (!Utils::W2S(predPos[i], screenPos[0]))
				continue;
			if (!Utils::W2S(predPos[i + 1], screenPos[1]))
				continue;

			g_Draw.Line(screenPos[0].x, screenPos[0].y, screenPos[1].x, screenPos[1].y, { lineColor.rgba[0], lineColor.rgba[1], lineColor.rgba[2], lineColor.rgba[3] });

			Vec2 extraLine = Vec2(predPos[i + 1].x, predPos[i + 1].y);
			extraLine += Math::VectorAngles(Math::ToAngle(Vec2(velocity.x, velocity.y)), 10);
			extraLine = Math::RotateVec2(extraLine, Vec2(predPos[i + 1].x, predPos[i + 1].y), DEG2RAD(90));
			if (!Utils::W2S(Vec3(extraLine.x, extraLine.y, predPos[i + 1].z), screenPos[2]))
				continue;

			g_Draw.Line(screenPos[1].x, screenPos[1].y, screenPos[2].x, screenPos[2].y, { lineColor.rgba[0], lineColor.rgba[1], lineColor.rgba[2], lineColor.rgba[3] });
		}
	}
}

void CESP::HandleDormancy(CBaseEntity* pEntity, int index)
{
	if (pEntity->GetDormant() && m_flEntityAlpha[index] > 0)
	{
		if (!m_bTimeSet[index])
		{
			m_flTimeSinceDormant[index] = g_Interfaces.GlobalVars->curtime;
			m_bTimeSet[index] = true;
		}

		float flFade = 5;
		if (flFade <= 0.0f)
			flFade = 0.01f;

		m_flEntityAlpha[index] = 150.0f - ((g_Interfaces.GlobalVars->curtime - m_flTimeSinceDormant[index]) * (150.0f / flFade));
		if (m_flEntityAlpha[index] < 0.0f)
			m_flEntityAlpha[index] = 0.0f;
	}

	if (!pEntity->GetDormant())
	{
		m_flEntityAlpha[index] = 255.0f;
		m_flTimeSinceDormant[index] = 0.0f;
		m_bTimeSet[index] = false;
	}
}

void CESP::DrawPlayers(CBaseEntity *pLocal)
{
	if (!Vars::ESP::Players::Active.m_Var)
		return;

	for (const auto &Player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
	{
		if (!Player->IsAlive() || Player->IsAGhost())
			continue;

		int nIndex = Player->GetIndex();
		bool bIsLocal = nIndex == g_Interfaces.Engine->GetLocalPlayer();
		
		DrawPaths();

		HandleDormancy(Player, nIndex);
		if (m_flEntityAlpha[nIndex] <= 0.0f)
			continue;

		if (!bIsLocal)
		{
			switch (Vars::ESP::Players::IgnoreCloaked.m_Var)
			{
				case 0: { break; }
				case 1: { if (Player->IsCloaked()) { continue; } break; }
				case 2: { if (Player->IsCloaked() && Player->GetTeamNum() != pLocal->GetTeamNum()) { continue; } break; }
			}

			switch (Vars::ESP::Players::IgnoreTeammates.m_Var)
			{
				case 0: break;
				case 1: { if (Player->GetTeamNum() == pLocal->GetTeamNum()) { continue; } break; }
				case 2: { if (Player->GetTeamNum() == pLocal->GetTeamNum() && !g_EntityCache.Friends[nIndex]) { continue; } break; }
			}
		}

		else
		{
			if (!Vars::ESP::Players::ShowLocal.m_Var)
				continue;
		}

		Color_t DrawColor = Utils::GetEntityDrawColor(Player);

		PlayerInfo_t pi{};

		if (g_Interfaces.Engine->GetPlayerInfo(Player->GetIndex(), &pi))
		{
			if (g_Playerlist.IsIgnored(pi.friendsID)) {
				DrawColor = Colors::IgnoredTarget;
			}
		}

		if (Vars::ESP::Players::Dlights.m_Var)
			CreateDLight(nIndex, DrawColor, Player->GetAbsOrigin(), Vars::ESP::Players::DlightRadius.m_Var);

		int x = 0, y = 0, w = 0, h = 0; Vec3 vTrans[8];
		if (GetDrawBounds(Player, vTrans, x, y, w, h))
		{
			int nHealth = Player->GetHealth(), nMaxHealth = Player->GetMaxHealth();

			size_t FONT = (Vars::ESP::Main::Outline.m_Var ? FONT_ESP_OUTLINED : FONT_ESP),
				   FONT_NAME = (Vars::ESP::Main::Outline.m_Var ? FONT_ESP_NAME_OUTLINED : FONT_ESP_NAME);

			int nTextX = ((x + w) + 3), nTextOffset = 0, nClassNum = Player->GetClassNum();

			if (Vars::ESP::Players::Uber.m_Var == 2 && nClassNum == ETFClass::CLASS_MEDIC) {
				if (const auto &pMedGun = Player->GetWeaponFromSlot(EWeaponSlots::SLOT_SECONDARY))
					nTextX += 5;
			}

			g_Interfaces.Surface->DrawSetAlphaMultiplier(Vars::ESP::Players::Alpha.m_Var);

			if (Vars::ESP::Players::Bones.m_Var)
			{
				const Color_t clrBone = Colors::Bones;

				DrawBones(Player, { 8, 7, 6, 4 },	clrBone);
				DrawBones(Player, { 11, 10, 9, 4 }, clrBone);
				DrawBones(Player, { 0, 4, 1 },		clrBone);
				DrawBones(Player, { 14, 13, 1 },	clrBone);
				DrawBones(Player, { 17, 16, 1 },	clrBone);
			}

			switch (Vars::ESP::Players::Box.m_Var) {
				case 1: {
					int height = h + 1; //don't ask me /shrug
					Color_t DrawColor = Utils::GetEntityDrawColor(Player);;

					g_Draw.OutlinedRect(x, y, w, height, DrawColor);
					if (Vars::ESP::Main::Outline.m_Var == 2)
					{
						g_Draw.OutlinedRect(x - 1, y - 1, w + 2, height + 2, Colors::OutlineESP);
						g_Draw.OutlinedRect(x + 1, y + 1, w - 2, height - 2, Colors::OutlineESP);
					}

					break;
				}
				case 2: {
					g_Draw.CornerRect(x, y, w, h, 3, 5, DrawColor);
					if (Vars::ESP::Main::Outline.m_Var == 2)
					{
						g_Draw.CornerRect(x - 1, y - 1, w + 2, h + 2, 3, 5, Colors::OutlineESP);
					}

					break;
				}
				case 3: {
					Draw3DBox(vTrans, DrawColor);
					break;
				}
				default:
					break;
			}

			if (Vars::ESP::Players::Lines.m_Var)
			{
				Vec3 vScreen, vOrigin = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);

				if (g_Interfaces.Input->CAM_IsThirdPerson())
					Utils::W2S(pLocal->GetAbsOrigin(), vOrigin);

				if (Utils::W2S(Player->GetAbsOrigin(), vScreen))
					g_Draw.Line(vOrigin.x, vOrigin.y, vScreen.x, vScreen.y, DrawColor);
			}

			if (Vars::ESP::Players::Uber.m_Var && nClassNum == ETFClass::CLASS_MEDIC)
			{
				if (const auto& pMedGun = Player->GetWeaponFromSlot(EWeaponSlots::SLOT_SECONDARY))
				{
					if (Vars::ESP::Players::Uber.m_Var == 1)
					{
						g_Draw.String(FONT, nTextX, (y + nTextOffset), Colors::UberColor, ALIGN_DEFAULT, L"%.0f%%", (pMedGun->GetUberCharge() * 100.0f));
						nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
					}

					if (Vars::ESP::Players::Uber.m_Var == 2 && pMedGun->GetUberCharge())
					{
						x += w + 1;

						float flUber = pMedGun->GetUberCharge() * (pMedGun->GetItemDefIndex() == Medic_s_TheVaccinator ? 400.0f : 100.0f);
						float flMaxUber = 100.0f;

						if (flUber > flMaxUber)
							flUber = flMaxUber;

						static const int nWidth = 2;
						int nHeight = (h + (flUber < flMaxUber ? 2 : 1));
						int nHeight2 = (h + 1);

						float ratio = (flUber / flMaxUber);
						//g_Draw.Rect((x + nWidth), (y + nHeight - (nHeight * ratio)), nWidth, (nHeight * ratio), Colors::UberColor);
						if (Vars::ESP::Main::Outline.m_Var == 2)
							g_Draw.OutlinedRect((x + nWidth) - 1, (y + nHeight - (nHeight * ratio)) - 1, nWidth + 2, (nHeight * ratio) + 2, Colors::OutlineESP);

						x -= w + 1;
					}
				}
			}

			PlayerInfo_t pi;
			if (g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi))
			{

				if (Vars::ESP::Players::Name.m_Var)
				{
					// different method of making color picker name esp
					Color_t NameColor = Vars::ESP::Players::NameCustom.m_Var ? Colors::NameColor : DrawColor;

					int offset = (g_Draw.m_vecFonts[FONT_NAME].nTall + (g_Draw.m_vecFonts[FONT_NAME].nTall / 4));
					g_Draw.String(FONT_NAME, (x + (w / 2)), (y - offset), NameColor, ALIGN_CENTERHORIZONTAL, Utils::ConvertUtf8ToWide(pi.name).data());
				}
				if (Vars::ESP::Players::GUID.m_Var)
				{
					g_Draw.String(FONT, nTextX, (y + nTextOffset), Colors::White, ALIGN_DEFAULT, "%s", pi.guid);
					nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
				}
			}

			if (Vars::ESP::Players::Class.m_Var)
			{
				if (Vars::ESP::Players::Class.m_Var == 1 || Vars::ESP::Players::Class.m_Var == 3)
				{
					int offset = (Vars::ESP::Players::Name.m_Var ? g_Draw.m_vecFonts[FONT_NAME].nTall + (g_Draw.m_vecFonts[FONT_NAME].nTall * 0.3f) : 0);
					static const int nSize = 18;
					g_Draw.Texture((x + (w / 2) - (nSize / 2)), ((y - offset) - nSize), nSize, nSize, Colors::White, nClassNum);
				}

				if (Vars::ESP::Players::Class.m_Var >= 2)
				{
					g_Draw.String(FONT, nTextX, (y + nTextOffset), DrawColor, ALIGN_DEFAULT, L"%ls", GetPlayerClass(nClassNum));
					nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
				}
			}

			if (Vars::ESP::Players::Cond.m_Var)
			{
				size_t FONT = (Vars::ESP::Main::Outline.m_Var ? FONT_ESP_COND_OUTLINED : FONT_ESP_COND);
				int offset = g_Draw.m_vecFonts[FONT].nTall / 4;
				std::wstring cond_str = GetPlayerConds(Player);

				if (!cond_str.empty())
				{
					cond_str.erase((cond_str.end() - 1), cond_str.end());
					g_Draw.String(FONT, nTextX, (y + nTextOffset), Colors::Cond, ALIGN_DEFAULT, cond_str.data());
				}
			}

			if (Vars::ESP::Players::Healthbar::Enabled.m_Var)
			{
				x -= 1;

				float flHealth = static_cast<float>(nHealth);
				float flMaxHealth = static_cast<float>(nMaxHealth);

				Color_t HealthColor = Utils::GetHealthColor(nHealth, nMaxHealth);

				Color_t clr = flHealth > flMaxHealth ? Colors::Overheal : HealthColor;

				if (!Player->IsVulnerable())
					Colors::Invuln;

				if (flHealth > flMaxHealth)
					flHealth = flMaxHealth;

				static const int nWidth = 2;
				int nHeight = (h + (flHealth < flMaxHealth ? 2 : 1));
				int nHeight2 = (h + 1);

				float ratio = (flHealth / flMaxHealth);
				g_Draw.Rect(((x - nWidth) - 2), y, nWidth, nHeight2, { 0,0,0,150 });
				g_Draw.Rect(((x - nWidth) - 2), (y + nHeight - (nHeight * ratio)), nWidth, (nHeight* ratio), clr);
				
				if (flHealth < flMaxHealth) {
					g_Draw.String(FONT_ESP_COND_OUTLINED, (x - 10), (y + nHeight - (nHeight * ratio)), { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"%d", nHealth);
				}
				g_Draw.OutlinedRect(((x - nWidth) - 3), (y - 1), (nWidth + 2), (nHeight2 + 2), { 0,0,0,150 });
				g_Draw.OutlinedRect(((x - nWidth) - 2) - 1, (y + nHeight - (nHeight * ratio)) - 1, nWidth + 2, (nHeight* ratio) + 2, { 0,0,0,255 });

				x += 1;
			}

			// Backtrack ESP, for future use
			
			
			g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
		}
	}
}

void CESP::DrawBuildings(CBaseEntity *pLocal)
{
	if (!Vars::ESP::Buildings::Active.m_Var)
		return;

	const auto& Buildings = g_EntityCache.GetGroup(Vars::ESP::Buildings::IgnoreTeammates.m_Var ? EGroupType::BUILDINGS_ENEMIES : EGroupType::BUILDINGS_ALL);

	for (const auto &pBuilding : Buildings)
	{
		if (!pBuilding->IsAlive())
			continue;

		const auto& Building = reinterpret_cast<CBaseObject*>(pBuilding);

		Color_t DrawColor = Utils::GetEntityDrawColor(Building);

		if (Vars::ESP::Buildings::Dlights.m_Var)
			CreateDLight(Building->GetIndex(), DrawColor, Building->GetAbsOrigin(), Vars::ESP::Buildings::DlightRadius.m_Var);

		int x = 0, y = 0, w = 0, h = 0; Vec3 vTrans[8];
		if (GetDrawBounds(Building, vTrans, x, y, w, h))
		{
			auto nHealth = Building->GetHealth(), nMaxHealth = Building->GetMaxHealth(),
				 nTextX = ((x + w) + 3), nTextOffset = 0, nTextTopOffset = 0;

			Color_t HealthColor = Utils::GetHealthColor(nHealth, nMaxHealth);

			auto nType = EBuildingType(Building->GetType());

			size_t FONT      = (Vars::ESP::Main::Outline.m_Var ? FONT_ESP_OUTLINED : FONT_ESP),
				   FONT_NAME = (Vars::ESP::Main::Outline.m_Var ? FONT_ESP_NAME_OUTLINED : FONT_ESP_NAME),
				   FONT_COND = (Vars::ESP::Main::Outline.m_Var ? FONT_ESP_COND_OUTLINED : FONT_ESP_COND);

			bool bIsMini = Building->GetMiniBuilding();

			g_Interfaces.Surface->DrawSetAlphaMultiplier(Vars::ESP::Buildings::Alpha.m_Var);

			switch (Vars::ESP::Buildings::Box.m_Var) {
				case 1: {
					h += 1;

					g_Draw.OutlinedRect(x, y, w, h, DrawColor);

					if (Vars::ESP::Main::Outline.m_Var == 2)
						g_Draw.OutlinedRect((x - 1), (y - 1), (w + 2), (h + 2), Colors::OutlineESP);

					h -= 1;
					break;
				}
				case 2: {
					g_Draw.CornerRect(x, y, w, h, 3, 5, DrawColor);

					if (Vars::ESP::Main::Outline.m_Var == 2)
						g_Draw.CornerRect((x - 1), (y - 1), (w + 2), (h + 2), 3, 5, Colors::OutlineESP);
					break;
				}
				case 3: {
					Draw3DBox(vTrans, DrawColor);
					break;
				}
				default:
					break;
			}

			if (Vars::ESP::Buildings::Lines.m_Var)
			{
				Vec3 vScreen, vOrigin = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);

				if (g_Interfaces.Input->CAM_IsThirdPerson())
					Utils::W2S(pLocal->GetAbsOrigin(), vOrigin);

				if (Utils::W2S(Building->GetAbsOrigin(), vScreen))
					g_Draw.Line(vOrigin.x, vOrigin.y, vScreen.x, vScreen.y, DrawColor);
			}

			if (Vars::ESP::Buildings::Name.m_Var)
			{
				const wchar_t* szName;

				switch (nType)
				{
					case EBuildingType::SENTRY:
					{
						if (bIsMini)
						{
							szName = _(L"Mini Sentry");
							break;
						}

						szName = _(L"Sentry");
						break;
					}
					case EBuildingType::DISPENSER:
					{
						szName = _(L"Dispenser");
						break;
					}
					case EBuildingType::TELEPORTER:
					{
						szName = _(L"Teleporter");
						break;
					}
					default:
					{
						szName = _(L"Unknown");
						break;
					}
				}
				// the names are pixelated for buildings, idk why

				Color_t BNameColor = Vars::ESP::Buildings::NameCustom.m_Var ? Colors::BNameColor : DrawColor;

				nTextTopOffset += (g_Draw.m_vecFonts[FONT_NAME].nTall + (g_Draw.m_vecFonts[FONT_NAME].nTall / 4));
				g_Draw.String(FONT, (x + (w / 2)), (y - nTextTopOffset), BNameColor, ALIGN_CENTERHORIZONTAL, szName);
			}

			if (Vars::ESP::Buildings::Owner.m_Var && !Building->GetMapPlaced())
			{
				if (const auto& pOwner = Building->GetOwner())
				{
					PlayerInfo_t pi;
					if (g_Interfaces.Engine->GetPlayerInfo(pOwner->GetIndex(), &pi))
					{
						nTextTopOffset += (g_Draw.m_vecFonts[FONT_NAME].nTall + (g_Draw.m_vecFonts[FONT_NAME].nTall / 4));
						g_Draw.String(FONT_NAME, (x + (w / 2)), (y - nTextTopOffset), DrawColor, ALIGN_CENTERHORIZONTAL, _(L"Built by: %ls"), Utils::ConvertUtf8ToWide(pi.name).data());
					}
				}
			}

			float flConstructed = Building->GetConstructed() * 100.0f;
			if (flConstructed < 100.0f && static_cast<int>(flConstructed) != 0)
			{
				g_Draw.String(FONT, nTextX, (y + nTextOffset), DrawColor, ALIGN_DEFAULT, _(L"Building: %0.f%%"), flConstructed);
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			if (Vars::ESP::Buildings::Health.m_Var)
			{
				g_Draw.String(FONT, nTextX, (y + nTextOffset), DrawColor, ALIGN_DEFAULT, L"%d HP", nHealth);
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			if (Vars::ESP::Buildings::Level.m_Var && !bIsMini)
			{
				g_Draw.String(FONT, nTextX, (y + nTextOffset), DrawColor, ALIGN_DEFAULT, L"%d/3", Building->GetLevel());
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			if (Vars::ESP::Buildings::Cond.m_Var)
			{
				int offset = g_Draw.m_vecFonts[FONT_COND].nTall / 4;

				std::wstring cond_str = L"";

				if (nType == EBuildingType::SENTRY && Building->GetControlled())
					cond_str += _(L"CONTROLLED "); //dispensers are also apparently controlled sometimes
				//They are controlled when player is in their range, maybe add cond for them "ACTIVE" or something
				//indicating there is a player nearby.

				if (Building->GetDisabled()) //Building->IsSpook()
					cond_str += _(L"DISABLED ");

				if (!cond_str.empty())
				{
					cond_str.erase((cond_str.end() - 1), cond_str.end());
					g_Draw.String(FONT_COND, (x + (w / 2)), (y + h) + offset, Colors::CondBuildings, ALIGN_CENTERHORIZONTAL, cond_str.data());
				}
			}

			if (Vars::ESP::Buildings::HealthBar.m_Var)
			{
				x -= 1;

				float flHealth = static_cast<float>(nHealth);
				float flMaxHealth = static_cast<float>(nMaxHealth);

				if (flHealth > flMaxHealth)
					flHealth = flMaxHealth;

				static const int nWidth = 2;
				int nHeight = (h + (flHealth < flMaxHealth ? 2 : 1));
				int nHeight2 = (h + 1);

				float ratio = (flHealth / flMaxHealth);
				
				g_Draw.Rect(((x - nWidth) - 2), (y + nHeight - (nHeight * ratio)), nWidth, (nHeight* ratio), HealthColor);
				if (Vars::ESP::Main::Outline.m_Var == 2)
					g_Draw.OutlinedRect(((x - nWidth) - 2) - 1, (y + nHeight - (nHeight * ratio)) - 1, nWidth + 2, (nHeight * ratio) + 2, Colors::OutlineESP);

				x += 1;
			}
			g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
		}
	}
}

void CESP::DrawWorld()
{
	if (!Vars::ESP::World::Active.m_Var)
		return;
	
	Vec3 vScreen = {};
	size_t FONT = (Vars::ESP::Main::Outline.m_Var ? FONT_ESP_PICKUPS_OUTLINED : FONT_ESP_PICKUPS);

	g_Interfaces.Surface->DrawSetAlphaMultiplier(Vars::ESP::World::Alpha.m_Var);

	if (Vars::ESP::World::HealthText.m_Var) 
	{
		for (const auto& Health : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH)) 
		{
			if (Utils::W2S(Health->GetWorldSpaceCenter(), vScreen))
				g_Draw.String(FONT, vScreen.x, vScreen.y, Colors::Health, ALIGN_CENTER, _(L"HEALTH"));
		}
	}

	if (Vars::ESP::World::AmmoText.m_Var)
	{
		for (const auto& Ammo : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO))
		{
			if (Utils::W2S(Ammo->GetWorldSpaceCenter(), vScreen))
				g_Draw.String(FONT, vScreen.x, vScreen.y, Colors::Ammo, ALIGN_CENTER, _(L"AMMO"));
		}
	}

	g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
}

std::wstring CESP::GetPlayerConds(CBaseEntity* pEntity)
{
	std::wstring szCond = L"";
	int nCond = pEntity->GetCond(), nCondEx = pEntity->GetCondEx(), nCondEx2 = pEntity->GetCondEx2();

	if (nCond & TFCond_Slowed)
	{
		if (const auto& pWeapon = pEntity->GetActiveWeapon())
		{
			if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN)
				szCond += _(L"REV ");
		}
	}

	if (nCondEx2 & TFCondEx2_BlastImmune)
		szCond += _(L"BLAST IMMUNE ");

	if (nCondEx2 & TFCondEx2_BulletImmune)
		szCond += _(L"BULLET IMMUNE");

	if (nCondEx2 & TFCondEx2_FireImmune)
		szCond += _(L"FIRE IMMUNE");

	if ((nCond & TFCond_Ubercharged) || (nCond & TFCondEx_PhlogUber))
		szCond += _(L"UBER ");

	if (nCond & TFCond_Kritzkrieged)
		szCond += _(L"KRITZ ");

	if (nCond & TFCond_MegaHeal|| nCond & TFCond_Overhealed )
		szCond += _(L"HEAL ");

	if (nCond & TFCond_Bonked)
		szCond += _(L"BONK ");

	if (nCond & TFCond_Kritzkrieged || nCond & TFCond_MiniCrits ||
		nCondEx & TFCondEx_CritCanteen || nCondEx & TFCondEx_CritOnFirstBlood || nCondEx & TFCondEx_CritOnWin ||
		nCondEx & TFCondEx_CritOnKill || nCondEx & TFCondEx_CritDemoCharge || nCondEx & TFCondEx_CritOnFlagCapture ||
		nCondEx & TFCondEx_HalloweenCritCandy || nCondEx & TFCondEx_PyroCrits)
		szCond += _(L"CRIT ");

	if (nCond & TFCond_Cloaked)
		szCond += _(L"CLOAK ");

	if (nCond & TFCond_Zoomed)
		szCond += _(L"SCOPE ");

	if (nCond & TFCond_Taunting)
		szCond += _(L"TAUNT ");

	if (nCond & TFCond_Disguised)
		szCond += _(L"DISGUISE ");

	if (nCond & TFCond_Milked)
		szCond += _(L"MILK ");

	if (nCond & TFCond_Jarated)
		szCond += _(L"JARATE ");
	if (nCond & TFCond_OnFire)
		szCond += _(L"FIRE");

	if (nCond & TFCond_Bleeding)
		szCond += _(L"BLEED ");

	return szCond;
}

const wchar_t* CESP::GetPlayerClass(int nClassNum)
{
	static const wchar_t* szClasses[] = { L"unknown", L"scout", L"sniper", L"soldier", L"demoman",
										  L"medic",   L"heavy", L"pyro",   L"spy",     L"engineer" };

	return (nClassNum < 10 && nClassNum > 0) ? szClasses[nClassNum] : szClasses[0];
}

void CESP::CreateDLight(int nIndex, Color_t DrawColor, const Vec3 &vOrigin, float flRadius)
{
	auto pLight = g_Interfaces.EngineEffects->CL_AllocDlight(nIndex);
	pLight->m_flDie = g_Interfaces.Engine->Time() + 0.5f;
	pLight->m_flRadius = flRadius;
	pLight->m_Color.r = DrawColor.r;
	pLight->m_Color.g = DrawColor.g;
	pLight->m_Color.b = DrawColor.b;
	pLight->m_Color.m_Exponent = 5;
	pLight->m_nKey = nIndex;
	pLight->m_flDecay = 512.0f;
	pLight->m_vOrigin = vOrigin + Vec3(0.0f, 0.0f, 50.0f);
}

//Got this from dude719, who got it from somewhere else
void CESP::Draw3DBox(Vec3* vPoints, Color_t clr)
{
	Vector vStart, vEnd;

	for (int i = 0; i < 3; i++)
	{
		if (Utils::W2S(vPoints[i], vStart))
			if (Utils::W2S(vPoints[i + 1], vEnd))
				g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
	}

	if (Utils::W2S(vPoints[0], vStart))
		if (Utils::W2S(vPoints[3], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);

	for (int i = 4; i < 7; i++)
	{
		if (Utils::W2S(vPoints[i], vStart))
			if (Utils::W2S(vPoints[i + 1], vEnd))
				g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
	}

	if (Utils::W2S(vPoints[4], vStart))
		if (Utils::W2S(vPoints[7], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);

	if (Utils::W2S(vPoints[0], vStart))
		if (Utils::W2S(vPoints[6], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);

	if (Utils::W2S(vPoints[1], vStart))
		if (Utils::W2S(vPoints[5], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);

	if (Utils::W2S(vPoints[2], vStart))
		if (Utils::W2S(vPoints[4], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);

	if (Utils::W2S(vPoints[3], vStart))
		if (Utils::W2S(vPoints[7], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
}

void CESP::DrawBones(CBaseEntity* pPlayer, const std::vector<int>& vecBones, Color_t clr)
{
	size_t n, nMax = vecBones.size(), nLast = (nMax - 1);
	for (n = 0; n < nMax; n++)
	{
		if (n == nLast)
			continue;

		const auto vBone = pPlayer->GetHitboxPos(vecBones[n]);
		const auto vParent = pPlayer->GetHitboxPos(vecBones[n + 1]);

		Vec3 vScreenBone, vScreenParent;

		if (Utils::W2S(vBone, vScreenBone) && Utils::W2S(vParent, vScreenParent))
			g_Draw.Line(vScreenBone.x, vScreenBone.y, vScreenParent.x, vScreenParent.y, clr);
	}
}

void CESP::OffScreen(const Vec3& position, int alpha, Color_t teamColor) {
	Vec3 view_origin, target_pos, delta;
	vec2_t screen_pos, offscreen_pos;
	float  leeway_x, leeway_y, radius, offscreen_rotation;
	bool   is_on_screen;
	Vertex_fart verts[3], verts_outline[3];

	// todo - dex; move this?
	static auto get_offscreen_data = [](const Vec3& delta, float radius, vec2_t& out_offscreen_pos, float& out_rotation) {
		Vec3 view_angles(g_Interfaces.ViewRender->GetViewSetup()->angles);
		Vec3 fwd, right, up(0.f, 0.f, 1.f);
		float  front, side, yaw_rad, sa, ca;

		// get viewport angles forward directional vector.
		Math::AngleVectors(view_angles, &fwd);

		// convert viewangles forward directional vector to a unit vector.
		fwd.z = 0.f;
		fwd.Normalize();

		// calculate front / side positions.
		right = up.Cross(fwd);
		front = delta.Dot(fwd);
		side = delta.Dot(right);

		// setup offscreen position.
		out_offscreen_pos.x = radius * -side;
		out_offscreen_pos.y = radius * -front;

		// get the rotation ( yaw, 0 - 360 ).
		out_rotation = RAD2DEG(std::atan2(out_offscreen_pos.x, out_offscreen_pos.y) + PI);

		// get needed sine / cosine values.
		yaw_rad = DEG2RAD(-out_rotation);
		sa = std::sin(yaw_rad);
		ca = std::cos(yaw_rad);

		// rotate offscreen position around.
		out_offscreen_pos.x = (int)((g_ScreenSize.w / 2.f) + (radius * sa));
		out_offscreen_pos.y = (int)((g_ScreenSize.h / 2.f) - (radius * ca));
	};

	if (!Vars::ESP::Players::Arrows::Active.m_Var)
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		int teamNum = pLocal->GetTeamNum();
		// get the player's center screen position.
		target_pos = position;
		is_on_screen = Utils::W2Svec2(target_pos, screen_pos);

		// give some extra room for screen position to be off screen.
		leeway_x = g_ScreenSize.w / 18.f;
		leeway_y = g_ScreenSize.h / 18.f;

		// origin is not on the screen at all, get offscreen position data and start rendering.
		if (!is_on_screen
			|| screen_pos.x < -leeway_x
			|| screen_pos.x >(g_ScreenSize.w + leeway_x)
			|| screen_pos.y < -leeway_y
			|| screen_pos.y >(g_ScreenSize.h + leeway_y)) {

			// get viewport origin.
			view_origin = g_Interfaces.ViewRender->GetViewSetup()->origin;

			// get direction to target.
			delta = (target_pos - view_origin).Normalized();

			// note - dex; this is the 'YRES' macro from the source sdk.
			radius = Vars::ESP::Players::Arrows::DistFromCenter.m_Var * (g_ScreenSize.w / 480.f);

			// get the data we need for rendering.
			get_offscreen_data(delta, radius, offscreen_pos, offscreen_rotation);

			// bring rotation back into range... before rotating verts, sine and cosine needs this value inverted.
			// note - dex; reference: 
			// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/game/client/tf/tf_hud_damageindicator.cpp#L182
			offscreen_rotation = -offscreen_rotation;

			// setup vertices for the triangle.
			verts[0] = { offscreen_pos.x, offscreen_pos.y };        // 0,  0
			verts[1] = { offscreen_pos.x - 12.f, offscreen_pos.y + 24.f }; // -1, 1
			verts[2] = { offscreen_pos.x + 12.f, offscreen_pos.y + 24.f }; // 1,  1

			// setup verts for the triangle's outline.
			verts_outline[0] = { verts[0].m_pos.x - 1.f, verts[0].m_pos.y - 1.f };
			verts_outline[1] = { verts[1].m_pos.x - 1.f, verts[1].m_pos.y + 1.f };
			verts_outline[2] = { verts[2].m_pos.x + 1.f, verts[2].m_pos.y + 1.f };

			// rotate all vertices to point towards our target.
			verts[0] = Math::RotateVertex(offscreen_pos, verts[0], offscreen_rotation);
			verts[1] = Math::RotateVertex(offscreen_pos, verts[1], offscreen_rotation);
			verts[2] = Math::RotateVertex(offscreen_pos, verts[2], offscreen_rotation);
			// verts_outline[ 0 ] = render::RotateVertex( offscreen_pos, verts_outline[ 0 ], offscreen_rotation );
			// verts_outline[ 1 ] = render::RotateVertex( offscreen_pos, verts_outline[ 1 ], offscreen_rotation );
			// verts_outline[ 2 ] = render::RotateVertex( offscreen_pos, verts_outline[ 2 ], offscreen_rotation );

			// todo - dex; finish this, i want it.
			// auto &damage_data = m_offscreen_damage[ player->index( ) ];
			// 
			// // the local player was damaged by another player recently.
			// if( damage_data.m_time > 0.f ) {
			//     // // only a small amount of time left, start fading into white again.
			//     // if( damage_data.m_time < 1.f ) {
			//     //     // calculate step needed to reach 255 in 1 second.
			//     //     // float step = UINT8_MAX / ( 1000.f * g_csgo.m_globals->m_frametime );
			//     //     float step = ( 1.f / g_csgo.m_globals->m_frametime ) / UINT8_MAX;
			//     //     
			//     //     // increment the new value for the color.
			//     //     // if( damage_data.m_color_step < 255.f )
			//     //         damage_data.m_color_step += step;
			//     // 
			//     //     math::clamp( damage_data.m_color_step, 0.f, 255.f );
			//     // 
			//     //     damage_data.m_color.g( ) = (uint8_t)damage_data.m_color_step;
			//     //     damage_data.m_color.b( ) = (uint8_t)damage_data.m_color_step;
			//     // }
			//     // 
			//     // g_cl.print( "%f %f %u %u %u\n", damage_data.m_time, damage_data.m_color_step, damage_data.m_color.r( ), damage_data.m_color.g( ), damage_data.m_color.b( ) );
			//     
			//     // decrement timer.
			//     damage_data.m_time -= g_csgo.m_globals->m_frametime;
			// }
			// 
			// else
			//     damage_data.m_color = colors::white;

			// render!
			//color = g_menu.main.players.offscreen_color.get(); // damage_data.m_color;
			teamColor.a = (alpha == 255) ? alpha : alpha / 2;

			g_Draw.DrawTexturedPolygon(3, verts, teamColor);
			//g_csgo.m_surface->DrawTexturedPolygon(3, verts);

			// g_csgo.m_surface->DrawSetColor( colors::black );
			// g_csgo.m_surface->DrawTexturedPolyLine( 3, verts_outline );
		}
	}
}

bool CPlayerArrows::ShouldRun(CBaseEntity* pLocal)
{
	if (!Vars::ESP::Players::Arrows::Active.m_Var || g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	if (!pLocal->IsAlive() || pLocal->IsStunned())
		return false;

	if (pLocal->IsInBumperKart() || pLocal->IsAGhost())
		return false;

	return true;
}
//
//void CPlayerArrows::DrawArrowTo(const Vec3& vecFromPos, const Vec3& vecToPos, Color_t color)
//{
//	color.a = 150;
//	auto GetClockwiseAngle = [&](const Vec3& vecViewAngle, const Vec3& vecAimAngle) -> float
//	{
//		Vec3 vecAngle = Vec3();
//		Math::AngleVectors(vecViewAngle, &vecAngle);
//
//		Vec3 vecAim = Vec3();
//		Math::AngleVectors(vecAimAngle, &vecAim);
//
//		return -atan2(vecAngle.x * vecAim.y - vecAngle.y * vecAim.x, vecAngle.x * vecAim.x + vecAngle.y * vecAim.y);
//	};
//
//	auto MapFloat = [&](float x, float in_min, float in_max, float out_min, float out_max) -> float {
//		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
//	};
//
//	Vec3 vecAngleTo = Math::CalcAngle(vecFromPos, vecToPos);
//	Vec3 vecViewAngle = g_Interfaces.Engine->GetViewAngles();
//
//	const float deg = GetClockwiseAngle(vecViewAngle, vecAngleTo);
//	const float xrot = cos(deg - PI / 2);
//	const float yrot = sin(deg - PI / 2);
//
//	const float x1 = ((g_ScreenSize.w * Vars::ESP::Players::Arrows::DistFromCenter.m_Var) + 5.f) * xrot;// *1.5;
//	const float y1 = ((g_ScreenSize.w * Vars::ESP::Players::Arrows::DistFromCenter.m_Var) + 5.f) * yrot;
//	const float x2 = ((g_ScreenSize.w * Vars::ESP::Players::Arrows::DistFromCenter.m_Var) + 10.f) * xrot; //* 1.5;
//	const float y2 = ((g_ScreenSize.w * Vars::ESP::Players::Arrows::DistFromCenter.m_Var) + 10.f) * yrot;
//
//	constexpr float arrow_angle = DEG2RAD(90.f);
//	//float arrow_angle = DEG2RAD(Vars::Visuals::ArrowAngle.m_Var);
//	//constexpr float arrow_lenght = 20.0f;
//	float arrow_lenght = Vars::ESP::Players::Arrows::ArrowSize.m_Var;
//
//	const Vec3 line{ x2 - x1, y2 - y1, 0.0f };
//	const float length = line.Lenght();
//
//	const float fpoint_on_line = arrow_lenght / (atanf(arrow_angle) * length);
//	const Vec3 point_on_line = Vec3(x2, y2, 0) + (line * fpoint_on_line * -1.0f);
//	const Vec3 normal_vector{ -line.y, line.x, 0.0f };
//	const Vec3 normal = Vec3(arrow_lenght, arrow_lenght, 0.0f) / (length * 2);
//
//	const Vec3 rotation = normal * normal_vector;
//	const Vec3 left = point_on_line + rotation;
//	const Vec3 right = point_on_line - rotation;
//
//	float cx = static_cast<float>(g_ScreenSize.w / 2);
//	float cy = static_cast<float>(g_ScreenSize.h / 2);
//
//	//float fMap = std::clamp(MapFloat(vecFromPos.DistTo(vecToPos), 1000.0f, 100.0f, 0.0f, 1.0f), 0.0f, 1.0f);
//	//float fMap = std::clamp(MapFloat(vecFromPos.DistTo(vecToPos), 1000.f, 100.f, 0.0f, 1.0f), 0.0f, 1.0f);
//	Color_t HeatColor = color;
//	//HeatColor.a = static_cast<byte>(fMap * 255.0f);
//	//static bool alphaUp = true;
//	//static int alpha = 0;
//
//	HeatColor.a = alpha;
//	//g_Draw.Line(cx + x2, cy + y2, cx + left.x, cy + left.y, HeatColor);
//	//g_Draw.Line(cx + x2, cy + y2, cx + right.x, cy + right.y, HeatColor);
//	//g_Draw.Line(cx + left.x, cy + left.y, cx + right.x, cy + right.y, HeatColor);
//
//	std::array<Vec2, 3>points
//	{
//		Vec2(cx + left.x, cy + left.y),
//		Vec2(cx + right.x, cy + right.y),
//		Vec2(cx+ x2, cy + y2)
//	};
//	g_Draw.DrawFilledTriangle(points, HeatColor);
//	//g_Draw.DrawFilledTriangle()
//}
//
void CPlayerArrows::Run()
{

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!ShouldRun(pLocal))
			return;

		Vec3 vLocalPos = pLocal->GetWorldSpaceCenter();

		m_vecPlayers.clear();

		for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			if (!pEnemy || !pEnemy->IsAlive() || pEnemy->IsCloaked() || pEnemy->IsAGhost())
				continue;

			if (Vars::Visuals::SpyWarningIgnoreFriends.m_Var && g_EntityCache.Friends[pEnemy->GetIndex()])
				continue;

			Vec3 vEnemyPos = pEnemy->GetWorldSpaceCenter();

			//Vec3 vAngleToEnemy = Math::CalcAngle(vLocalPos, vEnemyPos);
			//Vec3 viewangless = g_Interfaces.Engine->GetViewAngles();
			//viewangless.x = 0;
			//float fFovToEnemy = Math::CalcFov(viewangless, vAngleToEnemy);

			//if (fFovToEnemy < Vars::Visuals::FieldOfView.m_Var)
			//	continue;

			m_vecPlayers.push_back(vEnemyPos);
		}
		if (m_vecPlayers.empty())
			return;

		for (const auto& Player : m_vecPlayers) {
			Color_t teamColor;
			if (pLocal->GetTeamNum() == 2) {
				teamColor = Colors::TeamBlu;
			}
			else {
				teamColor = Colors::TeamRed;
			}
			g_ESP.OffScreen(Player, alpha, teamColor);
		}
	}
}