#pragma once

#include <sstream>
#include "ACommand.h"
#include "../GamepadClient.h"

class CommandOne : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::ONE; }

	CommandOne(Guest &sender, GamepadClient &gamepadClient)
		: _sender(sender), _gamepadClient(gamepadClient)
	{}

	bool run() override
	{
		bool isOne = _gamepadClient.toggleIgnoreDeviceID(_sender.userID);
		_replyMessage = std::string()
			+ "[ChatBot] | " + _sender.name
			+ " toggled ignore device ID mode:\t" + (isOne ? "ON" : "OFF")
			+ "\0";
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!one" };
	}

protected:

	Guest& _sender;
	GamepadClient& _gamepadClient;
};
