#pragma once

#include <sstream>
#include "parsec-dso.h"
#include "ACommandIntegerArg.h"

class CommandGuests : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::GUESTS; }

	CommandGuests(const char* msg, ParsecHostConfig &config)
		: ACommandIntegerArg(msg, internalPrefixes()), _config(config)
	{}

	bool run() override
	{
		if ( !ACommandIntegerArg::run() )
		{
			_replyMessage = "[ChatBot] | Usage: !guests <number>\nExample: !guests 7\0";
			return false;
		}

		_config.maxGuests = _intArg;
		std::ostringstream reply;
		reply << "[ChatBot] | Max guests set to " << _intArg << "\0";
		_replyMessage = reply.str();

		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!guests" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!guests " };
	}

	ParsecHostConfig& _config;
};
