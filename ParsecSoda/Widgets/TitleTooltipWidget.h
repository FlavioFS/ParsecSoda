#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"

class TitleTooltipWidget
{
public:
	static bool render(const char* title, const char* description, bool forceShow = false);
};

