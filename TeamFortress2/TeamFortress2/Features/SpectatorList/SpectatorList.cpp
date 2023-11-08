#include "SpectatorList.h"

#include "../Vars.h"

bool CSpectatorList::GetSpectators(CBaseEntity *pLocal)
{
	m_vecSpectators.clear();

	for (const auto &pTeammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
	{
		CBaseEntity *pObservedPlayer = g_Interfaces.EntityList->GetClientEntityFromHandle(pTeammate->GetObserverTarget());

		if (pTeammate && !pTeammate->IsAlive() && pObservedPlayer == pLocal)
		{
			std::wstring szMode = L"";

			switch (pTeammate->GetObserverMode()) {
				case OBS_MODE_FIRSTPERSON: { szMode = _(L"1st"); break; }
				case OBS_MODE_THIRDPERSON: { szMode = _(L"3rd"); break; }
				default: continue;
			}

			PlayerInfo_t PlayerInfo;
			if (g_Interfaces.Engine->GetPlayerInfo(pTeammate->GetIndex(), &PlayerInfo))
				m_vecSpectators.push_back({ Utils::ConvertUtf8ToWide(PlayerInfo.name), szMode, g_EntityCache.Friends[pTeammate->GetIndex()], pTeammate->GetTeamNum(), pTeammate->GetIndex() });
		}
	}

	return !m_vecSpectators.empty();
}

bool CSpectatorList::ShouldRun()
{
	return Vars::Visuals::SpectatorList.m_Var && !g_Interfaces.EngineVGui->IsGameUIVisible();
}

void CSpectatorList::Run()
{
	if (!ShouldRun())
		return;

	if (Vars::Visuals::SpectatorList.m_Var)
		DrawClassic();
}

void CSpectatorList::DragSpecList(int& x, int& y, int w, int h, int offsety)
{
	if (!g_Menu.menuOpen)
		return;

	int mousex, mousey;
	g_Interfaces.Surface->GetCursorPos(mousex, mousey);

	static POINT delta;
	static bool drag = false;
	static bool move = false;
	bool held = GetAsyncKeyState(VK_LBUTTON);

	if ((mousex > x && mousex < x + w && mousey > y - offsety && mousey < y - offsety + h) && held)
	{
		drag = true;

		if (!move) {
			delta.x = mousex - x;
			delta.y = mousey - y;
			move = true;
		}
	}

	if (drag) {
		x = mousex - delta.x;
		y = mousey - delta.y;
	}

	if (!held) {
		drag = false;
		move = false;
	}
}

void CSpectatorList::DrawClassic()
{
	DragSpecList(
		m_nSpecListX,
		m_nSpecListY,
		m_nSpecListW,
		m_nSpecListTitleBarH,
		0);

	int wz, hz;
	g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_MENU].dwFont, _(L"Spectators"), wz, hz);

	if (g_Menu.menuOpen || m_vecSpectators.size() > 0) {
		g_Draw.Rect(m_nSpecListX, m_nSpecListY, m_nSpecListW, 2, Vars::Menu::Colors::WidgetActive);

		//g_Draw.Rect(m_nSpecListX, m_nSpecListY + 2, m_nSpecListW, g_Draw.m_vecFonts[FONT_MENU].nTall + 5, { 0,0,0,200 });

		//g_Draw.String(FONT_MENU, m_nSpecListX + wz, m_nSpecListY + 10, { 255,255,255,255 }, ALIGN_CENTERVERTICAL, _("Spectators"));

		g_Draw.Rect(m_nSpecListX - m_nSpecListY + 2, m_nSpecListW, 2 - 2, 18, { 20,20,20,255 });

		g_Draw.String(FONT_MENU, m_nSpecListX + wz, m_nSpecListY + 10, { 200,200,200,255 }, ALIGN_DEFAULT, _("Spectators"));
	}

	if (const auto &pLocal = g_EntityCache.m_pLocal)
	{
		if (!pLocal->IsAlive() || !GetSpectators(pLocal))
			return;

		int nDrawY = m_nSpecListY;

		for (const auto& Spectator : m_vecSpectators)
		{
			int nDrawX = g_ScreenSize.c;

			int w, h;
			g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_MENU].dwFont, (Spectator.m_sMode + Spectator.m_sName).c_str(), w, h);

			int nAddX = 0, nAddY = g_Draw.m_vecFonts[FONT_MENU].nTall;

			if (Vars::Visuals::SpectatorList.m_Var)
			{
				nDrawX -= 55;

				PlayerInfo_t pi;
				if (!g_Interfaces.Engine->GetPlayerInfo(Spectator.m_nIndex, &pi))
					continue;

				g_Draw.Avatar(m_nSpecListX, nDrawY + 25, 15, 15, pi.friendsID);
				nDrawY += 6;

				nAddX = 25;
				nAddY = 14;
			}

			if (Spectator.m_sMode.data() == _(L"1st")) { //  There's definitely a better way to do this lol
				g_Draw.String(FONT_MENU, m_nSpecListX + 25, nDrawY + 25, { 255,0,0,255 }, ALIGN_CENTERVERTICAL, _(L"[%ls] %ls"), Spectator.m_sMode.data(), Spectator.m_sName.data());
			}
			else {
				g_Draw.String(FONT_MENU, m_nSpecListX + 25, nDrawY + 25, { 255,255,255,255 }, ALIGN_CENTERVERTICAL, _(L"[%ls] %ls"), Spectator.m_sMode.data(), Spectator.m_sName.data());
			}
			nDrawY += nAddY;
		}
	}
}