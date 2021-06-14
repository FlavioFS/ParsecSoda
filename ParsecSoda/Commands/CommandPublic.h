#pragma once

#include "parsec-dso.h"
#include "ACommand.h"
#include <iostream>

class CommandPublic : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::PUBLIC; }

	CommandPublic(ParsecHostConfig& config)
		: _config(config)
	{}

	bool run() override
	{
		_replyMessage = "[ChatBot] | Room set to public.\0";
		_config.publicGame = true;
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!public" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!public " };
	}

	ParsecHostConfig& _config;
};

