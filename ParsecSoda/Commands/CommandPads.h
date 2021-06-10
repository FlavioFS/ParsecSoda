#pragma once

#include "ACommand.h"
#include <iostream>
#include "../GamepadClient.h"
#include <sstream>

class CommandPads : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::PADS; }

	CommandPads(GamepadClient &gamepadClient)
		: _gamepadClient(gamepadClient)
	{}

	bool run() override
	{
		_owners = _gamepadClient.getGamepadStatus();
		
		std::ostringstream reply;
		reply << "[ChatBot] | Gamepad Holders:\n";

		std::vector<GamepadStatus>::iterator it = _owners.begin();
		uint16_t i = 1;
		for (; it != _owners.end(); ++it)
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
		return true;
	}

	const vector<GamepadStatus> owners()
	{
		return _owners;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!pads", "!pad " };
	}

protected:
	GamepadClient& _gamepadClient;
	vector<GamepadStatus> _owners;
};

