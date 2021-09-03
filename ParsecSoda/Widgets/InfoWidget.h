#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"

class InfoWidget
{
public:
	static bool render();

private:
	static bool renderSugoiButton(const char* text, const wchar_t* link);
	static bool renderLinkButton(const char* text, const wchar_t* link);
};

