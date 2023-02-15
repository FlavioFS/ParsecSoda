#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/ImVecExtensions.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include <cmath>

class ProgressCircularWidget
{
public:
	static void render(
		int radius,
		int thickness,
		float value,
		float valueMin = 0.0f,
		float valueMax = 1.0f,
		ImU32 activeColor = COLOR_PRIMARY_U32,
		ImU32 backgroundColor = COLOR_WIDGET_BG_U32
	);
};

