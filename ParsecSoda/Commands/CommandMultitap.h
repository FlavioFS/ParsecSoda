#pragma once

#include <sstream>
#include "ACommand.h"
#include "../GamepadClient.h"

class CommandMultitap : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::MULTITAP; }

	CommandMultitap(Guest &sender, GamepadClient &gamepadClient)
		: _sender(sender), _gamepadClient(gamepadClient)
	{}

	bool run() override
	{
		bool isMultitap = _gamepadClient.toggleMultitap(_sender.userID);
		_replyMessage = std::string()
			+ "[ChatBot] | " + _sender.name
			+ " toggled multitap:\t" + (isMultitap ? "ON" : "OFF")
			+ "\0";
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!multitap", "!one" };
	}

protected:

	Guest& _sender;
	GamepadClient& _gamepadClient;
};
