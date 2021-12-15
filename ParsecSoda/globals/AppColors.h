#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"

#define COLOR_PRIMARY_U32 ImGui::GetColorU32(IM_COL32(0, 120, 204, 255))
#define COLOR_POSITIVE_U32 ImGui::GetColorU32(IM_COL32(0, 171, 105, 255))
#define COLOR_NEGATIVE_U32 ImGui::GetColorU32(IM_COL32(191, 41, 71, 255))
#define COLOR_WIDGET_BG_U32 ImGui::GetColorU32(IM_COL32(20, 20, 20, 255))

class AppColors
{
public:
	static void init();

	static ImVec4 alpha(ImVec4& color, float w);
	static ImU32 toU32(ImVec4& color);

	static void pushColor(ImVec4& color);
	static void pop();

	static void pushPrimary();
	static void pushSecondary();

	static void pushTitle();
	static void pushLabel();
	static void pushInput();
	static void pushPositive();
	static void pushNegative();
	static void pushWarning();

	static ImVec4 title;
	static ImVec4 label;
	static ImVec4 input;
	static ImVec4 positive;
	static ImVec4 negative;
	static ImVec4 warning;

	static ImVec4 white;
	static ImVec4 black;
	static ImVec4 invisible;

	static ImVec4 primary;
	static ImVec4 secondary;

	static ImVec4 backgroundIcon;

	static ImVec4 disabled;
};