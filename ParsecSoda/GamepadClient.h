#pragma once

#include "Gamepad.h"
#include <Windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <vector>
#include <iostream>
#include "GuestData.h"

#define GAMEPAD_LIMIT_NOT_FOUND -1

class GamepadClient
{
	typedef struct ParsecGuestPrefs
	{
		uint32_t userId = OWNER_ID_NONE;
		uint8_t padLimit = XUSER_MAX_COUNT;
		uint8_t slots = 0;
		bool mirror = false;
	} ParsecGuestPrefs;

public:
	~GamepadClient();
	bool init();
	Gamepad createGamepad();
	void createMaximumGamepads();
	Gamepad connectNextGamepad();
	void release();
	void releaseGamepads();
	Gamepad getGamepad(int index);
	bool sendMessage(ParsecGuest guest, ParsecMessage message);
	bool tryAssignGamepad(ParsecGuest guest, uint32_t padId);
	int onRageQuit(ParsecGuest guest);
	void setLimit(uint32_t guestUserId, uint8_t padLimit);
	void setMirror(uint32_t guestUserId, bool mirror);
	bool toggleMirror(uint32_t guestUserId);
	void refreshSlots(ParsecGuestPrefs *prefs);
	const std::vector<GamepadStatus> getGamepadStatus();

private:
	void freeSlots(uint32_t userId);
	bool isRequestState(ParsecMessage message);
	bool isRequestButton(ParsecMessage message);
	ParsecGuestPrefs * getLimit(uint32_t guestUserId);
	PVIGEM_CLIENT _client;
	std::vector<Gamepad> _gamepads;
	std::vector<ParsecGuestPrefs> _guestPrefs;
};

