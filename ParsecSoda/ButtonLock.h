#pragma once

#include "Bitwise.h"

class ButtonLock
{
public:
	void setButtonLocked(const unsigned int& bitMask, const bool& value)
	{
		Bitwise::setValue(buttons, bitMask, value);
	}

	const bool isButtonLocked(const unsigned int& bitMask)
	{
		return Bitwise::getBit(buttons, bitMask);
	}

	void unlockAll()
	{
		buttons = 0;
		leftTrigger = rightTrigger = leftStick = rightStick = false;
	}

	void lockAll()
	{
		buttons = 0xFFFF;
		leftTrigger = rightTrigger = leftStick = rightStick = true;
	}

	unsigned short buttons{ 0 };
	bool leftTrigger{ false };
	bool rightTrigger{ false };
	bool leftStick{ false };
	bool rightStick{ false };

	bool isEnabled{ false };
};