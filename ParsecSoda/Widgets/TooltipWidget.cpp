#include "TooltipWidget.h"

bool TooltipWidget::render(const char* text)
{
	AppStyle::pushInput();
	ImGui::SetTooltip(text);
	AppStyle::pop();
	return true;
}
