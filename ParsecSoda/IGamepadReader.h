#pragma once

#include <vector>
#include "GamepadState.h"

class IGamepadReader
{
public:
	virtual std::vector<GamepadState> readInputs() = 0;
};