#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "IconButton.h"

class NavBar
{
public:
	static void render(
        bool& showHostSettings,
        bool& showGamepads,
        bool& showChat,
        bool& showGuests,
        bool& showLog
    );
};

