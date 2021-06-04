#pragma once

#include <sstream>
#include "ACommandIntegerArg.h"
#include "../AudioMix.h"

class CommandSpeakers : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::SPEAKERS; }

	void run(const char * msg, AudioMix * mixer)
	{
		int volume = 0;
		if ( !ACommandIntegerArg::run(msg, "/speakers ", &volume) )
		{
			_replyMessage = std::string() + "[ChatBot] | Usage: /speakers <integer in range [0, 100]>\nExample: /speakers 42\0";
			return;
		}

		mixer->setVolume2(volume / 100.0f);

		std::ostringstream reply;
		reply << "[ChatBot] | Speakers volume set to " << volume << "%\0";
		_replyMessage = reply.str();
	}
};
