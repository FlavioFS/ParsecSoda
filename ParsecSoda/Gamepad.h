#pragma once

#include <Windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <vector>
#include <iostream>
#include <functional>
#include "parsec-dso.h"
#include "Bitwise.h"
#include "KeyboardMaps.h"
#include "GuestDevice.h"

using namespace std;

#define GAMEPAD_INDEX_ERROR -1
#define GAMEPAD_DEADZONE 4096

#define GAMEPAD_STICK_MIN -32768
#define GAMEPAD_STICK_MAX 32767
#define GAMEPAD_TRIGGER_MIN 0
#define GAMEPAD_TRIGGER_MAX 255

class Gamepad
{
public:
	Gamepad();
	Gamepad(ParsecDSO * parsec, PVIGEM_CLIENT client);
	bool alloc();
	bool realloc();
	bool connect();
	bool disconnect();
	void release();
	bool isAttached();
	void setIndex(ULONG index);
	ULONG getIndex() const;
	XINPUT_STATE getState();
	void clearState();

	// State mesages
	bool setState(ParsecGamepadStateMessage state);
	bool setState(ParsecKeyboardMessage key);
	bool setState(ParsecGamepadButtonMessage button);
	bool setState(ParsecGamepadAxisMessage axis);

	void setOwner(Guest& guest, uint32_t deviceID, bool isKeyboard);
	void copyOwner(Gamepad pad);
	void clearOwner();
	const bool isOwned();
	bool isConnected() const;
	GuestDevice owner = GuestDevice();
	bool mirror = false;

	ParsecDSO * parsec;

private:
	bool refreshIndex();
	PVIGEM_CLIENT _client;
	PVIGEM_TARGET pad;
	ULONG _index = -1;
	bool _isAlive = false;
	bool _isConnected = false;

	VOID CALLBACK notification(PVIGEM_CLIENT Client,PVIGEM_TARGET Target,UCHAR LargeMotor,UCHAR SmallMotor,UCHAR LedNumber, LPVOID UserData);
};