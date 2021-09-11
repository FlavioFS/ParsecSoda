#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../Bitwise.h"
#include "../GamepadClient.h"
#include "../Stopwatch.h"
#include "../Debouncer.h"

#include "ProgressCircularWidget.h"
#include "AnimatedGamepadWidget.h"
#include "IconButton.h"
#include "IntRangeWidget.h"
#include "TitleTooltipWidget.h"
#include "ViGEm/Client.h"

#include <Xinput.h>

#include <vector>

#include <cstdint>
#include <iostream>

#include <SDL.h>


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

class MasterOfPuppetsWidget
{
public:
	static bool render(GamepadClient& gamepadClient);
	static void fetchSystemGamepads();

private:
	class Master
	{
	public:
		Master(SDL_Joystick* joystick, bool isXInput = true)
			: joystick(joystick), isXInput(isXInput) {}
		SDL_Joystick* joystick;
		bool isXInput;
	};

	class Puppet
	{
	public:
		Puppet(Gamepad& gamepad, bool isSelected = false)
			: gamepad(gamepad), isSelected(isSelected) {}
		Gamepad& gamepad;
		bool isSelected = false;
	};

	static void renderMasterJoysticks(int& masterIndex, GamepadClient& gamepadClient);
	static void renderPuppets(vector<Gamepad>& gamepads);

	static XINPUT_GAMEPAD sdlJoystickToGamepad(SDL_Joystick* pad, bool isXInput = false);
	static void eraseDetachedJoysticks();
	static bool contains(SDL_Joystick* joystick);
	
	static void mapInputs(int& masterIndex, vector<Gamepad>& gamepads);
	static void setMaster(int value, int& masterIndex, GamepadClient& gamepadClient);
	static void clearPuppets(vector<Gamepad>& gamepads);

	static vector<Master> _masters;
};