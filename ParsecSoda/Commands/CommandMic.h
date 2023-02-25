#pragma once

#include <sstream>
#include "ACommandIntegerArg.h"
#include "../AudioIn.h"

class CommandMic : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::MIC; }

	CommandMic(const char* msg, AudioIn &audioIn)
		: ACommandIntegerArg(msg, internalPrefixes()), _audionIn(audioIn)
	{}

	bool run() override
	{
		if ( !ACommandIntegerArg::run() )
		{
			_replyMessage = std::string() + "[ChatBot]\tUsage: !mic <integer in range [0, 100]>\nExample: !mic 42\0";
			return false;
		}

		_audionIn.volume = (float)_intArg / 100.0f;

		std::ostringstream reply;
		reply << "[ChatBot]\tMicrophone volume set to " << _intArg << "%\0";
		_replyMessage = reply.str();
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!mic" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!mic " };
	}

	AudioIn& _audionIn;
};
