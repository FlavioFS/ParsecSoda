#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

class AppColors
{

public:
	AppColors();

	void pushTitle();
	void pushLabel();
	void pushInput();
	void pop();

private:
	ImVec4 _title;
	ImVec4 _label;
	ImVec4 _input;
};