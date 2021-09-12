#pragma once

#include <Windows.h>
#include <Xinput.h>

#define XINPUT_AXIS_MIN -32768
#define XINPUT_AXIS_MAX  32767
#define XINPUT_TRIGGER_MIN 0
#define XINPUT_TRIGGER_MAX 255

class GamepadState
{
public:
	GamepadState();
	GamepadState(XINPUT_STATE state);

	XINPUT_STATE state;
};