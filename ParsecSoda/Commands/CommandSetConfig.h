#pragma once

#include "parsec.h"
#include "ACommand.h"
#include <iostream>

class CommandSetConfig : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::SETCONFIG; }

	void run(Parsec * ps, ParsecHostConfig * config, const char * sessionId)
	{
		_replyMessage = "[ChatBot] | Room settings applied.\0";
		ParsecHostSetConfig(ps, config, sessionId);
	}
};

