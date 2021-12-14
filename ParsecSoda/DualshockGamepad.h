#pragma once

#include "AGamepad.h"
#include "ViGEm/Util.h"

#define DS4_BYTE_MIN -128
#define DS4_BYTE_MAX 127

class DualshockGamepad : public AGamepad
{
public:
	const AGamepad::Type type() const override { return AGamepad::Type::DUALSHOCK; }

	DualshockGamepad();
	DualshockGamepad(ParsecDSO* parsec, PVIGEM_CLIENT client);
	
	// AGamepad Interface
	bool alloc() override;
	bool connect() override;
	void clearState() override;
	void clearOwner() override;
	void setState(XINPUT_STATE state) override;
	void setStateSafe(XINPUT_STATE state) override;

private:
	DS4_REPORT XInputToDS4(XINPUT_GAMEPAD& state);
	BYTE ShortToByteAxis(SHORT axis);
	
};

