#pragma once

#include "IWidget.h"
#include "ToggleTexButtonWidget.h"
#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../AppIcons.h"

class HostSettingsWidget
{
public:
	HostSettingsWidget(AppIcons *icons, Hosting* hosting);
	bool render();

	const ImVec2 DEFAULT_BUTTON_SIZE = ImVec2(40, 40);
	const uint8_t LINK_COMPATIBLE_SECRET_SIZE = 9;

private:
	bool isDirty();

	AppIcons* _icons;
	ToggleTexButtonWidget _publicRoomBtn, _playBtn;
	
	char _roomName[HOST_NAME_LEN];
	char _gameID[GAME_ID_LEN];
	char _secret[HOST_SECRET_LEN];
	int32_t _maxGuests;
	bool _publicGame;
	Hosting* _hosting;
};

