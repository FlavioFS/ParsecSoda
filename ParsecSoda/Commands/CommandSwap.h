#pragma once

#include <iostream>
#include <sstream>
#include "ACommandIntegerArg.h"
#include "../GamepadClient.h"

class CommandSwap : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::SWAP; }

	CommandSwap(const char* msg, Guest &sender, GamepadClient  &gamepadClient)
		: ACommandIntegerArg(msg, internalPrefixes()), _sender(sender), _gamepadClient(gamepadClient)
	{}

	bool run() override
	{
		if (!ACommandIntegerArg::run())
		{
			_replyMessage = "[ChatBot] | Usage: !swap <integer in range [1, 4]>\nExample: !swap 4\0";
			return false;
		}

		GAMEPAD_PICK_REQUEST result = _gamepadClient.pick(_sender, _intArg-1);

		bool rv = false;
		std::ostringstream reply;

		switch (result)
		{
		case GAMEPAD_PICK_REQUEST::OK:
			reply
				<< "[ChatBot] | Gamepad " << _intArg << " was given to " << _sender.name << "\t(#" << _sender.userID << ")\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			rv = true;
			break;
		case GAMEPAD_PICK_REQUEST::SAME_USER:
			reply
				<< "[ChatBot] | " << _sender.name << ", you have that gamepad already.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GAMEPAD_PICK_REQUEST::TAKEN:
			reply
				<< "[ChatBot] | " << _sender.name << ", the gamepad you tried to pick is already taken.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GAMEPAD_PICK_REQUEST::EMPTY_HANDS:
			reply
				<< "[ChatBot] | " << _sender.name << ", you must be holding a gamepad to use !swap command.\n"
				<< "\t\tPress any face button (A, B, X, Y) to receive a random gamepad (if available).\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GAMEPAD_PICK_REQUEST::LIMIT_BLOCK:
			reply
				<< "[ChatBot] | " << _sender.name << ", your current gamepad limit is set to 0.\0";
			break;
		case GAMEPAD_PICK_REQUEST::OUT_OF_RANGE:
			reply
				<< "[ChatBot] | " << _sender.name << ", your gamepad index is wrong (valid range is [1, 4]).\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		default:
			break;
		}

		_replyMessage = reply.str();
		return rv;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!swap", "!pick" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!swap ", "!pick " };
	}
	string _msg;
	Guest& _sender;
	GamepadClient& _gamepadClient;
};

