#pragma once

#include <iostream>
#include <sstream>
#include "ACommandIntegerArg.h"
#include "../GamepadClient.h"

class CommandStrip : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::TAKE; }

	CommandStrip(const char* msg, Guest &sender, GamepadClient  &gamepadClient)
		: ACommandIntegerArg(msg, internalPrefixes()), _sender(sender), _gamepadClient(gamepadClient)
	{}

	bool run() override
	{
		if (!ACommandIntegerArg::run())
		{
			_replyMessage = "[ChatBot]\tUsage: !strip <integer in range [1, 4]>\nExample: !strip 4\0";
			return false;
		}

		bool success = _gamepadClient.clearOwner(_intArg-1);
		if (!success)
		{
			_replyMessage = "[ChatBot]\tUsage: !strip <integer in range [1, 4]>\nExample: !strip 4\0";
			return false;
		}

		std::ostringstream reply;
		reply
			<< "[ChatBot]\tGamepad " << _intArg << " was forcefully dropped by " << _sender.name << "\n"
			<< "\t\tType !pads to see the gamepad list.\0";

		_replyMessage = reply.str();
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!strip" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!strip " };
	}
	string _msg;
	Guest& _sender;
	GamepadClient& _gamepadClient;
};

