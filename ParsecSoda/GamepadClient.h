#pragma once

#include "Gamepad.h"
#include <Windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <vector>
#include <iostream>

#define GAMEPAD_LIMIT_NOT_FOUND -1

class GamepadClient
{
	typedef struct ParsecGuestLimit
	{
		uint32_t userId = OWNER_ID_NONE;
		uint8_t padLimit = XUSER_MAX_COUNT;
		uint8_t slots = 0;
	} ParsecGuestLimit;

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
	void onRageQuit(ParsecGuest guest);
	void setLimit(uint32_t guestUserId, uint8_t padLimit);
	void refreshSlots(ParsecGuestLimit *limit);

private:
	ParsecGuestLimit * getLimit(uint32_t guestUserId);
	PVIGEM_CLIENT _client;
	std::vector<Gamepad> _gamepads;
	std::vector<ParsecGuestLimit> _guestLimits;
};

