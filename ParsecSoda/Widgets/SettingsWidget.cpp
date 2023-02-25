#include "SettingsWidget.h"

bool SettingsWidget::render(SFXList& sfxList)
{
    ImGui::PushID("##Settings Window");

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(240, 180), ImVec2(800, 900));
    ImGui::Begin("Settings");
    AppStyle::pushInput();

    static ImVec2 size, textSize, buttonSize = ImVec2(25, 25);
    size = ImGui::GetContentRegionAvail();

    float cursor, textPosition;

    if (BadgeButtonWidget::render(AppIcons::refresh, "Refresh SFX list", "Press to reload custom sound effects list.", "Refresh SFX list button"))
    {
        sfxList.init(MetadataCache::getSfxPath().c_str());
    }
    ImGui::SameLine();
    cursor = ImGui::GetCursorPosY();
    textSize = ImGui::CalcTextSize("Reload SFX List");
    ImGui::SetCursorPosY(cursor + 0.5f * (buttonSize.y + 4 - textSize.y));
    ImGui::Text("Reload SFX List");

    if (SwitchWidget::render(
        MetadataCache::preferences.enableSfx,
        "###Enable SFX switch", "SFX & Bonk",
        "Sound effects [ON]", "Parsec Soda will play sfx commands.",
        "Sound effects [OFF]", "Parsec Soda will NOT play any sfx commands."
    ))
    {
        MetadataCache::savePreferences();
    }

    if (SwitchWidget::render(
        MetadataCache::preferences.enableKickSfx,
        "###Enable Kick SFX switch", "Kick SFX",
        "Kick sound effect [ON]", "Parsec Soda will play the kick sound effect.",
        "Kick sound effect [OFF]", "Parsec Soda will NOT play the kick sound effect."
    ))
    {
        MetadataCache::savePreferences();
    }

    if (SwitchWidget::render(
        MetadataCache::preferences.enableBanSfx,
        "###Enable Ban SFX switch", "Ban SFX",
        "Ban sound effect [ON]", "Parsec Soda will play the ban sound effect.",
        "Ban sound effect [OFF]", "Parsec Soda will NOT play the ban sound effect."
    ))
    {
        MetadataCache::savePreferences();
    }

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    ImGui::PopStyleVar(3);
    ImGui::PopID();

    return true;
}