#pragma once

#include <sstream>
#include "ACommandIntegerArg.h"
#include "../GamepadClient.h"

class CommandDC : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::DC; }

	CommandDC(const char* msg, GamepadClient& gamepadClient)
		: ACommandIntegerArg(msg, internalPrefixes()), _gamepadClient(gamepadClient)
	{}

	bool run() override
	{
		if ( !ACommandIntegerArg::run() )
		{
			_replyMessage = std::string() + "[ChatBot] | Usage: !dc <integer in range [1, 4]>\nExample: !dc 3\0";
			return false;
		}

		std::ostringstream reply;
		if (_intArg < 1 || _intArg > XUSER_MAX_COUNT)
		{
			reply << "[ChatBot] | Wrong index: " << _intArg << " is not in range [1, 4].\0";
		}

		if (_gamepadClient.disconnect(_intArg - 1))
		{
			reply << "[ChatBot] | Gamepad " << _intArg << " disconnected.\0";
		}
		else
		{
			reply << "[ChatBot] | Gamepad " << _intArg << " fail to disconnect.\0";
		}

		_replyMessage = reply.str();
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!dc" };
	}

private:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!dc " };
	}

	GamepadClient &_gamepadClient;

};
