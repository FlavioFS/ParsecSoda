#pragma once

#include <sstream>
#include "ACommandIntegerArg.h"
#include "../GamepadClient.h"

class CommandAFK : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::AFK; }

	void run(ParsecGuest *guests, int guestcount, GamepadClient *padClient)
	{
		int clearCount = padClient->clearAFK(guests, guestcount);

		std::ostringstream reply;
		reply << "[ChatBot] | Retrieving AFK gamepads... Gamepads cleared:  " << clearCount << "\0";

		_replyMessage = reply.str();
	}
};
