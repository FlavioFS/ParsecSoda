#pragma once

#include "ACommand.h"
#include <iostream>
#include "../GamepadClient.h"
#include <sstream>

class CommandOwners : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::OWNERS; }

	void run(GamepadClient * padClient)
	{
		std::vector<GuestData> owners = padClient->getOwners();
		
		std::ostringstream reply;
		reply << "[ChatBot] | Gamepad Holders:\n";

		std::vector<GuestData>::iterator it = owners.begin();
		uint16_t i = 1;
		for (; it != owners.end(); ++it)
		{
			reply << "\t\t[" << i << "] \t";
			if ((*it).userId == OWNER_ID_NONE)
			{
				reply << "\n";
			}
			else
			{
				reply << "(" << (*it).userId << ")\t" << (*it).name << "\n";
			}
			++i;
		}
		reply << "\0";

		_replyMessage = reply.str();
	}
};

