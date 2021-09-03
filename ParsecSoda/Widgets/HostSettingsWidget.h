#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../AudioTools.h"
#include "../Debouncer.h"
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
#include <chrono>

class HostSettingsWidget
{
public:
	HostSettingsWidget(Hosting& hosting);
	bool render();
	void updateSecretLink();

	const ImVec2 DEFAULT_BUTTON_SIZE = ImVec2(40, 40);
	const uint8_t LINK_COMPATIBLE_SECRET_SIZE = 9;

private:
	void savePreferences();
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
	char _secretLink[128];
	int32_t _maxGuests;
	bool _publicGame;

	int _micVolume = 80;
	int _speakersVolume = 30;


	const unsigned int DEBOUNCE_TIME_MS = 1000;

	vector<Thumbnail>& _thumbnails;
	string _thumbnailName = "Please select thumbnail.";
};

