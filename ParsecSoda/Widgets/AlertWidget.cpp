#include "AlertWidget.h"

bool AlertWidget::render(const char * title, const char* message)
{
    static bool result;
    result = false;
    static ImVec2 BUTTON_SIZE = ImVec2(160, 50);
    static ImVec2 pos, wSize, textSize;
    textSize = ImGui::CalcTextSize(message);

    wSize = ImGui::GetWindowSize();
    pos = ImVec2(ImGui::GetWindowPos().x + (wSize.x - 300) * 0.5f, ImGui::GetCursorPosY());

    ImGui::SetNextWindowSize(ImVec2(300, 30 * (textSize.x / wSize.x) + 160));
    ImGui::SetNextWindowPos(pos);
    AppStyle::pushTitle();
    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
        wSize = ImGui::GetWindowSize();

        ImGui::Dummy(ImVec2(10, 5));
        

        AppStyle::pushInput();
        if (textSize.x < wSize.x - 40)
        {
            ImGui::SetCursorPosX((wSize.x - textSize.x) * 0.5f);
        }
        ImGui::TextWrapped(message);
        AppStyle::pop();

        ImGui::SetCursorPosY(wSize.y - 70);

        ImGui::PushStyleColor(ImGuiCol_Button, AppColors::secondary);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.12f, 0.59f, 0.92f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AppColors::primary);
        if (ImGui::Button("OK", ImVec2(wSize.x - 40, 50)))
        {
            result = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        ImGui::EndPopup();
    }
    AppStyle::pop();

    return result;
}
