#pragma once

#include <Windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <vector>
#include <iostream>
#include "parsec.h"
#include "Bitwise.h"
#include "KeyboardMaps.h"
#include "Guest.h"

using namespace std;

#define GAMEPAD_INDEX_ERROR -1
#define GAMEPAD_KEYBOARD_PAD_ID -2
#define OWNER_ID_NONE -1
#define GAMEPAD_DEADZONE 4096

#define GAMEPAD_STICK_MIN -32768
#define GAMEPAD_STICK_MAX 32767
#define GAMEPAD_TRIGGER_MIN 0
#define GAMEPAD_TRIGGER_MAX 255

typedef struct GamepadStatus
{
	Guest owner = Guest();
	uint32_t ownerPadId;
	bool isConnected = false;
} GamepadStatus;

class Gamepad
{
public:
	Gamepad();
	Gamepad(PVIGEM_CLIENT client);
	bool connect();
	bool disconnect();
	void release();
	bool isAttached();
	bool refreshIndex();
	ULONG getIndex() const;
	XINPUT_STATE getState();
	void clearState();
	bool setState(ParsecGamepadStateMessage state);
	bool setState(ParsecKeyboardMessage keys);
	bool setState(ParsecGamepadButtonMessage buttons);
	bool setState(ParsecGamepadAxisMessage axis);
	void setPadId(uint32_t padId);
	void setOwner(Guest& owner, bool mirror = false);
	void setOwner(Guest owner, uint32_t padId, bool mirror);
	void setOwnerGuest(ParsecGuest guest, uint32_t padId, bool mirror = false);
	void copyOwner(Gamepad pad);
	void clearOwner();
	const bool isOwned();
	Guest& getOwner();
	uint32_t& getOwnerPadId();
	bool& isConnected();
	void onRageQuit();
	void setMirror(bool mirror);
	GamepadStatus getStatus();

private:
	void setOwnerGuest (
		uint32_t ownerUserId = (uint32_t)GUEST_ID_ERRORS::NONE,
		uint32_t ownerIndex = (uint32_t)GUEST_ID_ERRORS::NONE,
		const char * ownerName = "",
		uint32_t padId = GAMEPAD_INDEX_ERROR, bool mirror = false
	);

	PVIGEM_CLIENT _client;
	PVIGEM_TARGET pad;
	ULONG _index = -1;
	bool _isAlive = false;
	bool _isConnected = false;
	Guest _owner;
	uint32_t _ownerPadId = (uint32_t)GUEST_ID_ERRORS::NONE;
	bool _mirror = false;
};