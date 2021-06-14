#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "IconButton.h"

class ConfirmPopupWidget
{
public:
	static bool render(const char* title, bool& showPopup);
};

