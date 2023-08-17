#include "Menu.h"
#include "../Menu/Menu.h"
#include "../ConfigManager/ConfigManager.h"
#include "../Visuals/Visuals.h"
#include "../Playerlist/Playerlist.h"
#include "font_awesome_5.h"
#include "../Misc/Misc.h"

// I know this might sound dumb but here are the rules before messing with the menu
// Before you add stuff here please tell me first what you're going to add and we'll discuss about it
// Please test stuff before you push, don't pull a fedoraware moment pls :(
// YOU MUST encrypt every string with "_(stuffgoeshere)" except for vectors and shit like that
    // Q: Why can't I do that on vectors?
    // A: Because they fuck up for some reason, idk honestly lol
// Try to fit in as much features as possible without making it look out of place / not user friendly (if that makes sense)
// If you have b1g problems with the menu then message me on discord

static int tab = 0;

auto s = ImVec2{};
auto p = ImVec2{};
auto MenuSize = ImVec2{ 784, 522 };

#define FixSlider ImGui::PushItemWidth(ImGui::GetCurrentWindow()->Size.x - 8)

ImVec4 to_vec4(float r, float g, float b, float a)
{
    return ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

ImVec4 mColor(Color_t color) {
    return ImVec4(Color::TOFLOAT(color.r), Color::TOFLOAT(color.g), Color::TOFLOAT(color.b), Color::TOFLOAT(color.a));
}

Color_t vColor(ImVec4 color) {
    return {
        (byte)(color.x * 256.0f > 255 ? 255 : color.x * 256.0f),
        (byte)(color.y * 256.0f > 255 ? 255 : color.y * 256.0f),
        (byte)(color.z * 256.0f > 255 ? 255 : color.z * 256.0f),
        (byte)(color.w * 256.0f > 255 ? 255 : color.w * 256.0f)
    };
}

std::string name;

void ColorPicker(const char* label, Color_t& color, bool alpha = true) {
    ImVec4 FUCKOFF = mColor(color);
    ImGui::PushItemWidth(150);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12);
    if (alpha) {
        if (ImGui::ColorEdit4(label, &FUCKOFF.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview)) {
            color = vColor(FUCKOFF);
        }
    }
    else {
        if (ImGui::ColorEdit3(label, &FUCKOFF.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
            color = vColor(FUCKOFF);
        }
    }
    ImGui::PopStyleVar();
    ImGui::PopItemWidth();
}

void ColorPicker2(const char* label, Color_t& color, bool alpha = true) {
    ImVec4 FUCKOFF = mColor(color);
    ImGui::PushItemWidth(150);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12);
    if (alpha) {
        if (ImGui::ColorEdit4(label, &FUCKOFF.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview)) {
            color = vColor(FUCKOFF);
        }
    }
    else {
        if (ImGui::ColorEdit3(label, &FUCKOFF.x, ImGuiColorEditFlags_NoInputs)) {
            color = vColor(FUCKOFF);
        }
    }
    ImGui::PopStyleVar();
    ImGui::PopItemWidth();
}

Color_t* vpColor(ImVec4 color) {
    Color_t col = { (byte)(color.x * 255.0f),
        (byte)(color.y * 255.0f),
        (byte)(color.z * 255.0f),
        (byte)(color.w * 255.0f) };
    return &col;
}

void AlignToRight(int lul) { // My friend told me that you could do it with a macro but fuck it
    ImGui::SameLine(ImGui::GetContentRegionMax().x - lul);
    ImGui::SetNextItemWidth(lul);
}

auto MenuCol = mColor(Vars::Menu::Colors::WidgetActive); // Getting the menu color for "CustomStyle()"
auto ShadowCol = mColor(Vars::Menu::Colors::ShadowColor); // Getting the Shadow color for "CustomStyle()"

void CustomStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Border] = ImVec4(0.24f, 0.24f, 0.24f, 0.33f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.08f, 0.09f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = MenuCol;
    style.Colors[ImGuiCol_Button] = ImVec4(0.07f, 0.08f, 0.09f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05, 0.05, 0.05, 1.00);
    style.Colors[ImGuiCol_ScrollbarGrab] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_ScrollbarGrabActive] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_FrameBgHovered] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_FrameBgActive] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_TitleBgActive] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_CheckMark] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_SliderGrab] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_SliderGrabActive] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_Header] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_HeaderHovered] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_HeaderActive] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_SeparatorHovered] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_SeparatorActive] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_ResizeGrip] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_ResizeGripHovered] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_ResizeGripActive] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_Tab] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_TabActive] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_TabUnfocusedActive] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_DragDropTarget] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_NavHighlight] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_WindowShadow] = ShadowCol;

    style.FrameBorderSize = 1.0f;
    style.WindowShadowSize = Vars::Menu::ShadowSize;
}

bool ClickText(const char* label) {
    const auto id = ImGui::GetID(label);
    ImGui::PushID(label);
    ImGui::Text(label);
    if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[0]) {
        return true;
    }
    ImGui::PopID();
    return false;
}

bool InputKeybind(const char* label, CVar<int>& output, bool bAllowNone = true)
{
    bool active = false;

    auto VK2STR = [&](const short key) -> const char* {
        switch (key) {
        case VK_LBUTTON: return "M1";
        case VK_RBUTTON: return "M2";
        case VK_MBUTTON: return "M3";
        case VK_XBUTTON1: return "M4";
        case VK_XBUTTON2: return "M5";
        case VK_SPACE: return "SPC";
        case 0x0: return "None";
        case VK_A: return "A";
        case VK_B: return "B";
        case VK_C: return "C";
        case VK_D: return "D";
        case VK_E: return "E";
        case VK_F: return "F";
        case VK_G: return "G";
        case VK_H: return "H";
        case VK_I: return "J";
        case VK_J: return "I";
        case VK_K: return "K";
        case VK_L: return "L";
        case VK_M: return "M";
        case VK_N: return "N";
        case VK_O: return "O";
        case VK_P: return "P";
        case VK_Q: return "Q";
        case VK_R: return "R";
        case VK_S: return "S";
        case VK_T: return "T";
        case VK_U: return "U";
        case VK_V: return "V";
        case VK_W: return "W";
        case VK_X: return "X";
        case VK_Y: return "Y";
        case VK_Z: return "Z";
        case VK_0: return "0";
        case VK_1: return "1";
        case VK_2: return "2";
        case VK_3: return "3";
        case VK_4: return "4";
        case VK_5: return "5";
        case VK_6: return "6";
        case VK_7: return "7";
        case VK_8: return "8";
        case VK_9: return "9";
        case VK_ESCAPE: return "ESC";
        case VK_SHIFT: return "SHFT";
        case VK_LSHIFT: return "SHFT";
        case VK_RSHIFT: return "SHFT";
        case VK_CONTROL: return "CTRL";
        case VK_MENU: return "ALT";
        default: break;
        }

        WCHAR output[16] = { L"\0" };
        if (const int result = GetKeyNameTextW(MapVirtualKeyW(key, MAPVK_VK_TO_VSC) << 16, output, 16)) {
            char outputt[128];
            sprintf(outputt, "%ws", output);
            return outputt;
        }

        return "VK2STR_FAILED";
    };

    auto labell = VK2STR(output.m_Var);

    const auto id = ImGui::GetID(label);
    ImGui::PushID(label);

    if (ImGui::GetActiveID() == id) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
        ImGui::Button("...", ImVec2(45, 17));
        ImGui::PopStyleColor();

        static float time = g_Interfaces.Engine->Time();
        float elapsed = g_Interfaces.Engine->Time() - time;
        static CVar<int>* curr = nullptr, * prevv = curr;
        if (curr != prevv) {
            time = g_Interfaces.Engine->Time();
            prevv = curr;
        }

        if (curr == nullptr && elapsed > 0.1f) {
            for (short n = 0; n < 256; n++) {
                if ((n > 0x0 && n < 0x7) || (n > L'A' - 1 && n < L'Z' + 1) || (n > L'0' - 1 && n < L'9' + 1) || n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT || n == VK_ESCAPE || n == VK_HOME || n == VK_CONTROL || n == VK_MENU) {
                    if ((!ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[0])) {
                        ImGui::ClearActiveID();
                        break;
                    }
                    if (GetAsyncKeyState(n) & 0x8000)
                    {
                        if (n == VK_HOME || n == VK_INSERT) {
                            break;
                        }

                        if (n == VK_ESCAPE && bAllowNone) {
                            ImGui::ClearActiveID();
                            output.m_Var = 0x0;
                            break;
                        }

                        output.m_Var = n;
                        ImGui::ClearActiveID();
                        break;
                    }
                } //loop
            }
        }

        if (curr != prevv) {
            time = g_Interfaces.Engine->Time();
            prevv = curr;
        }

        ImGui::GetCurrentContext()->ActiveIdAllowOverlap = true;

        if ((!ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[0]))
            ImGui::ClearActiveID();
    }
    else if (ImGui::Button(VK2STR(output.m_Var), ImVec2(45, 17))) {
        ImGui::SetActiveID(id, ImGui::GetCurrentWindow());
    }

    ImGui::PopID();

    return true;
}

void combo(const char* label, int* current_item, const char* const* items, int items_count, int popup_max_height_in_items = -1) {
    ImGui::PushItemWidth(100);
    ImGui::Combo(label, current_item, items, items_count, popup_max_height_in_items);
    ImGui::PopItemWidth();
}

ImVec4 clear_color = ImVec4(0.51f, 0.55f, 0.57f, 1.00f);
static bool one = true;
static bool two = false;
static int sl = 0;

void AimbotTab() {
    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);
        ImGui::MenuChild(_("Global"), ImVec2(250, 260), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Enabled"), &Vars::Aimbot::Global::Active.m_Var);
            //ImGui::Checkbox("Autoshoot", &Vars::Aimbot::Global::AutoShoot.m_Var); // We don't really need auto shoot atm.
            AlignToRight(50);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(MenuCol.x / 1.5, MenuCol.y / 1.5, MenuCol.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(MenuCol.x, MenuCol.y, MenuCol.z, 255));
            InputKeybind(_("Aim key"), Vars::Aimbot::Global::AimKey);
            ImGui::PopStyleColor(3);
            FixSlider;
            ImGui::SliderFloat("Aimbot FOV", &Vars::Aimbot::Hitscan::AimFOV.m_Var, 1.0f, 180.f, _("%.0f"), ImGuiSliderFlags_AlwaysClamp);
            ImGui::Checkbox(_("Aim at players"), &Vars::Aimbot::Global::AimPlayers.m_Var);
            AlignToRight(23);
            ColorPicker(_("Target"), Colors::Target);
            ImGui::Checkbox(_("Aim at buildings"), &Vars::Aimbot::Global::AimBuildings.m_Var);
            ImGui::Checkbox(_("Ignore invulnerable"), &Vars::Aimbot::Global::IgnoreInvlunerable.m_Var);
            AlignToRight(23);
            ColorPicker(_("Invulnerable"), Colors::Invuln);
            ImGui::Checkbox(_("Ignore cloaked"), &Vars::Aimbot::Global::IgnoreCloaked.m_Var);
            AlignToRight(23);
            ColorPicker(_("Cloaked"), Colors::Cloak);
            ImGui::Checkbox(_("Ignore friends"), &Vars::Aimbot::Global::IgnoreFriends.m_Var);
            ImGui::Checkbox(_("Ignore taunting"), &Vars::Aimbot::Global::IgnoreTaunting.m_Var);
            ImGui::Checkbox(_("Ignore Vaccinator"), &Vars::Aimbot::Global::IgnoreVaccinator.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }


    {//left bottom
        ImGui::SetCursorPosY(302);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);
        ImGui::MenuChild(_("Crithack"), ImVec2(250, 210), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            static bool lefunny = false;
            static bool lefunny2 = false;
            ImGui::Checkbox(_("Enabled"), &lefunny);
            AlignToRight(50);
            InputKeybind(_("Crithack Key"), Vars::Aimbot::CritHack::CritKey);
            ImGui::Checkbox(_("Crit Bar"), &Vars::Aimbot::CritHack::CritBar.m_Var);
            ImGui::Checkbox(_("Melee Crits"), &Vars::Aimbot::CritHack::MeleeCrits.m_Var);
            ImGui::Checkbox(_("Save Bucket"), &Vars::Aimbot::CritHack::SaveBucket.m_Var);

            if (lefunny) {
                ImGui::Text(_("Why did you enable this you fucking tard"));
                ImGui::Text(_("Have patience"));
                if (!lefunny2) {
                    g_Interfaces.Surface->PlaySound(_("vo//taunts//sniper_taunts15.mp3"));
                    lefunny2 = true;
                }
            }
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//right
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(262);
        ImGui::MenuChild(_("Hitscan"), ImVec2(250, 470), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Active###Hit scan"), &Vars::Aimbot::Hitscan::Active.m_Var);
            static const char* hitscanSortMethod[]{ "FOV", "Distance" };
            ImGui::Combo(_("Sort method###hitscanSortMethod"), &Vars::Aimbot::Hitscan::SortMethod.m_Var, hitscanSortMethod, IM_ARRAYSIZE(hitscanSortMethod));

            static const char* hitscanAimMethod[]{ "Plain", "Smooth", "Silent" };
            ImGui::Combo(_("Aim method###hitscanAimMethod"), &Vars::Aimbot::Hitscan::AimMethod.m_Var, hitscanAimMethod, IM_ARRAYSIZE(hitscanAimMethod));

            static const char* hitscanAimHitbox[]{ "Head", "Body", "Auto" };
            ImGui::Combo(_("Hitbox###hitscanAimHitbox"), &Vars::Aimbot::Hitscan::AimHitbox.m_Var, hitscanAimHitbox, IM_ARRAYSIZE(hitscanAimHitbox));

            FixSlider;
            ImGui::SliderFloat(_("Smooth factor"), &Vars::Aimbot::Hitscan::SmoothingAmount.m_Var, 1.0f, 10.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);

            static const char* hitscanTapFire[]{ "Off", "Distance", "Always" };
            ImGui::Combo(_("Tap-fire"), &Vars::Aimbot::Hitscan::TapFire.m_Var, hitscanTapFire, IM_ARRAYSIZE(hitscanTapFire));
            ImGui::Checkbox(_("Minigun rev up"), &Vars::Aimbot::Hitscan::AutoRev.m_Var);
            ImGui::Checkbox(_("Multipoint"), &Vars::Aimbot::Hitscan::ScanHitboxes.m_Var);
            //ImGui::Checkbox("Head multipoint", &Vars::Aimbot::Hitscan::ScanHead.m_Var); // uh
            ImGui::Checkbox(_("Wait for headshot"), &Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var);
            ImGui::Checkbox(_("Wait for charge"), &Vars::Aimbot::Hitscan::WaitForCharge.m_Var);
            ImGui::Checkbox(_("Scoped only"), &Vars::Aimbot::Hitscan::ScopedOnly.m_Var);
            ImGui::Checkbox(_("Scope automatically"), &Vars::Aimbot::Hitscan::AutoScope.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//right
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(517);
        ImGui::MenuChild(_("Projectile"), ImVec2(260, 186), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Active###Projectile"), &Vars::Aimbot::Projectile::Active.m_Var);
            ImGui::Checkbox(_("Performance mode"), &Vars::Aimbot::Projectile::PerformanceMode.m_Var);
            //static const char* projectileSortMethod[]{ "FOV", "Distance" };
            //ImGui::Combo(_("Sort method###projectileSortMethod"), &Vars::Aimbot::Projectile::SortMethod.m_Var, projectileSortMethod, IM_ARRAYSIZE(projectileSortMethod));

            static const char* projectileAimMethod[]{ "Plain", "Silent" };
            ImGui::Combo(_("Aim method###projectileAimMethod"), &Vars::Aimbot::Projectile::AimMethod.m_Var, projectileAimMethod, IM_ARRAYSIZE(projectileAimMethod));

            static const char* projectileAimHitbox[]{ "Body", "Feet", "Auto" };
            ImGui::Combo(_("Aim position###projectileAimPosition"), &Vars::Aimbot::Projectile::AimPosition.m_Var, projectileAimHitbox, IM_ARRAYSIZE(projectileAimHitbox));

           //ImGui::Checkbox(_("R8 Method"), &Vars::Aimbot::Projectile::R8Method.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//right
        ImGui::SetCursorPosY(227);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(517);
        ImGui::MenuChild(_("Melee"), ImVec2(260, 285), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Active###Melee"), &Vars::Aimbot::Melee::Active.m_Var);
            static const char* meleeSortMethod[]{ "FoV", "Distance" };
            ImGui::Combo(_("Sort method###meleeSortMethod"), &Vars::Aimbot::Melee::SortMethod.m_Var, meleeSortMethod, IM_ARRAYSIZE(meleeSortMethod));
            static const char* meleeAimMethod[]{ "Plain", "Smooth", "Silent" };
            ImGui::Combo(_("Aim method###meleeAimMethod"), &Vars::Aimbot::Melee::AimMethod.m_Var, meleeAimMethod, IM_ARRAYSIZE(meleeAimMethod));
            FixSlider;
            ImGui::SliderFloat(_("Smooth factor###meleeSmooth"), &Vars::Aimbot::Melee::SmoothingAmount.m_Var, 1.0f, 10.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::Checkbox(_("Range check"), &Vars::Aimbot::Melee::RangeCheck.m_Var);
            ImGui::Checkbox(_("Melee prediction"), &Vars::Aimbot::Melee::PredictSwing.m_Var);
            ImGui::Checkbox(_("Whip teammates"), &Vars::Aimbot::Melee::WhipTeam.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
}

void TriggerbotTab() {
    {//Left Upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);
        ImGui::MenuChild(_("Global"), ImVec2(250, 220), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Enabled"), &Vars::Triggerbot::Global::Active.m_Var);
            AlignToRight(50);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(MenuCol.x / 1.5, MenuCol.y / 1.5, MenuCol.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(MenuCol.x, MenuCol.y, MenuCol.z, 255));
            InputKeybind(_("Trigger key"), Vars::Triggerbot::Global::TriggerKey);
            ImGui::PopStyleColor(3);
            ImGui::Checkbox(_("Ignore Invulnerable"), &Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var);
            ImGui::Checkbox(_("Ignore Cloaked"), &Vars::Triggerbot::Global::IgnoreCloaked.m_Var);
            ImGui::Checkbox(_("Ignore Friends"), &Vars::Triggerbot::Global::IgnoreFriends.m_Var);
            ImGui::Checkbox(_("Ignore Taunts"), &Vars::Triggerbot::Global::IgnoreTaunts.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    { // Left Middle
        ImGui::SetCursorPosY(262);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);

        ImGui::MenuChild(_("Auto Airblast"), ImVec2(250, 120), false, ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(_("Active##gAAB"), &Vars::Triggerbot::Blast::Active.m_Var);
        ImGui::Checkbox(_("Rage mode##gAABr"), &Vars::Triggerbot::Blast::Rage.m_Var);
        ImGui::Checkbox(_("Silent##gAABs"), &Vars::Triggerbot::Blast::Silent.m_Var);

        ImGui::EndChild();
        ImGui::EndGroup();
    }
    { // Left Bottom
        ImGui::SetCursorPosY(382);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);

        ImGui::MenuChild(_("Auto Detonator"), ImVec2(250, 130), false, ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(_("Active###gAD"), &Vars::Triggerbot::Detonate::Active.m_Var);
        ImGui::Checkbox(_("Stickybombs###gADs"), &Vars::Triggerbot::Detonate::Stickies.m_Var);
        ImGui::Checkbox(_("Detonator flares###gADd"), &Vars::Triggerbot::Detonate::Flares.m_Var);
        //FixSlider;
        //ImGui::SliderFloat("Detonate radius###gADr", &Vars::Triggerbot::Detonate::RadiusScale.m_Var, 0.5f, 1.0f, "%.1f", ImGuiSliderFlags_Logarithmic);

        ImGui::EndChild();
        ImGui::EndGroup();
    }
    // Left middle row


    { // Middle Upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(262);

        ImGui::MenuChild(_("Auto Stab"), ImVec2(250, 220), false, ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(_("Active###gABS"), &Vars::Triggerbot::Stab::Active.m_Var);
        ImGui::Checkbox(_("Rage mode###gABSr"), &Vars::Triggerbot::Stab::RageMode.m_Var);
        ImGui::Checkbox(_("Silent###gABSs"), &Vars::Triggerbot::Stab::Silent.m_Var);
        ImGui::Checkbox(_("Disguise after stab###gABSd"), &Vars::Triggerbot::Stab::Disguise.m_Var);
        ImGui::Checkbox(_("Ignore razorback###gABSig"), &Vars::Triggerbot::Stab::IgnRazor.m_Var);
        FixSlider;
        ImGui::SliderFloat(_("Stab range"), &Vars::Triggerbot::Stab::Range.m_Var, 0.5f, 1.0f, _("%.1f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);


        ImGui::EndChild();
        ImGui::EndGroup();
    }
    { // Middle bottom
        ImGui::SetCursorPosY(262);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(262);

        ImGui::MenuChild(_("Auto Uber"), ImVec2(250, 250), false, ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(_("Active###gAU"), &Vars::Triggerbot::Uber::Active.m_Var);
        ImGui::Checkbox(_("Only on friends###gAUf"), &Vars::Triggerbot::Uber::OnlyFriends.m_Var);
        ImGui::Checkbox(_("Uber self###gAUs"), &Vars::Triggerbot::Uber::PopLocal.m_Var);
        FixSlider;
        ImGui::SliderFloat(_("Health left###gAUhp"), &Vars::Triggerbot::Uber::HealthLeft.m_Var, 1.f, 99.f, _("%.0f%%"), ImGuiSliderFlags_AlwaysClamp);

        ImGui::EndChild();
        ImGui::EndGroup();
    }

    { // Right upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(517);

        ImGui::MenuChild(_("Auto Shoot"), ImVec2(260, 220), false, ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(_("Active##gAS"), &Vars::Triggerbot::Shoot::Active.m_Var);
        ImGui::Checkbox(_("Shoot players##gASsp"), &Vars::Triggerbot::Shoot::TriggerPlayers.m_Var);
        ImGui::Checkbox(_("Shoot buildings##gASsb"), &Vars::Triggerbot::Shoot::TriggerBuildings.m_Var);
        ImGui::Checkbox(_("Head only##gASho"), &Vars::Triggerbot::Shoot::HeadOnly.m_Var);
        ImGui::Checkbox(_("Wait for headshot##gASwfc"), &Vars::Triggerbot::Shoot::WaitForHeadshot.m_Var);
        ImGui::Checkbox(_("Scoped Only##gASwfc"), &Vars::Triggerbot::Shoot::ScopedOnly.m_Var);
        FixSlider;
        ImGui::SliderFloat(_("Head scale##gAShs"), &Vars::Triggerbot::Shoot::HeadScale.m_Var, 0.5f, 1.0f, "%.1f", ImGuiSliderFlags_Logarithmic);

        ImGui::EndChild();
        ImGui::EndGroup();
    }

    { // Middle bottom
        ImGui::SetCursorPosY(262);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(517);

        ImGui::MenuChild(_("Auto Vaccinator"), ImVec2(260, 250), false, ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(_("Active###gAV"), &Vars::Triggerbot::AutoVaccinator::Active.m_Var);
        ImGui::Checkbox(_("Only on friends###gAUf"), &Vars::Triggerbot::AutoVaccinator::OnlyFriends.m_Var);
        FixSlider;
        ImGui::SliderFloat(_("Health left###gAUhp"), &Vars::Triggerbot::AutoVaccinator::UberMaxPercentage.m_Var, 1.f, 99.f, _("%.0f%%"), ImGuiSliderFlags_AlwaysClamp);

        ImGui::EndChild();
        ImGui::EndGroup();
    }
}
int nESPTab = 1;

void ESPTab() {
    {//left upper

        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);
        ImGui::MenuChild(_("Player"), ImVec2(250, 446), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5,5 });
            ImGui::SetNextWindowSize(ImVec2(160, 0));
            if (ImGui::BeginPopup(_("PlayerHealthBar"), ImGuiWindowFlags_NoScrollWithMouse)) {
                ImGui::Text(_(ICON_FA_EYE " Player Health Bar"));
                ImGui::Separator();
                ImGui::SetCursorPosX(2);
                ColorPicker2(_("Healthbar top color"), Colors::HealthBarTopColor, false);
                ImGui::SetCursorPosX(2);
                ColorPicker2(_("Healthbar bottom color"), Colors::HealthBarBottomColor, false);

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            ImGui::Checkbox(_("Enabled"), &Vars::ESP::Players::Active.m_Var);
            AlignToRight(23);
            ColorPicker(_("Team Blu"), Colors::TeamBlu, false);
            AlignToRight(43);
            ColorPicker(_("Team Red"), Colors::TeamRed, false);
            ImGui::Checkbox(_("Local ESP"), &Vars::ESP::Players::ShowLocal.m_Var);
            AlignToRight(23);
            ColorPicker(_("Local / Friend"), Colors::Friend);
            //ImGui::Checkbox(_("Player health"), &Vars::ESP::Players::Health.m_Var);
            ImGui::Checkbox(_("Player conditions"), &Vars::ESP::Players::Cond.m_Var);
            AlignToRight(23);
            ColorPicker(_("Conditions"), Colors::Cond);

            ImGui::Checkbox(_("Player health bar"), &Vars::ESP::Players::Healthbar::Enabled.m_Var);
            AlignToRight(20);
            ImGui::Text(ICON_FA_COG);
            AlignToRight(23);
            if (ImGui::InvisibleButton(_("PlayerHealthBar"), ImVec2(20, 20))) {
                ImGui::OpenPopup(_("PlayerHealthBar"));
            }
            ImGui::Checkbox(_("Player name"), &Vars::ESP::Players::Name.m_Var);
            ImGui::Checkbox(_("Custom name color"), &Vars::ESP::Players::NameCustom.m_Var);
            AlignToRight(23);
            ColorPicker(_("Custom name color"), Colors::NameColor, true);
            ImGui::Checkbox(_("Player Dlights"), &Vars::ESP::Players::Dlights.m_Var);

            static const char* ignoreTeammatesEsp[]{ "Off", "All", "Keep friends" };
            ImGui::Combo(_("Ignore teammates###ESPteam"), &Vars::ESP::Players::IgnoreTeammates.m_Var, ignoreTeammatesEsp, IM_ARRAYSIZE(ignoreTeammatesEsp));

            static const char* ignoreCloakedEsp[]{ "Off", "All", "Enemies only" };
            ImGui::Combo(_("Ignore cloaked###ESPcloak"), &Vars::ESP::Players::IgnoreCloaked.m_Var, ignoreCloakedEsp, IM_ARRAYSIZE(ignoreCloakedEsp));

            static const char* classEsp[]{ "Off", "Icon", "Text" };
            ImGui::Combo(_("Player class"), &Vars::ESP::Players::Class.m_Var, classEsp, IM_ARRAYSIZE(classEsp));

            static const char* uberESP[]{ "Off", "Text", "Bar" };
            ImGui::Combo(_("Player ubercharge"), &Vars::ESP::Players::Uber.m_Var, uberESP, IM_ARRAYSIZE(uberESP));

            static const char* boxESP[]{ "Off", "Simple", "Cornered", "3D" };
            ImGui::Combo(_("Player box"), &Vars::ESP::Players::Box.m_Var, boxESP, IM_ARRAYSIZE(boxESP));

            FixSlider;
            ImGui::SliderFloat(_("DLight radius"), &Vars::ESP::Players::DlightRadius.m_Var, 5.0f, 400.f, _("%.0f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);

        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(262);
        ImGui::MenuChild(_("Buildings"), ImVec2(250, 446), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5,5 });
            ImGui::SetNextWindowSize(ImVec2(160, 0));
            if (ImGui::BeginPopup(_("BuildingHealthBar"), ImGuiWindowFlags_NoScrollWithMouse)) {
                ImGui::Text(_(ICON_FA_EYE " Building Health Bar"));
                ImGui::Separator();
                ImGui::SetCursorPosX(2);
                ColorPicker2(_("Healthbar top color##2"), Colors::HealthBarbTopColor, false);
                ImGui::SetCursorPosX(2);
                ColorPicker2(_("Healthbar bottom color##2"), Colors::HealthBarbBottomColor, false);

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            ImGui::Checkbox(_("Enabled"), &Vars::ESP::Buildings::Active.m_Var);
            ImGui::Checkbox(_("Ignore team buildings###ESPbuildingsteam"), &Vars::ESP::Buildings::IgnoreTeammates.m_Var);
            ImGui::Checkbox(_("Building name"), &Vars::ESP::Buildings::Name.m_Var);
            ImGui::Checkbox(_("Custom name color"), &Vars::ESP::Buildings::NameCustom.m_Var);
            AlignToRight(23);
            ColorPicker(_("Custom name color"), Colors::BNameColor, true);
            ImGui::Checkbox(_("Building health"), &Vars::ESP::Buildings::Health.m_Var);
            ImGui::Checkbox(_("Building conditions"), &Vars::ESP::Buildings::Cond.m_Var);
            AlignToRight(23);
            ColorPicker(_("Conditions"), Colors::CondBuildings);

            ImGui::Checkbox(_("Building health bar"), &Vars::ESP::Buildings::HealthBar.m_Var);
            AlignToRight(20);
            ImGui::Text(ICON_FA_COG);
            AlignToRight(23);
            if (ImGui::InvisibleButton(_("BuildingHealthBar"), ImVec2(20, 20))) {
                ImGui::OpenPopup(_("BuildingHealthBar"), ImGuiWindowFlags_NoScrollWithMouse);
            }

            ImGui::Checkbox(_("Building level"), &Vars::ESP::Buildings::Level.m_Var);
            ImGui::Checkbox(_("Building owner"), &Vars::ESP::Buildings::Owner.m_Var);

            static const char* boxESPb[]{ "Off", "Simple", "Cornered", "3D" };
            ImGui::Combo(_("Building box"), &Vars::ESP::Buildings::Box.m_Var, boxESPb, IM_ARRAYSIZE(boxESPb));

            ImGui::Checkbox(_("Building Dlights"), &Vars::ESP::Buildings::Dlights.m_Var);
            FixSlider;
            ImGui::SliderFloat(_("Dlight radius###buildingsdlights"), &Vars::ESP::Buildings::DlightRadius.m_Var, 5.0f, 400.f, _("%.0f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(517);
        ImGui::MenuChild(_("World"), ImVec2(260, 446), false, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
        {
            const char* skyNames[] = {
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

            ImGui::Checkbox(_("Pickups ESP"), &Vars::ESP::World::Active.m_Var);
            AlignToRight(23);
            ColorPicker(_("Pickup ESP"), Colors::Weapon, false);

            ImGui::Checkbox(_("Healthpack ESP"), &Vars::ESP::World::HealthText.m_Var);
            AlignToRight(23);
            ColorPicker(_("Healthpack ESP"), Colors::Health, false);

            ImGui::Checkbox(_("Ammopack ESP"), &Vars::ESP::World::AmmoText.m_Var);
            AlignToRight(23);
            ColorPicker(_("Ammopack ESP"), Colors::Ammo, false);

            ImGui::Checkbox(_("World modulation"), &Vars::Visuals::WorldModulation.m_Var);
            AlignToRight(43);
            ColorPicker(_("World"), Colors::WorldModulation, false);
            AlignToRight(23);
            ColorPicker(_("Props"), Colors::StaticPropModulation, false);
            ImGui::Checkbox(_("Sky modulation"), &Vars::Visuals::SkyModulation.m_Var);
            AlignToRight(23);
            ColorPicker(_("Skybox"), Colors::SkyModulation, false);


            ImGui::Checkbox(_("Dev Textures"), &Vars::Visuals::DevTextures.m_Var);

            ImGui::Checkbox(_("Skybox changer"), &Vars::Visuals::SkyboxChanger.m_Var);

            // Probably a bad idea to do it like this...?
            if (ImGui::Combo(_("Skybox"), &Vars::Skybox::skyboxnum, skyNames, IM_ARRAYSIZE(skyNames), 6))
                g_Visuals.SkyboxChanger();

            if (Vars::Skybox::skyboxnum == 0) { // God damnit, this made the menu look a bit more messier :(
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.23f, 0.23f, 0.23f, 1.f));
                ImGui::SetCursorPosX(8);
                ImGui::PushItemWidth(ImGui::GetContentRegionMax().x - 16);
                if (ImGui::InputTextWithHint(_("##Custom skybox"), _("Custom skybox name"), &Vars::Skybox::SkyboxName))
                    g_Visuals.SkyboxChanger();

                ImGui::PopStyleColor(2);
            }

            ImGui::SetCursorPosX(5);
            ImGui::Text(_("Custom ESP font"));
            ImGui::SetCursorPosX(5);
            ImGui::PushItemWidth(ImGui::GetContentRegionMax().x - 10);

            if (ImGui::InputText(_("###CustomFont"), &Vars::Fart::customFont, ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string fart = Utils::str_tolower(Vars::Fart::customFont);
                //std::transform(fart.begin(), fart.end(), fart.begin(), std::tolower);
                if (Vars::Fart::customFont.c_str() == _("") || fart == _("default")) {
                    g_Draw.ReInitFonts(
                        {
                            //FONT_ESP
                            { 0x0, _("Segoe UI"), 12, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },
                            //FONT_ESP_OUTLINED
                            { 0x0, _("Segoe UI"), 12, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },

                            //FONT_ESP_NAME
                            { 0x0, _("Verdana"), 12, 0, FONTFLAG_DROPSHADOW },
                            //FONT_ESP_NAME_OUTLINED
                            { 0x0, _("Segoe UI"), 13, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS},

                            //FONT_ESP_COND
                            { 0x0, _("Segoe UI"), 12, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },
                            //FONT_ESP_COND_OUTLINED
                            { 0x0, _("Consolas"), 10, 0, FONTFLAG_OUTLINE },

                            //FONT_ESP_PICKUPS
                            { 0x0, _("Consolas"), 13, 0, FONTFLAG_NONE },
                            //FONT_ESP_PICKUPS_OUTLINED
                            { 0x0, _("Segoe UI"), 13, 100, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS},

                            //FONT_MENU
                            { 0x0, _("Verdana"), 12, 0, FONTFLAG_NONE | FONTFLAG_DROPSHADOW },
                            //FONT_MENU_OUTLINED
                            { 0x0, _("Verdana"), 12, 0, FONTFLAG_OUTLINE },

                            /*FONT_ICONS*/
                            { 0x0, _("Tf2weaponicons Regular"), 20, 0, FONTFLAG_NONE},
                        }
                    );
                }
                else {
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
            }
            ImGui::PopItemWidth();
            ImGui::Checkbox(_("Freecam"), &Vars::Misc::Freecam.m_Var);
            //ImGui::Checkbox("Autoshoot", &Vars::Aimbot::Global::AutoShoot.m_Var); // We don't really need auto shoot atm.
            AlignToRight(50);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(MenuCol.x / 1.5, MenuCol.y / 1.5, MenuCol.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(MenuCol.x, MenuCol.y, MenuCol.z, 255));
            InputKeybind(_("Freecam key"), Vars::Misc::FreecamKey, false);
            ImGui::PopStyleColor(3);
            FixSlider;
            ImGui::SliderFloat("Freecam speed", &Vars::Misc::FreecamSpeed.m_Var, 0.01f, 10.f, _("%.2f"), ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x / 1.9) - ImGui::CalcTextSize(_("ESP Tab")).x);
    ImGui::SetCursorPosY(ImGui::GetContentRegionMax().y - 22);
    ImGui::Text(_("ESP Tab"));
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetContentRegionMax().y - 25);
    ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - 43);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(MenuCol.x, MenuCol.y, MenuCol.z, 255));

    if (ImGui::Button(_(">>"), ImVec2(35, 0)))
        nESPTab = 2;
    ImGui::PopStyleColor();
}

void ESPTab2() { // Chams

    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);
        ImGui::MenuChild(_("Player Chams"), ImVec2(253, 446), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Player chams"), &Vars::Chams::Players::Active.m_Var);
            AlignToRight(23);
            ColorPicker(_("Fresnel Base"), Colors::FresnelBase, false);
            ImGui::Checkbox(_("Local chams"), &Vars::Chams::Players::ShowLocal.m_Var);
            ImGui::Checkbox(_("Chams on cosmetics"), &Vars::Chams::Players::Wearables.m_Var);
            ImGui::Checkbox(_("Chams on weapons"), &Vars::Chams::Players::Weapons.m_Var);
            ImGui::Checkbox(_("IgnoreZ"), &Vars::Chams::Players::IgnoreZ.m_Var);
            static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Fresnel" };
            ImGui::Combo(_("Player material"), &Vars::Chams::Players::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials));
            static const char* ignoreTeammatesChams[]{ "Off", "All", "Keep friends" };
            ImGui::Combo(_("Ignore teammates###chamsteam"), &Vars::Chams::Players::IgnoreTeammates.m_Var, ignoreTeammatesChams, IM_ARRAYSIZE(ignoreTeammatesChams));
            FixSlider;
            ImGui::SliderFloat(_("Player chams opacity"), &Vars::Chams::Players::Alpha.m_Var, 0.0f, 1.0f, _("%.2f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
            ImGui::Checkbox(_("Hitbox matrix"), &Vars::Chams::Players::HitboxThing.m_Var);
            AlignToRight(23);
            ColorPicker(_("Matrix color"), Colors::hitboxColor, true);
            FixSlider;
            ImGui::SliderFloat(_("Matrix time"), &Vars::Chams::Players::HitboxTimeThing.m_Var, 0.1f, 10.0f, _("%.0f"), ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(265);
        ImGui::MenuChild(_("Building Chams"), ImVec2(253, 213), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Building chams"), &Vars::Chams::Buildings::Active.m_Var);
            ImGui::Checkbox(_("Ignore team buildings###Chamsbuildingsteam"), &Vars::Chams::Buildings::IgnoreTeammates.m_Var);
            ImGui::Checkbox(_("IgnoreZ###buildingsignorez"), &Vars::Chams::Buildings::IgnoreZ.m_Var);
            static const char* pchamsbMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Fresnel" };
            ImGui::Combo(_("Building material"), &Vars::Chams::Buildings::Material.m_Var, pchamsbMaterials, IM_ARRAYSIZE(pchamsbMaterials));
            FixSlider;
            ImGui::SliderFloat(_("Building chams opacity"), &Vars::Chams::Buildings::Alpha.m_Var, 0.0f, 1.0f, _("%.2f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//left upper
        ImGui::SetCursorPosY(255);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(265);
        ImGui::MenuChild(_("DME Chams"), ImVec2(253, 233), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("DME Chams##dmeactive"), &Vars::Chams::DME::Active.m_Var);
            AlignToRight(43);
            ColorPicker(_("Hands"), Colors::Hands, false);
            AlignToRight(23);
            ColorPicker(_("Weapon"), Colors::Weapon, false);
            static const char* handsMaterial[]{
                "Original",
                "Shaded",
                "Shiny",
                "Flat",
                "Wireframe shaded",
                "Wireframe shiny",
                "Wireframe flat",
                "Fresnel"
            };
            ImGui::Combo(_("Arm material"), &Vars::Chams::DME::Hands.m_Var, handsMaterial, IM_ARRAYSIZE(handsMaterial));
            FixSlider;
            ImGui::SliderFloat(_("Arm opacity"), &Vars::Chams::DME::HandsAlpha.m_Var, 0.0f, 1.0f, _("%.2f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);

            static const char* weaponMaterial[]{
                "Original",
                "Shaded",
                "Shiny",
                "Flat",
                "Wireframe shaded",
                "Wireframe shiny",
                "Wireframe flat",
                "Fresnel"
            };
            ImGui::Combo(_("Weapon material"), &Vars::Chams::DME::Weapon.m_Var, weaponMaterial, IM_ARRAYSIZE(weaponMaterial));
            FixSlider;
            ImGui::SliderFloat(_("Weapon opacity"), &Vars::Chams::DME::WeaponAlpha.m_Var, 0.0f, 1.0f, _("%.2f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(524);
        ImGui::MenuChild(_("World Chams"), ImVec2(253, 446), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Pickup chams"), &Vars::Chams::World::Active.m_Var);
            ImGui::Checkbox(_("Healthpack chams"), &Vars::Chams::World::Health.m_Var);
            ImGui::Checkbox(_("Ammopack chams"), &Vars::Chams::World::Ammo.m_Var);
            ImGui::Checkbox(_("IgnoreZ###pickupsignorez"), &Vars::Chams::World::IgnoreZ.m_Var);
            static const char* projectilesTeam[]{ "Off", "All", "Enemy Only" };
            ImGui::Combo(_("Projectile chams"), &Vars::Chams::World::Projectiles.m_Var, projectilesTeam, IM_ARRAYSIZE(projectilesTeam));
            static const char* pchamspMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Fresnel" };
            ImGui::Combo(_("Pickup chams material"), &Vars::Chams::World::Material.m_Var, pchamspMaterials, IM_ARRAYSIZE(pchamspMaterials));
            FixSlider;
            ImGui::SliderFloat(_("Pickup chams opacity"), &Vars::Chams::World::Alpha.m_Var, 0.0f, 1.0f, _("%.2f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::SetCursorPosX(7);
    ImGui::SetCursorPosY(ImGui::GetContentRegionMax().y - 25);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(MenuCol.x, MenuCol.y, MenuCol.z, 255));

    if (ImGui::Button(_("<<"), ImVec2(35, 0)))
        nESPTab = 1;
    ImGui::SameLine();

    ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x / 1.9) - ImGui::CalcTextSize(_("Chams Tab")).x);
    ImGui::Text(_("Chams Tab"));

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - 43);

    if (ImGui::Button(_(">>"), ImVec2(35, 0)))
        nESPTab = 3;
    ImGui::PopStyleColor();
}

void ESPTab3() {
    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);
        ImGui::MenuChild(_("Player Glow"), ImVec2(253, 446), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            FixSlider;
            ImGui::Checkbox(_("Wireframe glow"), &Vars::Glow::Main::Wireframe.m_Var);
            if (!Vars::Glow::Main::Wireframe.m_Var)
            {
                ImGui::SliderInt("Global glow scale", &Vars::Glow::Main::Scale.m_Var, 1, 10, "%d", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
            }
            ImGui::Checkbox(_("Player glow"), &Vars::Glow::Players::Active.m_Var);
            ImGui::Checkbox(_("Show local"), &Vars::Glow::Players::ShowLocal.m_Var);
            static const char* ignoreTeammatesGlow[]{ "Off", "All", "Keep friends" };
            ImGui::Combo(_("Ignore teammates###glowteam"), &Vars::Glow::Players::IgnoreTeammates.m_Var, ignoreTeammatesGlow, IM_ARRAYSIZE(ignoreTeammatesGlow));
            ImGui::Checkbox(_("Glow on cosmetics"), &Vars::Glow::Players::Wearables.m_Var);
            ImGui::Checkbox(_("Glow on weapons"), &Vars::Glow::Players::Weapons.m_Var);
            FixSlider;
            ImGui::SliderFloat(_("Player glow opacity"), &Vars::Glow::Players::Alpha.m_Var, 0.1f, 1.0f, _("%.2f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
            static const char* colorGlow[]{ "Team", "Health" };
            ImGui::Combo(_("Player glow color"), &Vars::Glow::Players::Color.m_Var, colorGlow, IM_ARRAYSIZE(colorGlow));
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(265);
        ImGui::MenuChild(_("Building Glow"), ImVec2(253, 446), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Building glow"), &Vars::Glow::Buildings::Active.m_Var);
            ImGui::Checkbox(_("Ignore team buildings###glowbuildingsteam"), &Vars::Glow::Buildings::IgnoreTeammates.m_Var);
            //static const char* ignoreTeammatesGlow[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###glowteam", &Vars::Glow::Buildings::IgnoreTeammates.m_Var, ignoreTeammatesGlow, IM_ARRAYSIZE(ignoreTeammatesGlow)); ImGui::PopItemWidth(); HelpMarker("Which teammates the glow will ignore drawing on");
            FixSlider;
            ImGui::SliderFloat(_("Building glow opacity"), &Vars::Glow::Buildings::Alpha.m_Var, 0.1f, 1.0f, _("%.2f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
            static const char* colorGlow[]{ "Team", "Health" };
            ImGui::Combo(_("Building glow color"), &Vars::Glow::Buildings::Color.m_Var, colorGlow, IM_ARRAYSIZE(colorGlow));
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(524);
        ImGui::MenuChild(_("World Glow"), ImVec2(253, 446), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Pickup glow"), &Vars::Glow::World::Active.m_Var);
            ImGui::Checkbox(_("Healthpack glow"), &Vars::Glow::World::Health.m_Var);
            ImGui::Checkbox(_("Ammopack glow"), &Vars::Glow::World::Ammo.m_Var);
            static const char* projectilesgTeam[]{ "Off", "All", "Enemy Only" };
            ImGui::Combo(_("Projectile glow"), &Vars::Glow::World::Projectiles.m_Var, projectilesgTeam, IM_ARRAYSIZE(projectilesgTeam));
            FixSlider;
            ImGui::SliderFloat(_("Pickup glow opacity"), &Vars::Glow::World::Alpha.m_Var, 0.1f, 1.0f, _("%.2f"), ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::SetCursorPosX(7);
    ImGui::SetCursorPosY(ImGui::GetContentRegionMax().y - 25);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(MenuCol.x, MenuCol.y, MenuCol.z, 255));
    if (ImGui::Button(_("<<"), ImVec2(35, 0)))
        nESPTab = 2;
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x / 1.9) - ImGui::CalcTextSize(_("Glow Tab")).x);
    ImGui::Text(_("Glow Tab"));

}

void VisualsTab() {

    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);
        ImGui::MenuChild(_("Local"), ImVec2(250, 470), false, ImGuiWindowFlags_NoScrollWithMouse);
        {

            FixSlider;
            ImGui::SliderInt(_("Field of view"), &Vars::Visuals::FieldOfView.m_Var, 70, 150, _("%d"), ImGuiSliderFlags_AlwaysClamp);
            FixSlider;
            ImGui::SliderFloat(_("Aspect Ratio"), &Vars::Visuals::AspectRatioValue.m_Var, 0.f, 200.f, _("%.f"), ImGuiSliderFlags_AlwaysClamp);
            FixSlider;
            ImGui::SliderInt(_("Aim FOV Alpha"), &Vars::Visuals::AimFOVAlpha.m_Var, 0, 255, _("%d"), ImGuiSliderFlags_AlwaysClamp);
            static char buff[12];
            snprintf(buff, sizeof(buff), _("%i, %i, %i"), Vars::Visuals::ViewModelX.m_Var, Vars::Visuals::ViewModelY.m_Var, Vars::Visuals::ViewModelZ.m_Var);
            if (ImGui::BeginCombo(_("Viewmodel XYZ"), _(buff), 0, true))
            {
                FixSlider;
                ImGui::SliderInt(_("Viewmodel X"), &Vars::Visuals::ViewModelX.m_Var, -25, 25, _("%d"), ImGuiSliderFlags_AlwaysClamp);
                FixSlider;
                ImGui::SliderInt(_("Viewmodel Y"), &Vars::Visuals::ViewModelY.m_Var, -25, 25, _("%d"), ImGuiSliderFlags_AlwaysClamp);
                FixSlider;
                ImGui::SliderInt(_("Viewmodel Z"), &Vars::Visuals::ViewModelZ.m_Var, -25, 25, _("%d"), ImGuiSliderFlags_AlwaysClamp);
                ImGui::EndCombo();
            }
            ImGui::Checkbox(_("Aimbot viewmodel"), &Vars::Visuals::AimbotViewmodel.m_Var);
            ImGui::Checkbox(_("Aimbot crosshair"), &Vars::Visuals::CrosshairAimPos.m_Var);
            ImGui::Checkbox(_("Remove scope"), &Vars::Visuals::RemoveScope.m_Var);
            ImGui::Checkbox(_("Remove zoom"), &Vars::Visuals::RemoveZoom.m_Var);
            ImGui::Checkbox(_("Remove recoil"), &Vars::Visuals::RemovePunch.m_Var);
            ImGui::Checkbox(_("Chat info"), &Vars::Visuals::ChatInfo.m_Var);
            ImGui::Checkbox(_("Vote revealer"), &Vars::Misc::VoteRevealer.m_Var);
            ImGui::Checkbox(_("Clean screenshots"), &Vars::Misc::CleanScreenshot.m_Var);
            static const char* weaponTracer[]{
                 "None",
                 "Machina",
                 "Capper",
                 "Merasmus Vortex",
                 "Merasmus Zap",
            };
            ImGui::Combo(_("Bullet Tracer"), &Vars::Visuals::TracerEffect.m_Var, weaponTracer, IM_ARRAYSIZE(weaponTracer));
            static const char* ragdollEffect[]{
                "None",
                "Gib",
                "Burning",
                "Feign death",
                "Become ash",
                "Gold ragdoll",
                "Ice ragdoll",
            };
            ImGui::Combo(_("Ragdoll effect"), &Vars::Visuals::RagdollEffect.m_Var, ragdollEffect, IM_ARRAYSIZE(ragdollEffect));
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(262);
        ImGui::MenuChild(_("Spywarning"), ImVec2(250, 195), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Active###spywarn"), &Vars::Visuals::SpyWarning.m_Var);
            ImGui::Checkbox(_("Voice command###spywarn1"), &Vars::Visuals::SpyWarningAnnounce.m_Var);
            ImGui::Checkbox(_("Visible only###spywarn2"), &Vars::Visuals::SpyWarningVisibleOnly.m_Var);
            ImGui::Checkbox(_("Ignore friends###spywarn3"), &Vars::Visuals::SpyWarningIgnoreFriends.m_Var);
            const char* spyWmodes[]{ "Arrow", "Flash" };
            ImGui::Combo(_("Warning style"), &Vars::Visuals::SpyWarningStyle.m_Var, spyWmodes, IM_ARRAYSIZE(spyWmodes));
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//left upper
        ImGui::SetCursorPosY(237);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(262);
        ImGui::MenuChild(_("Thirdperson"), ImVec2(250, 275), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Thirdperson"), &Vars::Visuals::ThirdPerson.m_Var);
            AlignToRight(50);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(MenuCol.x / 1.5, MenuCol.y / 1.5, MenuCol.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(MenuCol.x, MenuCol.y, MenuCol.z, 255));
            InputKeybind(_("Thirdperson key"), Vars::Visuals::ThirdPersonKey);
            ImGui::PopStyleColor(3);
            ImGui::Checkbox(_("Show silent angles"), &Vars::Visuals::ThirdPersonSilentAngles.m_Var);
            ImGui::Checkbox(_("Instant yaw"), &Vars::Visuals::ThirdPersonInstantYaw.m_Var);
            FixSlider;
            ImGui::SliderFloat(_("Forward offset"), &Vars::Visuals::ThirdpersonOffsetX.m_Var, -100.0f, 100.f, _("%.0f"), ImGuiSliderFlags_AlwaysClamp);
            FixSlider;
            ImGui::SliderFloat(_("Right offset"), &Vars::Visuals::ThirdpersonOffsetY.m_Var, -100.0f, 100.f, _("%.0f"), ImGuiSliderFlags_AlwaysClamp);
            FixSlider;
            ImGui::SliderFloat(_("Up offset"), &Vars::Visuals::ThirdpersonOffsetZ.m_Var, -100.0f, 100.f, _("%.0f"), ImGuiSliderFlags_AlwaysClamp);
            ImGui::Checkbox(_("Thirdperson crosshair"), &Vars::Visuals::ThirdpersonCrosshair.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//right upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(517);
        ImGui::MenuChild(_("Radar"), ImVec2(260, 470), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Active###radar"), &Vars::Radar::Main::Active.m_Var);
            ImGui::Checkbox(_("Players###radarp"), &Vars::Radar::Players::Active.m_Var);
            ImGui::Checkbox(_("Buildings###radarbuildingsa"), &Vars::Radar::Buildings::Active.m_Var);
            FixSlider;
            ImGui::SliderInt(_("Radar size"), &Vars::Radar::Main::Size.m_Var, 20, 200, _("%d"), ImGuiSliderFlags_AlwaysClamp);
            FixSlider;
            ImGui::SliderInt(_("Radar range"), &Vars::Radar::Main::Range.m_Var, 50, 3000, _("%d"), ImGuiSliderFlags_AlwaysClamp);
            FixSlider;
            ImGui::SliderInt(_("Radar BG alpha"), &Vars::Radar::Main::BackAlpha.m_Var, 0, 255, _("%d"), ImGuiSliderFlags_AlwaysClamp);

            ImGui::Checkbox(_("Player Health bar###radarhealt"), &Vars::Radar::Players::Health.m_Var);
            ImGui::Checkbox(_("Buildings Health bar###radarbuildingsc"), &Vars::Radar::Buildings::Health.m_Var);
            const char* iconPlayersRadar[]{ "Scoreboard", "Portraits", "Avatar" };
            ImGui::Combo(_("Icon###radari"), &Vars::Radar::Players::IconType.m_Var, iconPlayersRadar, IM_ARRAYSIZE(iconPlayersRadar));
            static const char* ignoreTeammatespRadar[]{ "Off", "All", "Keep friends" };
            ImGui::Combo(_("Ignore teammates###radarplayersteam"), &Vars::Radar::Players::IgnoreTeam.m_Var, ignoreTeammatespRadar, IM_ARRAYSIZE(ignoreTeammatespRadar));
            ImGui::Checkbox(_("Ignore team building###radarbuildingsb"), &Vars::Radar::Buildings::IgnoreTeam.m_Var); //This feature is dumb anyway
            ImGui::TextUnformatted(_(""));
            ImGui::Checkbox(_("Out of FoV arrows###arrows"), &Vars::ESP::Players::Arrows::Active.m_Var);
            FixSlider;
            ImGui::SliderFloat(_("Distance from center"), &Vars::ESP::Players::Arrows::DistFromCenter.m_Var, 20.f, 200.f, _("%.2f"), ImGuiSliderFlags_AlwaysClamp);
            FixSlider;
            ImGui::SliderFloat(_("Arrow size"), &Vars::ESP::Players::Arrows::ArrowSize.m_Var, 5, 30, _("%.0fpx"), ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
}

void MiscTab() {
    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);
        ImGui::MenuChild(_("General"), ImVec2(300, 265), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Anti AFK"), &Vars::Misc::AntiAFK.m_Var);
            ImGui::Checkbox(_("Bypass sv_pure"), &Vars::Misc::BypassPure.m_Var);
            ImGui::Checkbox(_("Medal flip"), &Vars::Misc::MedalFlip.m_Var);
            ImGui::Checkbox(_("Noisemaker spam"), &Vars::Misc::NoisemakerSpam.m_Var);
            ImGui::Checkbox(_("Chat Spam"), &Vars::Misc::ChatSpam.m_Var);
            ImGui::Checkbox(_("No Interp"), &Vars::Misc::DisableInterpolation.m_Var);
            ImGui::Checkbox(_("Remove Hats"), &Vars::Visuals::RemoveHats.m_Var);
            ImGui::Checkbox(_("PlayerList"), &Vars::Visuals::PlayerList.m_Var);
            ImGui::Checkbox(_("Steam RPC"), &Vars::Misc::SteamRPC.m_Var);
            ImGui::SameLine();
            ImGui::InputTextWithHint(_("##customsteamrpc"), _("Custom Steam RPC"), &Vars::Misc::SteamRPCText);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//left bottom
        ImGui::SetCursorPosY(307);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);
        ImGui::MenuChild(_("Movement"), ImVec2(300, 205), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::Checkbox(_("Bhop"), &Vars::Misc::AutoJump.m_Var);
            ImGui::Checkbox(_("AutoStrafer"), &Vars::Misc::AutoStrafe.m_Var);
            ImGui::Checkbox(_("Auto RocketJump"), &Vars::Misc::AutoRocketJump.m_Var);
            ImGui::Checkbox(_("Taunt Slide"), &Vars::Misc::TauntSlide.m_Var);
            ImGui::Checkbox(_("Taunt Control"), &Vars::Misc::TauntControl.m_Var);
            ImGui::Checkbox(_("Mouse control"), &Vars::Misc::TauntControlMouse.m_Var);
            ImGui::Checkbox(_("No Push"), &Vars::Misc::NoPush.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//right
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(312);
        ImGui::MenuChild(_("HvH (autism)"), ImVec2(300, 300), false, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5,5 });
            ImGui::SetNextWindowSize(ImVec2(210, 0));
            if (ImGui::BeginPopup(_("FakeLagSettings"), ImGuiWindowFlags_NoScrollWithMouse)) {
                ImGui::Text(_(ICON_FA_EYE " Fakelag settings"));
                ImGui::Separator();
                ImGui::SetCursorPosX(2);
                ImGui::PushItemWidth(200);
                FixSlider;
                ImGui::SliderInt(_("Fakelag value"), &Vars::Misc::CL_Move::FakelagValue.m_Var, 1, 14, _("%d"), ImGuiSliderFlags_AlwaysClamp);
                ImGui::Checkbox(_("Fakelag on key"), &Vars::Misc::CL_Move::FakelagOnKey.m_Var);
                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            ImGui::Checkbox(_("AntiAim"), &Vars::AntiHack::AntiAim::Active.m_Var);
            const char* pitch[]{ "None", "Up", "Down", "Fake up", "Fake down", "Center", "Half Up" }; ImGui::Combo(_("Pitch"), &Vars::AntiHack::AntiAim::Pitch.m_Var, pitch, IM_ARRAYSIZE(pitch));
            const char* realYaw[]{ "None", "Left", "Right", "Backwards", "Spin", "Random", "Edge", "Static"}; ImGui::Combo(_("Real yaw"), &Vars::AntiHack::AntiAim::YawReal.m_Var, realYaw, IM_ARRAYSIZE(realYaw));
            const char* fakeYaw[]{ "None", "Left", "Right", "Backwards", "Spin", "Random", "Edge", "Static"}; ImGui::Combo(_("Fake yaw"), &Vars::AntiHack::AntiAim::YawFake.m_Var, fakeYaw, IM_ARRAYSIZE(fakeYaw));
            FixSlider;
            ImGui::SliderInt(_("Spin Speed"), &Vars::AntiHack::AntiAim::SpinSpeed.m_Var, 1, 20, _("%d"), ImGuiSliderFlags_AlwaysClamp);

            ImGui::Checkbox(_("Fakelag"), &Vars::Misc::CL_Move::Fakelag.m_Var);
            AlignToRight(70);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(MenuCol.x / 1.5, MenuCol.y / 1.5, MenuCol.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(MenuCol.x, MenuCol.y, MenuCol.z, 255));
            InputKeybind(_("Fakelag key"), Vars::Misc::CL_Move::FakelagKey);
            ImGui::PopStyleColor(3);

            AlignToRight(20);
            ImGui::Text(ICON_FA_COG);
            AlignToRight(23);
            if (ImGui::InvisibleButton(_("FakeLagSettings"), ImVec2(20, 20))) {
                ImGui::OpenPopup(_("FakeLagSettings"), ImGuiWindowFlags_NoScrollWithMouse);
            }

            //ImGui::Checkbox(_("Fakelag on key"), &Vars::Misc::CL_Move::FakelagOnKey.m_Var);
            //FixSlider;
            //ImGui::SliderInt(_("Fakelag value"), &Vars::Misc::CL_Move::FakelagValue.m_Var, 1, 14, _("%d"), ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//right
        ImGui::SetCursorPosY(342);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(312);
        ImGui::MenuChild(_("Exploits"), ImVec2(300, 169), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8,8 });
            ImGui::SetNextWindowSize(ImVec2(210, 0));
            if (ImGui::BeginPopup(_("DTSettings"), ImGuiWindowFlags_NoScrollWithMouse)) {
                ImGui::Text(_(ICON_FA_EYE " Doubletap settings"));
                ImGui::Separator();
                ImGui::PushItemWidth(200);
                ImGui::SetCursorPosX(2);
                ImGui::SliderInt(_("Ticks to shift"), &Vars::Misc::CL_Move::DTTicks.m_Var, 10, 24, _("%d"), ImGuiSliderFlags_ClampOnInput);
                ImGui::SetCursorPosX(2);
                ImGui::Checkbox(_("Wait for DT"), &Vars::Misc::CL_Move::WaitForDT.m_Var);
                ImGui::SetCursorPosX(2);
                ImGui::Checkbox(_("Don't DT in air"), &Vars::Misc::CL_Move::NotInAir.m_Var);
                ImGui::SetCursorPosX(8);
                ImGui::Text(_("Recharge Key"));
                AlignToRight(45);
                InputKeybind(_("Recharge Key"), Vars::Misc::CL_Move::RechargeKey);
                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            ImGui::Checkbox(_("DoubleTap"), &Vars::Misc::CL_Move::Doubletap.m_Var);
            AlignToRight(70);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(MenuCol.x / 1.5, MenuCol.y / 1.5, MenuCol.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(MenuCol.x, MenuCol.y, MenuCol.z, 255));
            InputKeybind(_("DoubleTap Key"), Vars::Misc::CL_Move::DoubletapKey);
            ImGui::PopStyleColor(3);

            AlignToRight(20);
            ImGui::Text(ICON_FA_COG);
            AlignToRight(23);
            if (ImGui::InvisibleButton(_("DTSettings"), ImVec2(20, 20))) {
                ImGui::OpenPopup(_("DTSettings"));
            }

            ImGui::Checkbox(_("Instant Respawn MVM"), &Vars::Misc::InstantRespawn.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
}

//char buffer[256];

void ConfigsTab() {
    {//left upper
        ImGui::SetCursorPosY(42);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(7);
        ImGui::MenuChild(_("Configs"), ImVec2(250, 445), false, ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::SetCursorPosX(5);

            static std::wstring selected = {};
            int nConfig = 0;
            /*
            for (const auto& entry : std::filesystem::directory_iterator(g_CFG.m_sConfigPath)) {
                if (std::string(std::filesystem::path(entry).filename().string()).find(_(".CAM")) == std::string_view::npos)
                {
                    continue;
                }
                nConfig++;
            }
            */
            for (const auto& entry : std::filesystem::directory_iterator(g_CFG.m_sConfigPath)) {
                if (std::string(std::filesystem::path(entry).filename().string()).find(_(".poop")) == std::string_view::npos) {
                    continue;
                }
                nConfig++;
                std::wstring s = entry.path().filename().wstring();
                s.erase(s.end() - 4, s.end());

                std::string configName(s.begin(), s.end());

                if (ImGui::ListBoxHeader(_("##cfglist"), ImVec2(-5, -130))) {
                    if (ImGui::Selectable(configName.c_str(), s == selected))
                        selected = s;

                    ImGui::ListBoxFooter();
                }
                nConfig++;
            }

            if (nConfig < 100) {
                std::string output = {};

                ImGui::SetCursorPosY(320);
                ImGui::SetCursorPosX(5);
                ImGui::PushItemWidth(ImGui::GetContentRegionMax().x - 10);
                if (ImGui::InputText(_("###configname"), &output, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    std::wstring outstring(output.begin(), output.end());
                    if (!std::filesystem::exists(g_CFG.m_sConfigPath + L"\\" + outstring)) {

                        g_CFG.Save(outstring.c_str());
                    }
                }
                ImGui::PopItemWidth();
            }

            ImGui::SetCursorPosX(5);

            if (ImGui::Button(_("Save"), { ImGui::GetContentRegionMax().x - 10, 0 })) {
                g_CFG.Save(selected.c_str());
                selected.clear();
                g_Visuals.AddToEventLog(_("Config %s saved!"), selected.data());
            }
            ImGui::SetCursorPosX(5);
            if (ImGui::Button(_("Load"), { ImGui::GetContentRegionMax().x - 10, 0 })) {
                g_CFG.Load(selected.c_str());
                selected.clear();
                std::string fart2 = Utils::str_tolower(Vars::Fart::customFont);
                if (Vars::Fart::customFont.c_str() == "" || fart2 == "default") {
                    g_Draw.ReInitFonts(
                        {
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
                        }
                    );
                }
                else {
                    g_Draw.ReInitFonts(
                        {
                            //FONT_ESP
                            { 0x0, _("runescape uf"), 12, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS },
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
                        }
                    );
                }
                g_Visuals.AddToEventLog(_("Config %s loaded!"), selected.data());
            }
            ImGui::SetCursorPosX(5);
            if (ImGui::Button(_("Delete"), { ImGui::GetContentRegionMax().x - 10, 0 })) {
                g_CFG.Remove(selected.c_str());
                selected.clear();
                g_Visuals.AddToEventLog(_("Config %s deleted!"), selected.data());
            }
            ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(_("Note: Press enter to create a new config.")).x) * 0.5f);
            ImGui::Text(_("Note: Press enter to create a new config."));
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::SameLine();
    //ImGui::SetCursorPosY(55);
    ImGui::BeginGroup();
    ImGui::MenuChild(_("Lua"), ImVec2(250, 445), false, ImGuiWindowFlags_NoScrollWithMouse);
    {
        ImGui::SetCursorPosX(5);
        ImGui::Text("Hopefully soon :tm:");
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    ImGui::SameLine();
    //ImGui::SetCursorPosY(55);
    ImGui::BeginGroup();
    ImGui::MenuChild(_("Others"), ImVec2(250, 445), false, ImGuiWindowFlags_NoScrollWithMouse);
    {
        ImGui::SetCursorPosX(5);
        ColorPicker2(_("Menu Color"), Vars::Menu::Colors::WidgetActive, false);
        ImGui::SetCursorPosX(5);
        ColorPicker2(_("Shadow Color"), Vars::Menu::Colors::ShadowColor, true);
        ImGui::SetCursorPosX(5);
        ImGui::SliderInt("Menu shadow size", &Vars::Menu::ShadowSize, 0, 1000, "%d", ImGuiSliderFlags_AlwaysClamp);
        ImGui::EndChild();
        ImGui::EndGroup();
    }
}

void ReplaceSpecials(std::string& str)
{
    int val;
    size_t c = 0, len = str.size();
    for (int i = 0; i + c < len; ++i)
    {
        str[i] = str[i + c];
        if (str[i] != '\\')
            continue;
        if (i + c + 1 == len)
            break;
        switch (str[i + c + 1])
        {
            // Several control characters
        case 'b':
            ++c;
            str[i] = '\b';
            break;
        case 'n':
            ++c;
            str[i] = '\n';
            break;
        case 'v':
            ++c;
            str[i] = '\v';
            break;
        case 'r':
            ++c;
            str[i] = '\r';
            break;
        case 't':
            ++c;
            str[i] = '\t';
            break;
        case 'f':
            ++c;
            str[i] = '\f';
            break;
        case 'a':
            ++c;
            str[i] = '\a';
            break;
        case '\\':
            ++c;
            break;
            // Convert specified value from HEX
        case 'x':
            if (i + c + 4 > len)
                continue;
            std::sscanf(&str[i + c + 2], "%02X", &val);
            c += 3;
            str[i] = val;
            break;
            // Convert from unicode
        case 'u':
            if (i + c + 6 > len)
                continue;
            // 1. Scan 16bit HEX value
            std::sscanf(&str[i + c + 2], "%04X", &val);
            c += 5;
            // 2. Convert value to UTF-8
            if (val <= 0x7F)
            {
                str[i] = val;
            }
            else if (val <= 0x7FF)
            {
                str[i] = 0xC0 | ((val >> 6) & 0x1F);
                str[i + 1] = 0x80 | (val & 0x3F);
                ++i;
                --c;
            }
            else
            {
                str[i] = 0xE0 | ((val >> 12) & 0xF);
                str[i + 1] = 0x80 | ((val >> 6) & 0x3F);
                str[i + 2] = 0x80 | (val & 0x3F);
                i += 2;
                c -= 2;
            }
            break;
        }
    }
    str.resize(len - c);
}

void Handle()
{
    if (!g_Menu.menuOpen && ImGui::GetStyle().Alpha > 0.f)
    {
        float fc = 255.f / 0.2f * ImGui::GetIO().DeltaTime;
        ImGui::GetStyle().Alpha = std::clamp(ImGui::GetStyle().Alpha - fc / 255.f, 0.f, 0.9f);
    }

    if (g_Menu.menuOpen && ImGui::GetStyle().Alpha < 1.f)
    {
        float fc = 255.f / 0.2f * ImGui::GetIO().DeltaTime;
        ImGui::GetStyle().Alpha = std::clamp(ImGui::GetStyle().Alpha + fc / 255.f, 0.f, 0.9f);
    }
}
//
IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
std::vector<float> frames;
static auto s2 = ImVec2{};
static auto p2 = ImVec2{};

Color_t GetTeamColor(int nTeamNum)
{
    switch (nTeamNum)
    {
    case 2: return Colors::TeamRed;
    case 3: return Colors::TeamBlu;
    default: return Colors::White;
    }
}

void CMenu::Render(IDirect3DDevice9* pDevice) {
    static bool bInitImGui = false;
    static bool bColumnsWidthened = false;
    bool modified_custom_style = false;

    //fix drawing without calling engine functons/cl_showpos
    pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
    //removes the source engine color correction
    pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    static LPDIRECT3DTEXTURE9 lg = nullptr;
    if (!bInitImGui) {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(FindWindowA(0, _("Team Fortress 2")));
        ImGui_ImplDX9_Init(pDevice);

        auto& io = ImGui::GetIO();
        io.IniFilename = NULL;
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange | ImGuiConfigFlags_NavEnableKeyboard;
        ImFontConfig font_config;
        font_config.OversampleH = 1;
        font_config.OversampleV = 1;
        font_config.PixelSnapH = 1;
        font_config.FontDataOwnedByAtlas = false;

        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x044F, // Cyrillic
            0,
        };
        name = io.Fonts->AddFontFromMemoryTTF((void*)MuseoFont, sizeof(MuseoFont), 19.0f, &font_config, ranges);
        font = io.Fonts->AddFontFromMemoryTTF((void*)MuseoFont, sizeof(MuseoFont), 13.0f, &font_config, ranges);

        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.FontDataOwnedByAtlas = false;
        io.Fonts->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), 14.0f, &icons_config, icons_ranges);
        bInitImGui = true;
    }

    if (GetAsyncKeyState(VK_INSERT) & 1) { // Can we please fix this in WndProcHook...? :(
        g_Interfaces.Surface->SetCursorAlwaysVisible(menuOpen = !menuOpen);
        g_Menu.flTimeOnChange = g_Interfaces.Engine->Time();
    }

    g_Menu.m_flFadeElapsed = g_Interfaces.Engine->Time() - g_Menu.flTimeOnChange;

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    ImGui::GetIO().MouseDrawCursor = menuOpen;
    Handle();
    if (menuOpen)
    {
        // I know it's probably a bad idea to do these under a loop but oh well you got to sacrifice some stuff sometimes just to get the perfect result.
        MenuCol = mColor(Vars::Menu::Colors::WidgetActive);
        ShadowCol = mColor(Vars::Menu::Colors::ShadowColor);
        CustomStyle();

        //ImGui::GetBackgroundDrawList()->AddRectFilled({ 0.f,0.f }, { (float)g_ScreenSize.w,(float)g_ScreenSize.h }, ImColor(0.5f, 0.5f, 0.5f));
        //ImGui::ShowDemoWindow();
        ImGui::PushStyleColor(ImGuiCol_Separator, MenuCol);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
        ImGui::PushFont(font);
        {
            ImGui::Begin(_("##console"), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
            {
                auto draw = ImGui::GetWindowDrawList();

                MenuCol = ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg];
                s2 = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2);
                p2 = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y);
                ImGui::GetBackgroundDrawList()->AddRect({ p2.x,p2.y }, { p2.x + s2.x, p2.y + s2.y }, ImColor(15, 17, 19, 255));
                draw->AddRectFilled(p2, ImVec2(p2.x + s2.x, p2.y + s2.y + 25), ImColor(15, 17, 19, 200), 0); // Titlebar
                draw->AddRectFilled(ImVec2(p2.x, p2.y + 25), ImVec2(p2.x + s2.x, p2.y + s2.y), ImColor(18, 20, 21, 255), 0, ImDrawCornerFlags_Bot); // Background
                draw->AddRectFilled(ImVec2(p2.x, p2.y + 24), ImVec2(p2.x + s2.x, p2.y + 25), ImColor(MenuCol)); // Line
                draw->AddText(ImVec2(p2.x + 15 / 2, p2.y + 13 / 2), ImColor(255, 255, 255, 255), _(ICON_FA_COG "  Console"));
            }

            ImGui::SetCursorPosY(32);
            if (ImGui::Button(_("Force SV_Cheats"), ImVec2(120, 20))) {
                ConVar* sv_cheats = g_Interfaces.CVars->FindVar("sv_cheats");
                sv_cheats->SetValue(1);
                Vars::Misc::CheatsBypass.m_Var = 1;
            }

            if (ImGui::Button(_("CL_FullUpdate"), ImVec2(120, 20)))
                g_Interfaces.Engine->ClientCmd_Unrestricted(_("cl_fullupdate"));

            if (ImGui::Button(_("SND_Restart"), ImVec2(120, 20)))
                g_Interfaces.Engine->ClientCmd_Unrestricted(_("snd_restart"));

            if (ImGui::Button(_("StopSound"), ImVec2(120, 20)))
                g_Interfaces.Engine->ClientCmd_Unrestricted(_("stopsound"));

            if (ImGui::Button(_("Status"), ImVec2(120, 20)))
                g_Interfaces.Engine->ClientCmd_Unrestricted(_("status"));

            if (ImGui::Button(_("Ping"), ImVec2(120, 20)))
                g_Interfaces.Engine->ClientCmd_Unrestricted(_("ping"));

            if (ImGui::Button(_("Retry"), ImVec2(120, 20)))
                g_Interfaces.Engine->ClientCmd_Unrestricted(_("retry"));

            ImGui::Dummy(ImVec2(0, 8));
            ImGui::End();
        }

        if (Vars::Visuals::PlayerList.m_Var) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(100, 0));
            ImGui::Begin(_("##PLIST"), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
            {
                ImGui::PopStyleVar();
                auto draw = ImGui::GetWindowDrawList();

                MenuCol = ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg];
                s2 = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2);
                p2 = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y);
                ImGui::GetBackgroundDrawList()->AddRect({ p2.x,p2.y }, { p2.x + s2.x, p2.y + s2.y }, ImColor(15, 17, 19, 255));
                draw->AddRectFilled(p2, ImVec2(p2.x + s2.x, p2.y + s2.y + 25), ImColor(15, 17, 19, 200), 5); // Titlebar
                draw->AddRectFilled(ImVec2(p2.x, p2.y + 25), ImVec2(p2.x + s2.x, p2.y + s2.y), ImColor(18, 20, 21, 200), 5, ImDrawCornerFlags_Bot); // Background
                draw->AddRectFilled(ImVec2(p2.x, p2.y + 24), ImVec2(p2.x + s2.x, p2.y + 25), ImColor(MenuCol)); // Line
                draw->AddText(ImVec2(p2.x + 15 / 2, p2.y + 13 / 2), ImColor(255, 255, 255, 255), _(ICON_FA_USER "  Playerlist"));
            }

            ImGui::SetCursorPosY(32);
            if (g_Interfaces.Engine->IsInGame() && !g_Playerlist.players.empty()) {
                if (ImGui::BeginTable(_("Playerlist"), 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                //PlayerInfo_t playerInfo{};

                    ImGui::TableSetupColumn(_("PlayerName"));
                    //ImGui::TableSetupColumn(_("ProfileButton"));
                    ImGui::TableSetupColumn(_("KickButton"));
                    ImGui::TableSetupColumn(_("IgnoreToggle"));

                    for (const auto& Player : g_Playerlist.players)
                    {
                        if (!Player.info.userID) {
                            continue;
                        }

                        bool bIgnored = g_Playerlist.IsIgnored(Player.info.friendsID);

                        ImGui::TableNextRow();

                        for (int column = 0; column < 3; column++)
                        {
                            ImGui::TableSetColumnIndex(column);
                            /*if (column == 0)
                            {
                                Color_t playerColor = Utils::GetTeamColor(Player.teamNum);
                                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(playerColor.r, playerColor.g, playerColor.b, playerColor.a));
                                ImGui::Text(_("%s"), Player.info.name);
                                ImGui::PopStyleColor();
                            }*/
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
                            ImGui::PushID(Player.info.userID);
                            {
                                if (column == 0)
                                {
                                    Color_t playerColor = Utils::GetTeamColor(Player.teamNum);
                                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(playerColor.r, playerColor.g, playerColor.b, playerColor.a));
                                    if (ImGui::Button(_(Player.info.name))) {
                                        g_SteamInterfaces.Friends015->ActivateGameOverlayToUser(_("steamid"), CSteamID((UINT64)(0x0110000100000000ULL + Player.info.friendsID)));
                                    }
                                    ImGui::PopStyleColor();
                                }
                                if (column == 1)
                                {
                                    if (ImGui::Button(_("Kick"))) {
                                        static char buff[256];
                                        snprintf(buff, sizeof(buff), _("callvote kick %d"), (int)Player.info.userID);
                                        g_Interfaces.Engine->ClientCmd_Unrestricted(buff);
                                    }
                                }
                                if (column == 2)
                                {
                                    if (ImGui::Checkbox(_("Ignore"), &bIgnored)) {
                                        if (!g_Playerlist.IsIgnored(Player.info.friendsID)) {
                                            g_Playerlist.AddIgnore(Player.info.friendsID);
                                        }
                                        else if (g_Playerlist.IsIgnored(Player.info.friendsID)) {
                                            g_Playerlist.RemoveIgnore(Player.info.friendsID);
                                        }
                                    }
                                    ImGui::SameLine();
                                    ImGui::Dummy(ImVec2(12, 0));
                                }
                            }
                            ImGui::PopID();
                            ImGui::PopStyleColor(2);
                            //ImGui::PopStyleVar();
                        }
                    }
                    ImGui::EndTable();
                }
            }
            //if (ImGui::BeginTable(_("PlayerList"), 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            //    static const char* Team[4] = { "None", "Spec", "Red", "Blue" };
            //    PlayerInfo_t pi{};
            //    ImGui::TableSetupColumn(_("Index"));
            //    ImGui::TableSetupColumn(_("Name"));
            //    ImGui::TableSetupColumn(_("Team"));
            //    ImGui::TableSetupColumn(_("Uhhhhhh"));
            //    //ImGui::TableHeadersRow();
            //    if (g_Interfaces.Engine->IsInGame()) {
            //        /*for (int playerIndex = 1; playerIndex < g_Interfaces.GlobalVars->maxclients; playerIndex++) {
            //            if (g_Interfaces.Engine->GetPlayerInfo(playerIndex, &pi))
            //            {

            //                auto pEntity = g_Interfaces.EntityList->GetClientEntity(playerIndex);
            //                if (!pEntity)
            //                    continue;

            //                int teamNum = pEntity->GetTeamNum();

            //                //static int selected = 0;
            //                //static bool row_selected[64] = {};
            //                ImGui::PushID(pi.friendsID);
            //                ImGui::TableNextRow();
            //                ImGui::PopID();

            //                static int hovered_column = -1;

            //                ImGui::TableSetColumnIndex(0);
            //                static char pName[64];

            //                ImGui::PushID(pi.name);
            //                auto name = g_SteamInterfaces.Friends002->GetFriendPersonaName(CSteamID((UINT64)(0x0110000100000000ULL + pi.friendsID)));

            //                if (pi.name != name) { // Do it yourself m-fed
            //                    snprintf(pName, sizeof(pName), _("%s [%s]"), pi.name, name);
            //                }
            //                else {
            //                    snprintf(pName, sizeof(pName), _("%s"), pi.name);
            //                }

            //                //ImGui::Selectable2(buffWAAAAA, row_selected[playerIndex], ImGuiSelectableFlags_SpanAllColumns, ImVec2(0.f, 0.f)); // I don't want to mess with this bullshit anymore tbh...
            //                ImGui::Text(_("%d"), playerIndex);
            //                ImGui::TableSetColumnIndex(1);
            //                ImGui::Text(_("%s"), pName);
            //                ImGui::TableSetColumnIndex(2);
            //                ImGui::Text(_("%s"), Team[pEntity->GetTeamNum()]);
            //                ImGui::TableSetColumnIndex(3);
            //                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            //                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            //                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            //                if (ImGui::Button(_("Kick"))) {
            //                    static char buff[256];
            //                    snprintf(buff, sizeof(buff), _("callvote kick %d"), (int)pi.userID);

            //                    g_Interfaces.Engine->ClientCmd_Unrestricted(buff);
            //                }
            //                ImGui::SameLine();
            //                if (ImGui::Button(_("Profile"))) {
            //                    g_SteamInterfaces.Friends015->ActivateGameOverlayToUser(_("steamid"), CSteamID((UINT64)(0x0110000100000000ULL + pi.friendsID)));
            //                }
            //                ImGui::SameLine();
            //                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
            //                if (g_Playerlist.IsIgnored(pi.friendsID)) { // There's definitely a better way to do this...
            //                    if (ImGui::Button(_("Unignore"), ImVec2(60, 0))) {
            //                        g_Playerlist.RemoveIgnore(pi.friendsID);
            //                    }
            //                }
            //                else {
            //                    if (ImGui::Button(_("Ignore"), ImVec2(60, 0))) {
            //                        g_Playerlist.AddIgnore(pi.friendsID);
            //                    }
            //                }
            //                ImGui::PopStyleColor(2);
            //                ImGui::PopStyleVar(2);

            //                ImGui::PopID();
            //            }
            //        }*/
            //        for (auto& EnemyPlayer : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES)) {
            //            // Do enemy
            //            if (g_Interfaces.Engine->GetPlayerInfo(EnemyPlayer->GetIndex(), &pi))
            //            {
            //                int teamNum = EnemyPlayer->GetTeamNum();

            //                //static int selected = 0;
            //                //static bool row_selected[64] = {};
            //                ImGui::PushID(pi.friendsID);
            //                ImGui::TableNextRow();
            //                ImGui::PopID();

            //                static int hovered_column = -1;

            //                ImGui::TableSetColumnIndex(0);
            //                static char pName[64];

            //                ImGui::PushID(pi.name);
            //                auto name = g_SteamInterfaces.Friends002->GetFriendPersonaName(CSteamID((UINT64)(0x0110000100000000ULL + pi.friendsID)));

            //                if (pi.name != name) { // Do it yourself m-fed
            //                    snprintf(pName, sizeof(pName), _("%s [%s]"), pi.name, name);
            //                }
            //                else {
            //                    snprintf(pName, sizeof(pName), _("%s"), pi.name);
            //                }

            //                //ImGui::Selectable2(buffWAAAAA, row_selected[playerIndex], ImGuiSelectableFlags_SpanAllColumns, ImVec2(0.f, 0.f)); // I don't want to mess with this bullshit anymore tbh...
            //                ImGui::Text(_("%d"), EnemyPlayer->GetIndex());
            //                ImGui::TableSetColumnIndex(1);
            //                ImGui::Text(_("%s"), pName);
            //                ImGui::TableSetColumnIndex(2);
            //                ImGui::Text(_("%s"), Team[EnemyPlayer->GetTeamNum()]);
            //                ImGui::TableSetColumnIndex(3);
            //                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            //                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            //                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            //                if (ImGui::Button(_("Kick"))) {
            //                    static char buff[256];
            //                    snprintf(buff, sizeof(buff), _("callvote kick %d"), (int)pi.userID);

            //                    g_Interfaces.Engine->ClientCmd_Unrestricted(buff);
            //                }
            //                ImGui::SameLine();
            //                if (ImGui::Button(_("Profile"))) {
            //                    g_SteamInterfaces.Friends015->ActivateGameOverlayToUser(_("steamid"), CSteamID((UINT64)(0x0110000100000000ULL + pi.friendsID)));
            //                }
            //                ImGui::SameLine();
            //                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
            //                if (g_Playerlist.IsIgnored(pi.friendsID)) { // There's definitely a better way to do this...
            //                    if (ImGui::Button(_("Unignore"), ImVec2(60, 0))) {
            //                        g_Playerlist.RemoveIgnore(pi.friendsID);
            //                    }
            //                }
            //                else {
            //                    if (ImGui::Button(_("Ignore"), ImVec2(60, 0))) {
            //                        g_Playerlist.AddIgnore(pi.friendsID);
            //                    }
            //                }
            //                ImGui::PopStyleColor(2);
            //                ImGui::PopStyleVar(2);

            //                ImGui::PopID();
            //            }
            //        }
            //        for (auto& AllyPlayer : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES)) {
            //            // Do ya mom
            //            if (g_Interfaces.Engine->GetPlayerInfo(AllyPlayer->GetIndex(), &pi))
            //            {
            //                int teamNum = AllyPlayer->GetTeamNum();

            //                //static int selected = 0;
            //                //static bool row_selected[64] = {};
            //                ImGui::PushID(pi.friendsID);
            //                ImGui::TableNextRow();
            //                ImGui::PopID();

            //                static int hovered_column = -1;

            //                ImGui::TableSetColumnIndex(0);
            //                static char pName[64];

            //                ImGui::PushID(pi.name);
            //                auto name = g_SteamInterfaces.Friends002->GetFriendPersonaName(CSteamID((UINT64)(0x0110000100000000ULL + pi.friendsID)));

            //                if (pi.name != name) { // Do it yourself m-fed
            //                    snprintf(pName, sizeof(pName), _("%s [%s]"), pi.name, name);
            //                }
            //                else {
            //                    snprintf(pName, sizeof(pName), _("%s"), pi.name);
            //                }

            //                //ImGui::Selectable2(buffWAAAAA, row_selected[playerIndex], ImGuiSelectableFlags_SpanAllColumns, ImVec2(0.f, 0.f)); // I don't want to mess with this bullshit anymore tbh...
            //                ImGui::Text(_("%d"), AllyPlayer->GetIndex());
            //                ImGui::TableSetColumnIndex(1);
            //                ImGui::Text(_("%s"), pName);
            //                ImGui::TableSetColumnIndex(2);
            //                ImGui::Text(_("%s"), Team[AllyPlayer->GetTeamNum()]);
            //                ImGui::TableSetColumnIndex(3);
            //                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            //                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            //                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            //                if (ImGui::Button(_("Kick"))) {
            //                    static char buff[256];
            //                    snprintf(buff, sizeof(buff), _("callvote kick %d"), (int)pi.userID);

            //                    g_Interfaces.Engine->ClientCmd_Unrestricted(buff);
            //                }
            //                ImGui::SameLine();
            //                if (ImGui::Button(_("Profile"))) {
            //                    g_SteamInterfaces.Friends015->ActivateGameOverlayToUser(_("steamid"), CSteamID((UINT64)(0x0110000100000000ULL + pi.friendsID)));
            //                }
            //                ImGui::SameLine();
            //                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
            //                if (g_Playerlist.IsIgnored(pi.friendsID)) { // There's definitely a better way to do this...
            //                    if (ImGui::Button(_("Unignore"), ImVec2(60, 0))) {
            //                        g_Playerlist.RemoveIgnore(pi.friendsID);
            //                    }
            //                }
            //                else {
            //                    if (ImGui::Button(_("Ignore"), ImVec2(60, 0))) {
            //                        g_Playerlist.AddIgnore(pi.friendsID);
            //                    }
            //                }
            //                ImGui::PopStyleColor(2);
            //                ImGui::PopStyleVar(2);

            //                ImGui::PopID();
            //            }
            //        }
            //    }
            //    ImGui::EndTable();
            //}
            ImGui::End();
        }
        ImGui::PopStyleColor();
        ImGui::PopFont();


        // Horrible code...
        ImGui::SetNextWindowPos(ImVec2((g_ScreenSize.w / 2) - MenuSize.x / 2, (g_ScreenSize.h / 2) - MenuSize.y / 2), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(MenuSize));

        ImGui::Begin(_("##GUI"), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        {
            {
                MenuCol = ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg];
                s = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2);
                p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y); auto draw = ImGui::GetWindowDrawList();
                ImGui::GetBackgroundDrawList()->AddRect({ p.x,p.y }, { p.x + s.x, p.y + s.y }, ImColor(15, 17, 19, 255));
                draw->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y + 40), ImColor(15, 17, 19, 200), 5); // Titlebar
                draw->AddRectFilled(ImVec2(p.x, p.y + 40), ImVec2(p.x + s.x, p.y + s.y), ImColor(18, 20, 21, 200), 5, ImDrawCornerFlags_Bot); // Background

                ImGui::PushFont(name);
                draw->AddText(ImVec2(p.x + 36 / 2, p.y + 29 / 2), ImColor(255, 255, 255, 255), _("james"));
                draw->AddText(ImVec2(p.x + 36 / 2 + ImGui::CalcTextSize(_("james")).x, p.y + 29 / 2), ImColor(MenuCol.x, MenuCol.y, MenuCol.z, 255.f), _(".poop"));
                ImGui::PopFont();
            }
            {
                ImGui::PushFont(font);
                ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 71 * 6.7); // 6 = how much tabs, .7 = how much tabs + 0.1
                ImGui::BeginGroup();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });
                if (ImGui::Tab(_(ICON_FA_CROSSHAIRS " Aimbot"), tab == 0))tab = 0; ImGui::SameLine();
                if (ImGui::Tab(_(ICON_FA_EXPAND " Triggerbot"), tab == 1, 90))tab = 1; ImGui::SameLine();
                if (ImGui::Tab(_(ICON_FA_EYE " ESP"), tab == 2))tab = 2; ImGui::SameLine();
                if (ImGui::Tab(_(ICON_FA_PALETTE " Visuals"), tab == 3))tab = 3; ImGui::SameLine();
                if (ImGui::Tab(_(ICON_FA_COGS " Misc"), tab == 4))tab = 4; ImGui::SameLine();
                if (ImGui::Tab(_(ICON_FA_FILE " Config"), tab == 5))tab = 5;

                ImGui::PopStyleVar();
                ImGui::EndGroup();
                ImGui::PopFont();
            }
            ImGui::PushFont(font);
            {
                switch (tab) {
                case 0: {
                    AimbotTab();
                    break;
                }
                case 1: {
                    TriggerbotTab();
                    break;
                }
                case 2: {
                    // Probably inefficient, how unfortunate.
                    if (nESPTab == 1)
                        ESPTab();
                    if (nESPTab == 2)
                        ESPTab2();
                    if (nESPTab == 3)
                        ESPTab3();
                    break;
                }
                case 3: {
                    VisualsTab();
                    break;
                }
                case 4: {
                    MiscTab();
                    break;
                }
                case 5: {
                    ConfigsTab();
                    break;
                }
                default: {
                    ImGui::SetCursorPosX(7);
                    //ImGui::Text(_("How the fuck did you manage to get here?\nInvalid tab value : 5 / %d"), tab);
                }
                }
                ImGui::PopFont();
            }
            ImGui::End();
        }
        ImGui::PopStyleVar();
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
}
