#include "BadgeIconWidget.h"

void BadgeIconWidget::render(
	Icon icon, const char* tooltipTitle, const char* tooltipDescription,
	ImVec4 tint, ImVec2 size
)
{
	ImGui::Image(icon, size, ImVec2(0, 0), ImVec2(1, 1), tint);
	TitleTooltipWidget::render(tooltipTitle, tooltipDescription);
}

void BadgeIconWidget::render(Icon icon, string tooltipTitle, string tooltipDescription, ImVec4 tint, ImVec2 size)
{
	render(icon, tooltipTitle.c_str(), tooltipDescription.c_str(), tint, size);
}
