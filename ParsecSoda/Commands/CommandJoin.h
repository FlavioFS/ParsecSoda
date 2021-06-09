#pragma once

#include "ACommand.h"
#include <iostream>

class CommandJoin : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::JOIN; }

	void run(bool isAdmin)
	{
		_replyMessage = std::string()
			+ "[ChatBot] | You don't need a command to get a gamepad.\n"
			+ "\tJust press (A, B, X or Y) to pick a random controller,\n"
			+ "\tif there are any free controllers available.\0";
	}
};

