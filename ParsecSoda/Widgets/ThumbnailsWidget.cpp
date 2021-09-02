#include "ThumbnailsWidget.h"

bool ThumbnailsWidget::render(ParsecSession& session, vector<Thumbnail>& _thumbnails)
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(800, 600));
    AppStyle::pushTitle();
    ImGui::Begin("Arcade Room Thumbnails");
    AppStyle::pushInput();
    
    static const uint32_t REFRESH_COOLDOWN = MINUTES(1);
    static uint32_t now, next;
    static float fill = 0;
    static Debouncer debouncer = Debouncer(500, [&]() {
        next = now + REFRESH_COOLDOWN;
        session.fetchArcadeRoomListSync();
    });

    now = Clock::now();

    if (next <= now)
    {
        next = now + REFRESH_COOLDOWN;
        debouncer.start();
    }
    fill = 1.0f - ((float)(next - now) / REFRESH_COOLDOWN);

    static ImVec2 progressSize = ImVec2(32, 32);
    static ImVec2 cursor;
    cursor = ImGui::GetCursorPos();
    ProgressCircularWidget::render(progressSize.x/2, progressSize.x/4, fill);
    ImGui::SetCursorPos(cursor);
    if (ImGui::Button("###Thumb Refresh Button", progressSize))
    {
        debouncer.start();
    }
    TitleTooltipWidget::render("Next refresh", "Click to force refresh of public room's thumbnails.");

    ImGui::Dummy(ImVec2(0, 5));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 5));


    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    vector<Thumbnail>::iterator it;
    static uint32_t index = 0;
    index = 0;
    for (it = _thumbnails.begin(); it != _thumbnails.end(); ++it)
    {
        index++;
        ImGui::PushID(index);
        if (IconButton::render(
            (*it).saved ? AppIcons::saveOn : AppIcons::saveOff,
            (*it).saved ? AppColors::positive : AppColors::negative,
            ImVec2(25, 25)
        ))
        {
            (*it).saved = !(*it).saved;
        }
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
        ImGui::TextWrapped((*it).name.c_str());
    }
    ImGui::PopStyleVar();

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}