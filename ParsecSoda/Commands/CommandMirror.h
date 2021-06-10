#pragma once

#include <sstream>
#include "ACommand.h"
#include "../GamepadClient.h"

class CommandMirror : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::MIRROR; }

	CommandMirror(Guest &sender, GamepadClient &gamepadClient)
		: _sender(sender), _gamepadClient(gamepadClient)
	{}

	bool run() override
	{
		bool isMirrored = _gamepadClient.toggleMirror(_sender.userID);
		_replyMessage = std::string()
			+ "[ChatBot] | " + _sender.name
			+ " toggled mirror mode:\t" + (isMirrored ? "ON" : "OFF")
			+ "\0";
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!mirror" };
	}

protected:

	Guest& _sender;
	GamepadClient& _gamepadClient;
};
