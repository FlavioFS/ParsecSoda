#pragma once

#include "parsec.h"

class KeyboardMap
{
public:
	static KeyboardMap defaultMap() {
		return KeyboardMap();
	};

	// Left Stick
	uint16_t LLeft = KEY_A;
	uint16_t LRight = KEY_D;
	uint16_t LUp = KEY_W;
	uint16_t LDown = KEY_S;

	// Right Stick
	uint16_t RLeft = KEY_LEFT;
	uint16_t RRight = KEY_RIGHT;
	uint16_t RUp = KEY_UP;
	uint16_t RDown = KEY_DOWN;

	// DPad
	uint16_t DLeft = KEY_KP_4;
	uint16_t DRight = KEY_KP_6;
	uint16_t DUp = KEY_KP_8;
	uint16_t DDown = KEY_KP_5;

	// Face Buttons
	uint16_t A = KEY_L;
	uint16_t B = KEY_O;
	uint16_t X = KEY_K;
	uint16_t Y = KEY_I;

	// Top
	uint16_t LB = KEY_Q;
	uint16_t RB = KEY_P;
	uint16_t LT = KEY_F;
	uint16_t RT = KEY_J;

	// Menu
	uint16_t Back = KEY_BACKSPACE;
	uint16_t Start = KEY_ENTER;
	uint16_t Guide = KEY_BACKTICK;

	// Stick Buttons
	uint16_t LThumb = KEY_C;
	uint16_t RThumb = KEY_M;
};

//enum class KEY_TO_GAMEPAD {
//	
//	// Left Stick
//	LEFT = KEY_A,
//	RIGHT = KEY_D,
//	UP = KEY_W,
//	DOWN = KEY_S,
//
//	// Right Stick
//	RLEFT = KEY_LEFT,
//	RRIGHT = KEY_RIGHT,
//	RUP = KEY_UP,
//	RDOWN = KEY_DOWN,
//
//	// DPad
//	DLEFT = KEY_KP_4,
//	DRIGHT = KEY_KP_6,
//	DUP = KEY_KP_8,
//	DDOWN = KEY_KP_5,
//	
//	// Face Buttons
//	A = KEY_L,
//	B = KEY_O,
//	X = KEY_K,
//	Y = KEY_I,
//
//	// Menu
//	BACK = KEY_BACKSPACE,
//	START = KEY_ENTER,
//
//	// Top
//	LB = KEY_Q,
//	RB = KEY_P,
//	LT = KEY_F,
//	RT = KEY_J,
//
//	// Stick buttons
//	LTHUMB = KEY_C,
//	RTHUMB = KEY_M
//
//};

//enum class KEY_TO_GAMEPAD2
//{
//	LEFT = KEY_LEFT,
//	RIGHT = KEY_RIGHT,
//	UP = KEY_UP,
//	DOWN = KEY_DOWN,
//	A = KEY_KP_1,
//	B = KEY_KP_2,
//	X = KEY_KP_4,
//	Y = KEY_KP_5,
//	BACK = KEY_KP_MULTIPLY,
//	START = KEY_KP_MINUS,
//	LB = KEY_KP_7,
//	RB = KEY_KP_8,
//	LT = KEY_KP_6,
//	RT = KEY_KP_3,
//	LTHUMB = KEY_KP_0,
//	RTHUMB = KEY_KP_PERIOD
//};