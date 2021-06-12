#pragma once

#include "../imgui/imgui.h"
#include "../AppIcons.h"
#include "../AppStyle.h"


class ConfirmPopupWidget
{
public:
	static bool render(const char* title, bool& showPopup, AppIcons& icons, AppStyle& style);
};

