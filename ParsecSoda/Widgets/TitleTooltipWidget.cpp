#include "TitleTooltipWidget.h"

bool TitleTooltipWidget::render(const char* title, const char* description, bool forceShow)
{
	if (ImGui::IsItemHovered() || forceShow)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

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

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		return true;
	}

	return false;
}

bool TitleTooltipWidget::renderBool(
	const bool& state,
	const char* titleTrue, const char* titleFalse,
	const char* descriptionTrue, const char* descriptionFalse,
	bool forceShow
)
{
	if (state) return render(titleTrue, descriptionTrue, forceShow);
	return render(titleFalse, descriptionFalse, forceShow);
}

