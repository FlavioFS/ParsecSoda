#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"

class AppColors
{
public:
	static void init();

	static ImVec4 alpha(ImVec4& color, float w);

	static void pushColor(ImVec4& color);
	static void pop();

	static void pushPrimary();
	static void pushSecondary();

	static void pushTitle();
	static void pushLabel();
	static void pushInput();
	static void pushAlert();
	static void pushError();

	static ImVec4 title;
	static ImVec4 label;
	static ImVec4 input;
	static ImVec4 alert;
	static ImVec4 error;

	static ImVec4 white;
	static ImVec4 black;
	static ImVec4 invisible;

	static ImVec4 primary;
	static ImVec4 secondary;
};