#pragma once

#include "Gamepad.h"

#define DS4_BYTE_MIN -128
#define DS4_BYTE_MAX 127

class DualshockGamepad : public Gamepad
{
public:
	DualshockGamepad() : Gamepad() {}
	DualshockGamepad(ParsecDSO* parsec, PVIGEM_CLIENT client) : Gamepad(parsec, client) {}
	bool alloc();
	bool connect();
	bool refreshIndex();
	XINPUT_STATE fetchXInputState();
	void clearState();

	// State mesages
	void setState(XINPUT_STATE state);
	void setStateSafe(XINPUT_STATE state);

private:
	DS4_REPORT XInputToDS4(XINPUT_GAMEPAD& state);
	BYTE ShortToByteAxis(SHORT axis);
	
};

