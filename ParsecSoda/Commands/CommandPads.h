#pragma once

#include <sstream>
#include "ACommandSearchUserIntArg.h"
#include "../GamepadClient.h"

class CommandPads : public ACommandSearchUserIntArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::PADS; }

	void run(const char * msg, ParsecGuest * guests, int guestCount, GamepadClient * padClient)
	{
		ParsecGuest targetUser;
		int padLimit = XUSER_MAX_COUNT;
		if ( !ACommandSearchUserIntArg::run(msg, "/pads ", guests, guestCount, &targetUser, &padLimit) )
		{
			_replyMessage = "[ChatBot] | Usage: /pads <username> <number>\nExample: /pads melon 2\0";
			return;
		}

		padClient->setLimit(targetUser.userID, padLimit);

		std::ostringstream reply;
		reply << "[ChatBot] | " << targetUser.name << " gamepad limit set to " << padLimit << "\0";
		_replyMessage = reply.str();
	}
};
