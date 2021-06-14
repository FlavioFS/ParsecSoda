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
#include "GuestList.h"

using namespace std;

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
public:
	typedef struct ParsecGuestPrefs
	{
		uint32_t userId = OWNER_ID_NONE;
		uint8_t padLimit = XUSER_MAX_COUNT;
		uint8_t slots = 0;
		bool mirror = false;
	} ParsecGuestPrefs;

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
	int clearAFK(GuestList &guests);

	bool disconnect(int gamepadIndex);
	bool clearOwner(int gamepadIndex);
	bool sendMessage(ParsecGuest guest, ParsecMessage message);
	int onRageQuit(Guest &guest);
	void setLimit(uint32_t guestUserId, uint8_t padLimit);
	bool toggleMirror(uint32_t guestUserId);
	const GAMEPAD_PICK_REQUEST pick(Guest guest, int gamepadIndex);
	ParsecGuestPrefs * getPrefs(uint32_t guestUserId);
	
	vector<GamepadStatus> getGamepadStatus();
	vector<Gamepad>& getGamepads();

	bool lock = false;

private:
	void releaseGamepads();
	void setMirror(uint32_t guestUserId, bool mirror);
	bool tryAssignGamepad(ParsecGuest guest, uint32_t padId);
	void refreshSlots(ParsecGuestPrefs *prefs);
	void freeSlots(uint32_t userId);
	bool isRequestState(ParsecMessage message);
	bool isRequestButton(ParsecMessage message);
	bool isRequestKeyboard(ParsecMessage message);
	bool find(uint32_t userID, uint32_t padID, Gamepad *gamepad);
	bool findFirst(uint32_t userID, Gamepad *gamepad);
	bool findAll(uint32_t userID, vector<Gamepad*> &gamepads);
	PVIGEM_CLIENT _client;
	vector<Gamepad> _gamepads;
	vector<ParsecGuestPrefs> _guestPrefs;
};