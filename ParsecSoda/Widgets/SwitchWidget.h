#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "ToggleIconButtonWidget.h"
#include <string>

using namespace std;

class SwitchWidget
{
public:
	/**
	 * Render a switch button widget with tooltips.
	 * @param isEnabled The switch state control variable.
	 * @param id ImGui ID to avoid target overlapping.
	 * @param text Text at right side of tooltip.
	 * @param tooltipTitle Title when hovered (defaults for ON/OFF when OFF not provided).
	 * @param tooltipDescription Description when hovered (defaults for ON/OFF when OFF not provided).
	 * @param tooltipTitleOff Title when hovered (and isEnabled is false).
	 * @param tooltipDescriptionOff Description when hovered (and isEnabled is false).
	 * @param size Button size.
	 * @param colorOn Tint color when enabled.
	 * @param colorOff Tint color when disabled.
	 * @return True if switched was toggled (clicked). False otherwise.
	 */
	static bool render(
		bool& isEnabled,
		string id = nullptr, string text = "",
		string tooltipTitle = "", string tooltipDescription = "",
		string tooltipTitleOff = "", string tooltipDescriptionOff = "",
		ImVec2 size = ImVec2(25, 25),
		ImVec4 colorOn = AppColors::positive, ImVec4 colorOff = AppColors::negative
	);
};

