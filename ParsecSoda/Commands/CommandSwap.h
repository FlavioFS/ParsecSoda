#pragma once

#include "ACommandIntegerArg.h"
#include "../GamepadClient.h"
#include <iostream>
#include <sstream>

class CommandSwap : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::SWAP; }

	void run(const char* msg, ParsecGuest guest, GamepadClient * padClient)
	{
		int gamepadIndex = -1;
		if (!ACommandIntegerArg::run(msg, "/swap ", &gamepadIndex))
		{
			_replyMessage = "[ChatBot] | Usage: /swap <integer in range [1, 4]>\nExample: /swap 4\0";
			return;
		}

		GAMEPAD_PICK_REQUEST result = padClient->pick(guest, gamepadIndex-1);

		std::ostringstream reply;
		switch (result)
		{
		case GAMEPAD_PICK_REQUEST::OK:
			reply
				<< "[ChatBot] | Gamepad " << gamepadIndex << " was given to " << guest.name << "\t(#" << guest.userID << ")\n"
				<< "\t\tType /pads to see the gamepad list.\0";
			break;
		case GAMEPAD_PICK_REQUEST::SAME_USER:
			reply
				<< "[ChatBot] | " << guest.name << ", you have that gamepad already.\n"
				<< "\t\tType /pads to see the gamepad list.\0";
			break;
		case GAMEPAD_PICK_REQUEST::TAKEN:
			reply
				<< "[ChatBot] | " << guest.name << ", the gamepad you tried to pick is already taken.\n"
				<< "\t\tType /pads to see the gamepad list.\0";
			break;
		case GAMEPAD_PICK_REQUEST::EMPTY_HANDS:
			reply
				<< "[ChatBot] | " << guest.name << ", you must be holding a gamepad to use /swap command.\n"
				<< "\t\tPress any face button (A, B, X, Y) to receive a random gamepad (if available).\n"
				<< "\t\tType /pads to see the gamepad list.\0";
			break;
		case GAMEPAD_PICK_REQUEST::LIMIT_BLOCK:
			reply
				<< "[ChatBot] | " << guest.name << ", your current gamepad limit is set to 0.\0";
			break;
		case GAMEPAD_PICK_REQUEST::OUT_OF_RANGE:
			reply
				<< "[ChatBot] | " << guest.name << ", your gamepad index is wrong (valid range is [1, 4]).\n"
				<< "\t\tType /pads to see the gamepad list.\0";
			break;
		default:
			break;
		}

		_replyMessage = reply.str();
	}
};

