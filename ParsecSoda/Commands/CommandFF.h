#pragma once

#include "ACommand.h"
#include <iostream>
#include "../GamepadClient.h"
#include <sstream>

class CommandFF : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::FF; }

	void run(ParsecGuest guest, GamepadClient * padClient)
	{
		int count = padClient->onRageQuit(guest);

		std::ostringstream reply;
		if (count > 1)
		{
			reply << "[ChatBot] | " << guest.name << " has dropped " << count << " gamepads!\0";
		}
		else if (count > 0)
		{
			reply << "[ChatBot] | " << guest.name << " has dropped " << count << " gamepad!\0";
		}
		else
		{
			reply <<  "[ChatBot] | " << guest.name << " has no gamepads to drop.\0";
		}

		_replyMessage = reply.str();
		reply.clear();
	}
};

