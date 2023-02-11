#include "BoolButtonWidget.h"

bool BoolButtonWidget::render(const char* label, bool& value, ImVec2 size)
{
    static float width;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::BeginGroup();
    if (label && sizeof(label) > 0)
    {
        static ImVec2 textSize;
        textSize = ImGui::CalcTextSize(label);
        ImGui::Text(label);
        width = ImGui::GetItemRectSize().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.5f * (textSize.x - size.x));
        //ImGui::Dummy(ImVec2(0.5f * width - size.x, 0.0f));
        //ImGui::SameLine();
    }

    if (ToggleIconButtonWidget::render(AppIcons::yes, AppIcons::no, value, size))
    {
        value = !value;
    }
    ImGui::EndGroup();
    ImGui::PopStyleVar();

	return true;
}
