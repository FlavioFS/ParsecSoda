#pragma once

#include <sstream>
#include "../GamepadClient.h"
#include "../GuestList.h"

class CommandAFK : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::AFK; }

	CommandAFK(GuestList &guests, GamepadClient& padClient)
		: _guests(guests), _padClient(padClient)
	{}

	bool run() override
	{
		int clearCount = _padClient.clearAFK(_guests);

		std::ostringstream reply;
		reply << "[ChatBot] | Retrieving AFK gamepads... Gamepads cleared:  " << clearCount << "\0";

		_replyMessage = reply.str();

		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!afk" };
	}

private:
	GuestList &_guests;
	GamepadClient &_padClient;
};
