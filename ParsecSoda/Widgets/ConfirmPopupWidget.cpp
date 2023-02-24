#include "ConfirmPopupWidget.h"

bool ConfirmPopupWidget::render(const char * title, bool &showPopup)
{
    static bool result;
    result = false;
    static ImVec2 BUTTON_SIZE = ImVec2(64, 64);

    ImGui::SetNextWindowSize(ImVec2(300, 200));
    AppStyle::pushTitle();
    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
        ImGui::Dummy(ImVec2(10, 40));

        ImGui::Indent(10);
        if (IconButton::render(AppIcons::no, AppColors::negative, BUTTON_SIZE))
        {
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        ImGui::Indent(165);
        if (IconButton::render(AppIcons::yes, AppColors::positive, BUTTON_SIZE))
        {
            result = true;
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    AppStyle::pop();

    return result;
}
