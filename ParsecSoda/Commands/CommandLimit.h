#pragma once

#include <sstream>
#include "ACommandSearchUserIntArg.h"
#include "../GamepadClient.h"

class CommandLimit : public ACommandSearchUserIntArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::LIMIT; }

	CommandLimit(const char* msg, GuestList &guests, GamepadClient &gamepadClient)
		: ACommandSearchUserIntArg(msg, internalPrefixes(), guests), _gamepadClient(gamepadClient)
	{}

	bool run() override
	{
		if ( !ACommandSearchUserIntArg::run() )
		{
			_replyMessage = "[ChatBot]\tUsage: !limit <username> <number>\nExample: !limit melon 2\0";
			return false;
		}

		_gamepadClient.setLimit(_targetGuest.userID, _intArg);

		std::ostringstream reply;
		reply << "[ChatBot]\t" << _targetGuest.name << " gamepad limit set to " << _intArg << "\0";
		_replyMessage = reply.str();
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!limit" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!limit " };
	}

	GamepadClient& _gamepadClient;
};
