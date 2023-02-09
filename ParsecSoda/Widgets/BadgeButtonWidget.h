#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"
#include <string>
#include "IconButton.h"
#include "TitleTooltipWidget.h"

using namespace std;

class BadgeButtonWidget
{
public:
	static bool render(
		Icon icon, const char* tooltipTitle = nullptr, const char* tooltipDescription = nullptr,
		const char* id = nullptr, ImVec2 size = ImVec2(25, 25), ImVec4 tint = AppColors::primary
	);

	static bool render(
		Icon icon, string tooltipTitle = nullptr, string tooltipDescription = nullptr,
		string id = nullptr, ImVec2 size = ImVec2(25, 25), ImVec4 tint = AppColors::primary
	);

private:
	static bool renderButtonAndTooltip(
		Icon icon, const char* tooltipTitle, const char* tooltipDescription,
		ImVec2 size, ImVec4 tint
	);
};

