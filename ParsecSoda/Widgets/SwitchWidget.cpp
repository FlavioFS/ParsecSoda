#include "SwitchWidget.h"

bool SwitchWidget::render(
	bool& isEnabled,
	string id, string text,
	string tooltipTitle, string tooltipDescription,
	string tooltipTitleOff, string tooltipDescriptionOff,
	ImVec2 size,
	ImVec4 colorOn, ImVec4 colorOff
)
{
	static string title;
	static string description;
	static ImVec4 color;
	static Icon icon;
	static bool isPressed;
	isPressed = false;
	
	if (isEnabled)
	{
		icon = AppIcons::switchOn;
		title = tooltipTitle;
		description = tooltipDescription;
	}
	else
	{
		icon = AppIcons::switchOff;
		title = tooltipTitleOff.empty() ? tooltipTitle : tooltipTitleOff;
		description = tooltipDescriptionOff.empty() ? tooltipDescription : tooltipDescriptionOff;
	}

	ImGui::BeginGroup();

	isPressed = ToggleIconButtonWidget::render(AppIcons::switchOn, AppIcons::switchOff, isEnabled, id.c_str(), colorOn, colorOff, size);
	if (isPressed)
	{
		isEnabled = !isEnabled;
	}

	if (!text.empty())
	{
		ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetFrameHeight() - ImGui::GetTextLineHeight() + 1));
		ImGui::Text(text.c_str());
	}
	ImGui::EndGroup();

	return isPressed;
}
