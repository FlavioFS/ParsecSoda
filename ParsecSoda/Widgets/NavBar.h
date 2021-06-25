#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../Hosting.h"
#include "IconButton.h"
#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"

class NavBar
{
public:
	static void render(
        bool& showLogin,
        bool& showHostSettings,
        bool& showGamepads,
        bool& showChat,
        bool& showGuests,
        bool& showLog,
        bool& showAudio
    );

private:
    static void renderNavtooltip(const char* title, bool& show);
};

