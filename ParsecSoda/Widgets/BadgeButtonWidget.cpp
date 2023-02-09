#include "BadgeButtonWidget.h"

bool BadgeButtonWidget::render(
	Icon icon,
	const char* tooltipTitle, const char* tooltipDescription, const char* id,
	ImVec2 size, ImVec4 tint
)
{
	static bool result;
	result = false;

	if (id)
	{
		ImGui::PushID(id);
		result = renderButtonAndTooltip(icon, tooltipTitle, tooltipDescription, size, tint);
		ImGui::PopID();
	}
	else
	{
		result = renderButtonAndTooltip(icon, tooltipTitle, tooltipDescription, size, tint);
	}

	return result;
}

bool BadgeButtonWidget::render(Icon icon, string tooltipTitle, string tooltipDescription, string id, ImVec2 size, ImVec4 tint)
{
	return render(icon, tooltipTitle.c_str(), tooltipDescription.c_str(), id.c_str(), size, tint);
}

bool BadgeButtonWidget::renderButtonAndTooltip(Icon icon, const char* tooltipTitle, const char* tooltipDescription, ImVec2 size, ImVec4 tint)
{
	static bool result;
	result = false;

	if (IconButton::render(icon, tint, size))
	{
		result = true;
	}

	if (tooltipTitle && tooltipDescription)
	{
		TitleTooltipWidget::render(tooltipTitle, tooltipDescription);
	}

	return result;
}
