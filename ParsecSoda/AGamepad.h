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
#include "Dice.h"

using namespace std;

#define GAMEPAD_INDEX_ERROR -1
#define GAMEPAD_DEADZONE 4096

#define GAMEPAD_SHORT_MIN -32768
#define GAMEPAD_SHORT_MAX 32767
#define GAMEPAD_BYTE_MIN -128
#define GAMEPAD_NYTE_MAX 127
#define GAMEPAD_UBYTE_MIN 0
#define GAMEPAD_UNYTE_MAX 255

class AGamepad
{
public:
	enum class Type
	{
		INVALID = -1,
		XBOX,
		DUALSHOCK
	};

	class Keyboard
	{
	public:
		void clear() {
			LLeft = LRight = LUp = LDown = RLeft = RRight = RUp = RDown = false;
		};
		bool LLeft = false;
		bool LRight = false;
		bool LUp = false;
		bool LDown = false;
		bool RLeft = false;
		bool RRight = false;
		bool RUp = false;
		bool RDown = false;
	};

	virtual const AGamepad::Type type() const { return AGamepad::Type::INVALID; }

	AGamepad();
	AGamepad(ParsecDSO * parsec, PVIGEM_CLIENT client);
	bool realloc();
	bool disconnect();
	void release();
	bool isAttached();
	void setIndex(ULONG index);
	ULONG getIndex() const;
	XINPUT_STATE getState();
	Keyboard& getKeyboard();

	void setOwner(Guest& guest, uint32_t deviceID, bool isKeyboard);
	void copyOwner(AGamepad* pad);
	const bool isOwned();
	bool isConnected() const;
	const bool isLocked() const;
	void setLocked(const bool value);
	void toggleLocked();

	// Child specific implementations
	virtual bool alloc() = 0;
	virtual bool connect() = 0;
	virtual void clearState() = 0;
	virtual void clearOwner();
	virtual bool refreshIndex() = 0;
	virtual void setState(XINPUT_STATE state) = 0;
	virtual void setStateSafe(XINPUT_STATE state) = 0;

	GuestDevice owner = GuestDevice();
	bool isPuppet = false;
	ParsecDSO * parsec;

protected:
	Keyboard _keyboard;
	PVIGEM_CLIENT _client;
	PVIGEM_TARGET _pad;
	ULONG _index = -1;
	bool _isAlive = false;
	bool _isConnected = false;
	bool _isLocked = false;

	XINPUT_STATE _currentState;
};