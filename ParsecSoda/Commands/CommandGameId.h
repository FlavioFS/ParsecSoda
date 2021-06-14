#pragma once

#include "parsec-dso.h"
#include "ACommandStringArg.h"

class CommandGameId : public ACommandStringArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::GAMEID; }

	CommandGameId(const char* msg, ParsecHostConfig &config)
		: ACommandStringArg(msg, internalPrefixes()), _config(config)
	{}

	bool run() override
	{
		if ( !ACommandStringArg::run() )
		{
			_replyMessage = "[ChatBot] | Usage: !gameid <id>\nExample: !gameid 1RR6JAsP4sdrjUOMEV4i7lVwMht\0";
			return false;
		}

		try
		{
			strcpy_s(_config.gameID, _stringArg.c_str());
		}
		catch (const std::exception&)
		{
			_replyMessage = "[ChatBot] | Failed to write game ID.\0";
			return false;
		}

		_replyMessage = std::string() + "[ChatBot] | Game ID changed:\n" + _stringArg + "\0";
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!gameid" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!gameid " };
	}

	ParsecHostConfig& _config;
};
