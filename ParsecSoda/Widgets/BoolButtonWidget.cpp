#include "BoolButtonWidget.h"

bool BoolButtonWidget::render(const char* label, bool& value, float iconIndent)
{
    ImGui::BeginGroup();
    ImGui::Text("Public room");
    ImGui::Dummy(ImVec2(iconIndent, 0.0f));
    ImGui::SameLine();
    if (ToggleIconButtonWidget::render(AppIcons::yes, AppIcons::no, value))
    {
        value = !value;
    }
    ImGui::EndGroup();

	return true;
}
