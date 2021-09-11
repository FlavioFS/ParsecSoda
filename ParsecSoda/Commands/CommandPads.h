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
		std::ostringstream reply;
		reply << "[ChatBot] | Gamepad Holders:\n";

		std::vector<Gamepad>::iterator gi = _gamepadClient.gamepads.begin();
		uint16_t i = 1;
		for (; gi != _gamepadClient.gamepads.end(); ++gi)
		{
			reply << "\t\t"
				<< ((*gi).isConnected() ? "ON  " : "OFF") << "\t"
				<< "[" << i << "] \t";

			if (_gamepadClient.isPuppetMaster && (*gi).isPuppet)
			{
				reply << "(Host) Puppet Master" << "\n";
			}
			else if (!(*gi).owner.guest.isValid())
			{
				reply << "\n";
			}
			else
			{
				reply << "(" << (*gi).owner.guest.userID << ")\t" << (*gi).owner.guest.name << "\n";
			}
			++i;
		}
		reply << "\0";

		_replyMessage = reply.str();
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!pads", "!pad " };
	}

protected:
	GamepadClient& _gamepadClient;
};

