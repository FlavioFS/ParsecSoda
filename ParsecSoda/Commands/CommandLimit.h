#pragma once

#include <sstream>
#include "ACommandSearchUserIntArg.h"
#include "../GamepadClient.h"

class CommandLimit : public ACommandSearchUserIntArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::LIMIT; }

	void run(const char * msg, ParsecGuest * guests, int guestCount, GamepadClient * padClient)
	{
		ParsecGuest targetUser;
		int padLimit = XUSER_MAX_COUNT;
		if ( !ACommandSearchUserIntArg::run(msg, "/limit ", guests, guestCount, &targetUser, &padLimit) )
		{
			_replyMessage = "[ChatBot] | Usage: /limit <username> <number>\nExample: /limit melon 2\0";
			return;
		}

		padClient->setLimit(targetUser.userID, padLimit);

		std::ostringstream reply;
		reply << "[ChatBot] | " << targetUser.name << " gamepad limit set to " << padLimit << "\0";
		_replyMessage = reply.str();
	}
};
