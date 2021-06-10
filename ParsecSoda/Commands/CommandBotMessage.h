#pragma once

#include "parsec.h"
#include "ACommand.h"
#include <iostream>

class CommandBotMessage : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::BOT_MESSAGE; }

	CommandBotMessage(const char* msg)
		: _msg(msg)
	{}

	bool run() override
	{
		_replyMessage = std::string() + "[ChatBot] | " + _msg + "\0";
		return true;
	}

protected:
	string _msg;
};

