#pragma once

#include "XBoxGamepad.h"
#include "DualshockGamepad.h"
#include <Windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <thread>
#include "GuestData.h"
#include "KeyboardMaps.h"
#include "GuestList.h"
#include "MetadataCache.h"

using namespace std;

#define GAMEPAD_LIMIT_NOT_FOUND -1

class GamepadClient
{
public:
	enum class PICK_REQUEST
	{
		OK,
		DISCONNECTED,
		SAME_USER,
		TAKEN,
		EMPTY_HANDS,
		LIMIT_BLOCK,
		OUT_OF_RANGE,
		PUPPET
	};

	class GuestPreferences
	{
	public:
		GuestPreferences()
			: userID(0), padLimit(1), mirror(false), ignoreDeviceID(true)
		{}
		GuestPreferences(uint32_t userID, int padLimit = 1, bool mirror = false, bool ignoreDeviceID = true)
			: userID(userID), padLimit(padLimit), mirror(mirror), ignoreDeviceID(ignoreDeviceID)
		{}

		uint32_t userID = 0;
		int padLimit = 1;
		bool mirror = false;
		bool ignoreDeviceID = true;
	};

	~GamepadClient();
	void setParsec(ParsecDSO* parsec);
	bool init();
	AGamepad* createGamepad(AGamepad::Type type = AGamepad::Type::XBOX);
	void createAllGamepads();
	void connectAllGamepads();
	void disconnectAllGamepads();
	void sortGamepads();
	void resize(size_t xboxCount, size_t dualshockCount);
	void resetAll();
	void toggleLock();
	void toggleLockGamepad(int index);
	AGamepad* connectNextGamepad();
	void release();
	AGamepad* getGamepad(int index);
	int clearAFK(GuestList &guests);

	bool disconnect(int gamepadIndex);
	bool clearOwner(int gamepadIndex);

	bool sendMessage(Guest guest, ParsecMessage message);
	int onQuit(Guest &guest);
	void setLimit(uint32_t guestUserId, uint8_t padLimit);
	bool toggleMirror(uint32_t guestUserID);
	bool toggleIgnoreDeviceID(uint32_t guestUserID);
	const PICK_REQUEST pick(Guest guest, int gamepadIndex);
	bool findPreferences(uint32_t guestUserID, function<void(GuestPreferences&)> callback);

	bool isBusy();
	
	vector<AGamepad*> gamepads;
	vector<GuestPreferences> guestPreferences;

	bool lock = false;
	bool isPuppetMaster = false;


private:
	bool _isBusy = false;

	bool sendGamepadStateMessage(ParsecGamepadStateMessage& gamepadState, Guest& guest, int& slots, GuestPreferences prefs = GuestPreferences());
	bool sendGamepadAxisMessage(ParsecGamepadAxisMessage& gamepadAxis, Guest& guest, int& slots, GuestPreferences prefs = GuestPreferences());
	bool sendGamepadButtonMessage(ParsecGamepadButtonMessage& gamepadButton, Guest& guest, int& slots, GuestPreferences prefs = GuestPreferences());
	bool sendKeyboardMessage(ParsecKeyboardMessage& keyboard, Guest& guest, int& slots, GuestPreferences prefs = GuestPreferences());

	void releaseGamepads();
	void setMirror(uint32_t guestUserID, bool mirror);
	void setIgnoreDeviceID(uint32_t guestUserID, bool ignoreDeviceID);
	bool tryAssignGamepad(Guest guest, uint32_t padId, int currentSlots, bool isKeyboard, GuestPreferences prefs = GuestPreferences());
	bool isRequestState(ParsecMessage message);
	bool isRequestButton(ParsecMessage message);
	bool isRequestKeyboard(ParsecMessage message);

	void reduce(function<void(AGamepad*)> func);
	bool reduceUntilFirst(function<bool(AGamepad*)> func);

	XINPUT_STATE toXInput(ParsecGamepadStateMessage& state, XINPUT_STATE previousState, GuestPreferences& prefs);
	XINPUT_STATE toXInput(ParsecGamepadButtonMessage& button, XINPUT_STATE previousState, GuestPreferences& prefs);
	XINPUT_STATE toXInput(ParsecGamepadAxisMessage& axis, XINPUT_STATE previousState, GuestPreferences& prefs);
	XINPUT_STATE toXInput(ParsecKeyboardMessage& key, AGamepad::Keyboard& keyboard, XINPUT_STATE previousState, GuestPreferences& prefs);


	PVIGEM_CLIENT _client;
	ParsecDSO* _parsec;

	thread _resetAllThread;

	KeyboardMap _keyboardMap;
};