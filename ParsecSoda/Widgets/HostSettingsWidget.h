#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../AudioTools.h"
#include "../globals/AppIcons.h"
#include "../globals/AppFonts.h"
#include "../globals/AppColors.h"
#include "../globals/AppStyle.h"
#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "TooltipWidget.h"
#include "ConfirmPopupWidget.h"
#include "IconButton.h"
#include "IntRangeWidget.h"
#include "BoolButtonWidget.h"
#include "AudioControlWidget.h"

class HostSettingsWidget
{
public:
	HostSettingsWidget(Hosting& hosting);
	bool render();

	const ImVec2 DEFAULT_BUTTON_SIZE = ImVec2(40, 40);
	const uint8_t LINK_COMPATIBLE_SECRET_SIZE = 9;

private:
	bool isDirty();
	float lerp(float val1, float val2, float t = 0.11f);
	float easing(float t);

	// Dependency injection
	Hosting& _hosting;
	AudioIn& _audioIn;
	AudioOut& _audioOut;

	// Imported Widgets
	ToggleIconButtonWidget _publicRoomBtn, _playBtn;

	// Attributes
	char _roomName[HOST_NAME_LEN];
	char _gameID[GAME_ID_LEN];
	char _secret[HOST_SECRET_LEN];
	int32_t _maxGuests;
	bool _publicGame;
};

