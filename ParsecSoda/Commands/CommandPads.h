#pragma once

#include "ACommand.h"
#include <iostream>
#include "../GamepadClient.h"
#include <sstream>

class CommandPads : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::PADS; }

	void run(GamepadClient * padClient)
	{
		std::vector<GamepadStatus> owners = padClient->getGamepadStatus();
		
		std::ostringstream reply;
		reply << "[ChatBot] | Gamepad Holders:\n";

		std::vector<GamepadStatus>::iterator it = owners.begin();
		uint16_t i = 1;
		for (; it != owners.end(); ++it)
		{
			reply << "\t\t"
				<< ((*it).isConnected ? "ON  " : "OFF") << "\t"
				<< "[" << i << "] \t";

			if ((*it).ownerUserId == OWNER_ID_NONE)
			{
				reply << "\n";
			}
			else
			{
				reply << "(" << (*it).ownerUserId << ")\t" << (*it).ownerName << "\n";
			}
			++i;
		}
		reply << "\0";

		_replyMessage = reply.str();
	}
};

