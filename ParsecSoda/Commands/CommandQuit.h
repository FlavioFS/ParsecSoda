#pragma once

#include "ACommand.h"

class CommandQuit : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::QUIT; }

	CommandQuit()
	{
		_replyMessage = "[ChatBot] | Closing stream...\0";
	}

	void run(bool * isRunning)
	{
		*isRunning = false;
	}
};

