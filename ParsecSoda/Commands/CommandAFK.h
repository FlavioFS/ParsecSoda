#pragma once

#include <sstream>
#include "ACommandIntegerArg.h"
#include "../GamepadClient.h"
#include "../GuestList.h"

class CommandAFK : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::AFK; }

	CommandAFK(GuestList guests, GamepadClient* padClient)
	{

	}

	bool run()
	{
		int clearCount = padClient->clearAFK(guests, guestcount);

		std::ostringstream reply;
		reply << "[ChatBot] | Retrieving AFK gamepads... Gamepads cleared:  " << clearCount << "\0";

		_replyMessage = reply.str();
	}
};
