#pragma once

#include <Windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <vector>
#include <iostream>
#include "parsec.h"
#include "Bitwise.h"

#define GAMEPAD_INDEX_ERROR -1
#define OWNER_ID_NONE 0
#define GAMEPAD_DEADZONE 4096

class Gamepad
{
public:
	Gamepad();
	Gamepad(PVIGEM_CLIENT client);
	bool connect();
	bool disconnect();
	void release();
	bool isAttached();
	ULONG getIndex();
	XINPUT_STATE getState();
	bool setState(ParsecGamepadStateMessage state);
	bool setState(ParsecGamepadButtonMessage buttons);
	bool setState(ParsecGamepadAxisMessage axis);
	void setOwnerGuest(ParsecGuest guest, uint32_t padId, bool mirror = false);
	std::string getOwnerGuestName();
	uint32_t getOwnerGuestUserId();
	uint32_t getOwnerPadId();
	void onRageQuit();
	void setMirror(bool mirror);

private:
	void setOwnerGuest (uint32_t ownerId = OWNER_ID_NONE, const char * ownerName = "", uint32_t padId = GAMEPAD_INDEX_ERROR, bool mirror = false);

	PVIGEM_CLIENT _client;
	PVIGEM_TARGET pad;
	ULONG _index = -1;
	bool _isAlive = false;
	bool _isConnected = false;
	uint32_t _ownerId = OWNER_ID_NONE;
	std::string _ownerName = "";
	uint32_t _ownerPadId = 0;
	bool _mirror = false;
};