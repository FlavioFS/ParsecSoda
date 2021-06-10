#pragma once

#include "Gamepad.h"
#include <Windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "GuestData.h"
#include "KeyboardMaps.h"

#define GAMEPAD_LIMIT_NOT_FOUND -1
#define GAMEPAD_PICK_TIMEOUT_MS 1000


enum class GAMEPAD_PICK_REQUEST
{
	OK,
	SAME_USER,
	TAKEN,
	EMPTY_HANDS,
	LIMIT_BLOCK,
	OUT_OF_RANGE
};

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
	void connectAllGamepads();
	void disconnectAllGamepads();
	void sortGamepads();
	Gamepad connectNextGamepad();
	void release();
	Gamepad getGamepad(int index);
	int clearAFK(ParsecGuest * guests, int guestCount);

	// MUTEX required (called externally)
	bool disconnect(int gamepadIndex);
	bool sendMessage(ParsecGuest guest, ParsecMessage message);
	int onRageQuit(ParsecGuest guest);
	void setLimit(uint32_t guestUserId, uint8_t padLimit);
	bool toggleMirror(uint32_t guestUserId);
	const GAMEPAD_PICK_REQUEST pick(ParsecGuest guest, int gamepadIndex);
	const std::vector<GamepadStatus> getGamepadStatus();

private:
	void releaseGamepads();
	void setMirror(uint32_t guestUserId, bool mirror);
	bool tryAssignGamepad(ParsecGuest guest, uint32_t padId);
	void refreshSlots(ParsecGuestPrefs *prefs);
	void freeSlots(uint32_t userId);
	bool isRequestState(ParsecMessage message);
	bool isRequestButton(ParsecMessage message);
	bool isRequestKeyboard(ParsecMessage message);
	ParsecGuestPrefs * getPrefs(uint32_t guestUserId);
	PVIGEM_CLIENT _client;
	std::vector<Gamepad> _gamepads;
	std::vector<ParsecGuestPrefs> _guestPrefs;
};