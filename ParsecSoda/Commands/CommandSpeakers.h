#pragma once

#include <sstream>
#include "ACommandIntegerArg.h"
#include "../AudioOut.h"

class CommandSpeakers : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::SPEAKERS; }

	CommandSpeakers(const char* msg, AudioOut& audioOut)
		: ACommandIntegerArg(msg, internalPrefixes()), _audioOut(audioOut)
	{}

	bool run() override
	{
		if (!ACommandIntegerArg::run())
		{
			_replyMessage = std::string() + "[ChatBot]\tUsage: !speakers <integer in range [0, 100]>\nExample: !speakers 42\0";
			return false;
		}

		_audioOut.volume = (float)_intArg / 100.0f;

		std::ostringstream reply;
		reply << "[ChatBot]\tSpeakers volume set to " << _intArg << "%\0";
		_replyMessage = reply.str();
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!speakers" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!speakers " };
	}

	AudioOut& _audioOut;
};
