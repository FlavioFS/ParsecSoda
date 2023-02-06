#include "BoolButtonWidget.h"

bool BoolButtonWidget::render(const char* label, bool& value)
{
    static float width;

    ImGui::BeginGroup();

    if (label && sizeof(label) > 0)
    {
        ImGui::Text(label);
        width = ImGui::GetItemRectSize().x;
        ImGui::Dummy(ImVec2(0.5f * width - 29.0f, 0.0f));
        ImGui::SameLine();
    }

    if (ToggleIconButtonWidget::render(AppIcons::yes, AppIcons::no, value))
    {
        value = !value;
    }
    ImGui::EndGroup();

	return true;
}
