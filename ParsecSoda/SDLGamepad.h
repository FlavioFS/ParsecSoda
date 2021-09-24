#pragma once

#include <SDL.h>
#include <Windows.h>
#include <ViGEm/Client.h>
#include "GamepadState.h"
#include "Bitwise.h"

#define SDL_AXIS_RANGE (SDL_JOYSTICK_AXIS_MAX - SDL_JOYSTICK_AXIS_MIN)

enum class DInput
{
	Triangle = 0,
	Circle = 1,
	X = 2,
	Square = 3,
	L1 = 4,
	R1 = 5,
	L2 = 6,
	R2 = 7,
	SELECT = 8,
	START = 9,
	L3 = 10,
	R3 = 11
};

enum class XInput
{
	A = 0,
	B = 1,
	X = 2,
	Y = 3,
	LB = 4,
	RB = 5,
	BACK = 6,
	START = 7,
	L3 = 8,
	R3 = 9
};

class SDLGamepad
{
public:
	SDLGamepad(SDL_Joystick* joystick, bool isXInput = true)
		: joystick(joystick), isXInput(isXInput) {}
	
	GamepadState getGamepadState();

	SDL_Joystick* joystick;
	bool isXInput;

private:
	BYTE axisToByte(Sint16 axis);
};