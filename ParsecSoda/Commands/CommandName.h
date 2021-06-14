#pragma once

#include "parsec-dso.h"
#include "ACommandStringArg.h"

class CommandName : public ACommandStringArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::NAME; }
	
	CommandName(const char* msg, ParsecHostConfig &config)
		: ACommandStringArg(msg, internalPrefixes()), _config(config)
	{}

	bool run() override
	{
		if ( !ACommandStringArg::run() )
		{
			_replyMessage = "[ChatBot] | Usage: !name <roomname>\nExample: !name Let's Play Gauntlet!\0";
			return false;
		}

		// Replaces \ + n with \n
		size_t index = 0;
		while (true)
		{
			index = _stringArg.find("\\n", index);
			if (index == std::string::npos) break;

			_stringArg.replace(index, 2, "\n");
			index++;
		}

		strcpy_s(_config.name, _stringArg.c_str());

		_replyMessage = std::string() + "[ChatBot] | Room name changed:\n" + _stringArg + "\0";
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!name" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!name " };
	}
	ParsecHostConfig& _config;
};
