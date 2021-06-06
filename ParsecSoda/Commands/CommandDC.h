#pragma once

#include <sstream>
#include "ACommandIntegerArg.h"
#include "../GamepadClient.h"

class CommandDC : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::DC; }

	void run(const char * msg, GamepadClient *padClient)
	{
		int padIndex = 0;
		if ( !ACommandIntegerArg::run(msg, "/dc ", &padIndex) )
		{
			_replyMessage = std::string() + "[ChatBot] | Usage: /dc <integer in range [1, 4]>\nExample: /dc 3\0";
			return;
		}

		std::ostringstream reply;
		if (padIndex < 1 || padIndex > XUSER_MAX_COUNT)
		{
			reply << "[ChatBot] | Wrong index: " << padIndex << " is not in range [1, 4].\0";
		}

		if (padClient->disconnect(padIndex-1))
		{
			reply << "[ChatBot] | Gamepad " << padIndex << " disconnected.\0";
		}
		else
		{
			reply << "[ChatBot] | Gamepad " << padIndex << " fail to disconnect.\0";
		}

		_replyMessage = reply.str();
	}
};
