#pragma once

#include <sstream>
#include "ACommandIntegerArg.h"
#include "../AudioMix.h"

class CommandMic : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::MIC; }

	CommandMic(const char* msg, AudioMix &mixer)
		: ACommandIntegerArg(msg, internalPrefixes()), _mixer(mixer)
	{}

	bool run() override
	{
		if ( !ACommandIntegerArg::run() )
		{
			_replyMessage = std::string() + "[ChatBot] | Usage: !mic <integer in range [0, 100]>\nExample: !mic 42\0";
			return false;
		}

		_mixer.setVolume1(_intArg / 100.0f);

		std::ostringstream reply;
		reply << "[ChatBot] | Microphone volume set to " << _intArg << "%\0";
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

	AudioMix& _mixer;
};
