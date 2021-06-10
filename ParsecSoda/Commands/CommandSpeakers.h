#pragma once

#include <sstream>
#include "ACommandIntegerArg.h"
#include "../AudioMix.h"

class CommandSpeakers : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::SPEAKERS; }

	CommandSpeakers(const char* msg, AudioMix& mixer)
		: ACommandIntegerArg(msg, internalPrefixes()), _mixer(mixer)
	{}

	bool run() override
	{
		if (!ACommandIntegerArg::run())
		{
			_replyMessage = std::string() + "[ChatBot] | Usage: !speakers <integer in range [0, 100]>\nExample: !speakers 42\0";
			return false;
		}

		_mixer.setVolume2(_intArg / 100.0f);

		std::ostringstream reply;
		reply << "[ChatBot] | Speakers volume set to " << _intArg << "%\0";
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

	AudioMix& _mixer;
};
