#pragma once

#include <sstream>
#include "ACommand.h"
#include "../GamepadClient.h"

class CommandMirror : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::MIRROR; }

	void run(ParsecGuest sender, GamepadClient * padClient)
	{
		bool isMirrored = padClient->toggleMirror(sender.userID);
		_replyMessage = std::string()
			+ "[ChatBot] | " + sender.name
			+ " toggled mirror mode:\t" + (isMirrored ? "ON" : "OFF")
			+ "\0";
	}
};
