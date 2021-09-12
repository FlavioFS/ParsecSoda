#pragma once

#include <vector>
#include "SDLGamepad.h"

using namespace std;

class SDLGamepadReader
{
public:
	vector<SDLGamepad>& getGamepads();
	void fetchGamepads();

private:
	void eraseDetachedGamepads();
	bool contains(SDL_Joystick* joystick);
	static GamepadState joystickToGamepadState(SDL_Joystick* pad, bool isXInput = false);

	vector<SDLGamepad> _gamepads;
};

