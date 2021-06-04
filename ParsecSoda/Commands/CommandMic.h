#pragma once

#include <sstream>
#include "ACommandIntegerArg.h"
#include "../AudioMix.h"

class CommandMic : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::MIC; }

	void run(const char * msg, AudioMix * mixer)
	{
		int volume = 0;
		if ( !ACommandIntegerArg::run(msg, "/mic ", &volume) )
		{
			_replyMessage = std::string() + "[ChatBot] | Usage: /mic <integer in range [0, 100]>\nExample: /mic 42\0";
			return;
		}

		mixer->setVolume1(volume / 100.0f);

		std::ostringstream reply;
		reply << "[ChatBot] | Microphone volume set to " << volume << "%\0";
		_replyMessage = reply.str();
	}
};
