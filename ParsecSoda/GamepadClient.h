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
#include "HotseatManager.h"

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
			: userID(0), padLimit(1), _isDefaultMirror(true), _isDefaultMultitap(true)
		{}
		GuestPreferences(uint32_t userID, int padLimit = 1, bool isDefaultMirror = true, bool isDefaultMultitap = true)
			: userID(userID), padLimit(padLimit), _isDefaultMirror(isDefaultMirror), _isDefaultMultitap(isDefaultMultitap)
		{}

		uint32_t userID = 0;
		int padLimit = 4;
		
		const bool& isMultitap() const {
			return _isDefaultMultitap ?
				MetadataCache::preferences.defaultMultitapValue :
				!MetadataCache::preferences.defaultMultitapValue;
		}
		const bool& isMirror() const {
			return _isDefaultMirror ?
				MetadataCache::preferences.defaultMirrorValue :
				!MetadataCache::preferences.defaultMirrorValue;
		}

		void toggleMultitap() { _isDefaultMultitap = !_isDefaultMultitap; }
		void toggleMirror() { _isDefaultMirror = !_isDefaultMirror; }

	private:
		bool _isDefaultMultitap = true;
		bool _isDefaultMirror = true;
	};

	~GamepadClient();
	void setParsec(ParsecDSO* parsec);
	void setHotseatManager(HotseatManager* hotSeatManager);
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
	void loadFromHotseats(GuestList& guests);

	bool disconnect(int gamepadIndex);
	bool clearOwner(int gamepadIndex);

	bool sendMessage(Guest guest, ParsecMessage message);
	int onQuit(Guest &guest);
	void setLimit(uint32_t guestUserId, uint8_t padLimit);
	bool toggleMirror(uint32_t guestUserID);
	bool toggleMultitap(uint32_t guestUserID);
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
	bool tryAssignGamepad(Guest guest, uint32_t deviceId, int currentSlots, bool isKeyboard, GuestPreferences prefs = GuestPreferences());
	bool enqueueHotseatRequest(Guest guest, uint32_t deviceId, int currentSlots, bool isKeyboard, GuestPreferences prefs = GuestPreferences());
	bool isRequestState(ParsecMessage message);
	bool isRequestButton(ParsecMessage message);
	bool isRequestKeyboard(ParsecMessage message);

	void reduce(function<void(AGamepad*)> func);
	bool reduceUntilFirst(function<bool(AGamepad*)> func);

	bool isHotseatsActive();

	XINPUT_STATE toXInput(ParsecGamepadStateMessage& state, AGamepad* pad, GuestPreferences& prefs);
	XINPUT_STATE toXInput(ParsecGamepadButtonMessage& button, const XINPUT_STATE& previousState, GuestPreferences& prefs);
	XINPUT_STATE toXInput(ParsecGamepadAxisMessage& axis, const XINPUT_STATE& previousState, GuestPreferences& prefs);
	XINPUT_STATE toXInput(ParsecKeyboardMessage& key, AGamepad::Keyboard& keyboard, const XINPUT_STATE& previousState, GuestPreferences& prefs);
	
	int getDpadSignX(const WORD& wButtons);
	int getDpadSignY(const WORD& wButtons);
	int getDpadSign(const WORD& wButtons, const WORD padDirPositive, const WORD padDirNegative);
	void setAxisFromSign(SHORT& axis, int sign);
	void setDpadFromAxis(WORD& wButtons, const WORD padDirPositive, const WORD padDirNegative, const SHORT& axis);
	void fetchDpadFromAxisX(XINPUT_STATE& state);
	void fetchDpadFromAxisY(XINPUT_STATE& state);
	void fecthAxisFromDpadX(XINPUT_STATE& state);
	void fecthAxisFromDpadY(XINPUT_STATE& state);
	XINPUT_STATE dpadToStick(XINPUT_STATE state);
	XINPUT_STATE stickToDpad(XINPUT_STATE state);
	bool isDpadEqual(XINPUT_STATE before, XINPUT_STATE now);
	bool isStickAsDpadEqual(XINPUT_STATE before, XINPUT_STATE now);


	PVIGEM_CLIENT _client;
	ParsecDSO* _parsec;
	HotseatManager* _hotseatManager;

	thread _resetAllThread;

	KeyboardMap _keyboardMap;
};