#include "GamepadState.h"

GamepadState::GamepadState()
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));
}

GamepadState::GamepadState(XINPUT_STATE state)
	: state(state)
{
}
