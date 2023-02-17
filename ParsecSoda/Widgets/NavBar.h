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
        Hosting& hosting,
        bool& showLogin,
        bool& showHostSettings,
        bool& showGamepads,
        bool& showButtonLock,
        bool& showHotseats,
        bool& showMasterOfPuppets,
        bool& showChat,
        bool& showGuests,
        bool& showThumbnails,
        bool& showLog,
        bool& showAudio,
        bool& showVideo,
        bool& showInfo
    );

private:
    static void renderNavItem(Icon onIcon, Icon offIcon, bool& toggle, ImVec2 size, const char* tooltip);
    static void renderNavTooltip(const char* title, bool& show);
};

