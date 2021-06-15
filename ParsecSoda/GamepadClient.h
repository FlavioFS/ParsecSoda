#pragma once

#include "Gamepad.h"
#include <Windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include "GuestData.h"
#include "KeyboardMaps.h"
#include "GuestList.h"

using namespace std;

#define GAMEPAD_LIMIT_NOT_FOUND -1
#define GAMEPAD_PICK_TIMEOUT_MS 1000



class GamepadClient
{
public:
	enum class PICK_REQUEST
	{
		OK,
		SAME_USER,
		TAKEN,
		EMPTY_HANDS,
		LIMIT_BLOCK,
		OUT_OF_RANGE
	};

	class GuestPreferences
	{
	public:
		GuestPreferences()
			: userID(0), padLimit(1), mirror(false)
		{}
		GuestPreferences(uint32_t userID, int padLimit = 1, bool mirror = false)
			: userID(userID), padLimit(padLimit), mirror(mirror)
		{}

		uint32_t userID = 0;
		int padLimit = 1;
		bool mirror = false;
	};

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

	bool sendMessage(Guest guest, ParsecMessage message);
	int onQuit(Guest &guest);
	void setLimit(uint32_t guestUserId, uint8_t padLimit);
	bool toggleMirror(uint32_t guestUserId);
	const PICK_REQUEST pick(Guest guest, int gamepadIndex);
	bool findPreferences(uint32_t guestUserID, function<void(GuestPreferences&)> callback);
	
	vector<Gamepad> gamepads;
	vector<GuestPreferences> guestPreferences;

	bool lock = false;


private:
	bool sendGamepadStateMessage(ParsecGamepadStateMessage& gamepadState, Guest& guest, int& slots);
	bool sendGamepadAxisMessage(ParsecGamepadAxisMessage& gamepadAxis, Guest& guest, int& slots);
	bool sendGamepadButtonMessage(ParsecGamepadButtonMessage& gamepadButton, Guest& guest, int& slots);
	bool sendKeyboardMessage(ParsecKeyboardMessage& keyboard, Guest& guest, int& slots);

	void releaseGamepads();
	void setMirror(uint32_t guestUserId, bool mirror);
	bool tryAssignGamepad(Guest guest, uint32_t padId, int currentSlots, bool isKeyboard);
	bool isRequestState(ParsecMessage message);
	bool isRequestButton(ParsecMessage message);
	bool isRequestKeyboard(ParsecMessage message);

	void reduce(function<void(Gamepad&)> func);
	bool reduceUntilFirst(function<bool(Gamepad&)> func);

	PVIGEM_CLIENT _client;
};