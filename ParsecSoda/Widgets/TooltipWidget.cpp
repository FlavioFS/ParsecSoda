#include "TooltipWidget.h"

bool TooltipWidget::render(const char* text, AppStyle& style)
{
	style.pushInput();
	ImGui::SetTooltip(text);
	style.pop();
	return true;
}
