#pragma once

#include "ACommandPrefix.h"
#include "../Stringer.h"

class ACommandStringArg : public ACommandPrefix
{
public:
	ACommandStringArg(const char* msg, vector<const char*> prefixes)
		: ACommandPrefix(msg, prefixes), _stringArg("")
	{}

	bool run()
	{
		if (!ACommandPrefix::run())
		{
			return false;
		}

		size_t cmdSize = strlen(_prefix);
		std::string str = _msg;
		_stringArg = str.substr(cmdSize);

		return true;
	}

protected:
	string _stringArg;
};
