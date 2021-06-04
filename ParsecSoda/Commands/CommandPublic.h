#pragma once

#include "parsec.h"
#include "ACommand.h"
#include <iostream>

class CommandPublic : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::PUBLIC; }

	void run(ParsecHostConfig * config)
	{
		_replyMessage = "[ChatBot] | Room set to public.\0";
		config->publicGame = true;
	}
};

