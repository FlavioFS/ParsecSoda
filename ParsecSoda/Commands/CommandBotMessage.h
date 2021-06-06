#pragma once

#include "parsec.h"
#include "ACommand.h"
#include <iostream>

class CommandBotMessage : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::BOT_MESSAGE; }

	void run(const char * msg)
	{
		_replyMessage = std::string() + "[ChatBot] | " + msg + "\0";
	}
};

