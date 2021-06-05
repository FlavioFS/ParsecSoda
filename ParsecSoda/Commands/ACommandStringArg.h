#pragma once

#include "ACommandPrefix.h"
#include "../Stringer.h"

class ACommandStringArg : public ACommandPrefix
{
public:
	bool run(const char * msg, const char * prefix, std::string * result)
	{
		if (!ACommandPrefix::run(msg, prefix))
		{
			return false;
		}

		size_t cmdSize = strlen(prefix);
		std::string str = msg;
		*result = str.substr(cmdSize);

		return true;
	}
};
