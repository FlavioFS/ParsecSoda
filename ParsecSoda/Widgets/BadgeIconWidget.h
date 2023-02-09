#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"
#include <string>
#include "IconButton.h"
#include "TitleTooltipWidget.h"

using namespace std;

class BadgeIconWidget
{
public:
	static void render(
		Icon icon, const char* tooltipTitle, const char* tooltipDescription,
		ImVec4 tint = AppColors::primary, ImVec2 size = ImVec2(25, 25)
	);

	static void render(
		Icon icon, string tooltipTitle, string tooltipDescription,
		ImVec4 tint = AppColors::primary, ImVec2 size = ImVec2(25, 25)
	);
};

