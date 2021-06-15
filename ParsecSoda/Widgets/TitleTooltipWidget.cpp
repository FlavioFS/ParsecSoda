#include "TitleTooltipWidget.h"

bool TitleTooltipWidget::render(const char* title, const char* description, bool forceShow)
{
	if (ImGui::IsItemHovered() || forceShow)
	{
		ImGui::BeginTooltip();
		AppFonts::pushInput();
		
		// Title
		AppColors::pushPrimary();
		ImGui::Text(title);
		AppColors::pop();

		// Description
		AppColors::pushLabel();
		ImGui::Text(description);
		AppColors::pop();

		AppFonts::pop();
		ImGui::EndTooltip();
		return true;
	}
}
