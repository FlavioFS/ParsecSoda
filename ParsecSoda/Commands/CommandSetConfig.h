#pragma once

#include "parsec-dso.h"
#include "ACommand.h"
#include <iostream>

class CommandSetConfig : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::SETCONFIG; }

	CommandSetConfig(ParsecDSO* parsec, ParsecHostConfig* config, const char* sessionId)
		: _parsec(parsec), _config(config), _sessionId(sessionId)
	{}

	bool run() override
	{
		_replyMessage = "[ChatBot]\tRoom settings applied.\0";
		ParsecHostSetConfig(_parsec, _config, _sessionId);
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!setconfig" };
	}

protected:
	ParsecDSO* _parsec;
	ParsecHostConfig* _config;
	const char* _sessionId;
};