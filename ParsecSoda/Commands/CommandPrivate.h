#pragma once

#include "parsec.h"
#include "ACommand.h"
#include <iostream>

class CommandPrivate : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::PRIVATE; }

	void run(ParsecHostConfig * config)
	{
		_replyMessage = "[ChatBot] | Room set to private.\0";
		config->publicGame = false;
	}
};

