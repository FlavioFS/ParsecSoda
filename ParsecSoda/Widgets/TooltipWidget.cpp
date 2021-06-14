#include "TooltipWidget.h"

bool TooltipWidget::render(const char* text, bool forceShow)
{
	if (ImGui::IsItemHovered() || forceShow)
	{
		AppStyle::pushInput();
		ImGui::SetTooltip(text);
		AppStyle::pop();
		return true;
	}
}
