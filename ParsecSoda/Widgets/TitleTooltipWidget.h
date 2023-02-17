#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"

class TitleTooltipWidget
{
public:
	static bool render(const char* title, const char* description, bool forceShow = false);
	static bool renderBool(
		const bool& state,
		const char* titleTrue, const char* titleFalse,
		const char* descriptionTrue, const char* descriptionFalse,
		bool forceShow = false
	);
};

