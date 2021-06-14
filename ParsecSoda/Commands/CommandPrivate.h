#pragma once

#include "parsec-dso.h"
#include "ACommand.h"
#include <iostream>

class CommandPrivate : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::PRIVATE; }

	CommandPrivate(ParsecHostConfig& config)
		: _config(config)
	{}

	bool run() override
	{
		_replyMessage = "[ChatBot] | Room set to private.\0";
		_config.publicGame = false;
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!private" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!private " };
	}

	ParsecHostConfig& _config;
};

