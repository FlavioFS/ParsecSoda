#include "ConfirmPopupWidget.h"

bool ConfirmPopupWidget::render(const char * title, bool &showPopup, AppIcons& icons, AppStyle& style)
{
    static bool result;
    result = false;
    static ImVec2 BUTTON_SIZE = ImVec2(64, 64);

    ImGui::SetNextWindowSize(ImVec2(300, 200));
    style.pushTitle();
    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
        ImGui::BeginChild(title, ImVec2(10, 40));
        ImGui::EndChild();

        ImGui::Indent(10);
        if (ImGui::ImageButton(icons.no, BUTTON_SIZE))
        {
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        ImGui::Indent(165);
        if (ImGui::ImageButton(icons.yes, BUTTON_SIZE))
        {
            result = true;
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    style.pop();

    return result;
}
