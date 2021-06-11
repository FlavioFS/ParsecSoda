#pragma once

#include "IWidget.h"
#include "ToggleTexButtonWidget.h"
#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../AppIcons.h"
#include "../AppFonts.h"
#include "../AppColors.h"
#include "../AppStyle.h"
#include "TooltipWidget.h"

class HostSettingsWidget
{
public:
	HostSettingsWidget(AppIcons& icons, Hosting& hosting);
	bool render(AppStyle& style);

	const ImVec2 DEFAULT_BUTTON_SIZE = ImVec2(40, 40);
	const uint8_t LINK_COMPATIBLE_SECRET_SIZE = 9;

private:
	bool isDirty();

	// Dependency injection
	Hosting& _hosting;
	AppIcons& _icons;

	// Imported Widgets
	ToggleTexButtonWidget _publicRoomBtn, _playBtn;

	// Attributes
	char _roomName[HOST_NAME_LEN];
	char _gameID[GAME_ID_LEN];
	char _secret[HOST_SECRET_LEN];
	int32_t _maxGuests;
	bool _publicGame;
};

