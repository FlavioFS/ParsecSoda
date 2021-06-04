#pragma once

#include "ACommandPrefix.h"

class ACommandIntegerArg : public ACommandPrefix
{
public:
	bool run(const char * msg, const char * prefix, int * result)
	{
		if (!ACommandPrefix::run(msg, prefix))
		{
			return false;
		}

		try
		{
			std::string str = msg;
			size_t cmdSize = strlen(prefix);
			*result = std::stoi(str.substr(cmdSize));

			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}
};
