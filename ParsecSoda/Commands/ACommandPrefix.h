#pragma once

#include "ACommand.h"
#include "../Stringer.h"
#include <vector>

class ACommandPrefix : public ACommand
{
public:
	ACommandPrefix(const char* msg, vector<const char*> prefixes)
		: _msg(msg), _prefixes(prefixes), _prefix("")
	{}

	bool run() override
	{
		vector<const char*>::iterator prefix = _prefixes.begin();
		for (; prefix != _prefixes.end(); ++prefix)
		{
			if (Stringer::startsWithPattern(_msg, *prefix))
			{
				_prefix = *prefix;
				return true;
			}
		}

		return false;
	}

protected:
	const char* _msg;
	const char* _prefix;
	vector<const char*> _prefixes;
};
