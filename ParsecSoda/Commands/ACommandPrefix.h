#pragma once

#include "ACommand.h"
#include "../Stringer.h"

class ACommandPrefix : public ACommand
{
public:
	bool run(const char * msg, const char * prefix)
	{
		if (!Stringer::startsWithPattern(msg, prefix))
		{
			return false;
		}

		return true;
	}
};
