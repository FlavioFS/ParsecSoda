#pragma once

#include "parsec-dso.h"
#include "ACommandStringArg.h"
#include "../SFXList.h"

class CommandSFX : public ACommandStringArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::SFX; }
	
	CommandSFX(const char* msg, SFXList &sfxList)
		: ACommandStringArg(msg, internalPrefixes()), _sfxList(sfxList)
	{}

	bool run() override
	{
		if ( !ACommandStringArg::run() )
		{
			_replyMessage =
				string("[ChatBot] | Usage: !sfx <sound name> | Example: !sfx bruh\n") +
				string("List of available sound names:\n") +
				_sfxList.loadedTags() +
				string("\0");
			return false;
		}

		SFXList::SFXPlayResult result = _sfxList.play(_stringArg);

		switch (result)
		{
		case SFXList::SFXPlayResult::COOLDOWN:
			_replyMessage =
				string("[ChatBot] | Command !sfx is on cooldown: ") +
				to_string(_sfxList.getRemainingCooldown()) +
				string(" seconds left.");
			break;
		case SFXList::SFXPlayResult::NOT_FOUND:
			_replyMessage =
				string("[ChatBot] | This sound does not exist.\n") +
				string("List of available sound effects:\n") +
				_sfxList.loadedTags() +
				string("\0");
			break;
		case SFXList::SFXPlayResult::OK:
		default:
			_replyMessage = "";
			break;
		}

		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!sfx" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!sfx " };
	}
	SFXList& _sfxList;
};
