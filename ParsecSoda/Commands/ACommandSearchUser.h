#pragma once

#include "ACommandPrefix.h"
#include "parsec.h"
#include "../Stringer.h"
#include "../Utils.h"

enum class SEARCH_USER_RESULT
{
	FAILED = -1,
	NOT_FOUND = 0,
	FOUND = 1
};

class ACommandSearchUser : public ACommandPrefix
{
public:
	SEARCH_USER_RESULT run(const char * msg, const char* prefix, ParsecGuest* guests, int guestCount, ParsecGuest * targetUser)
	{
		if (!ACommandPrefix::run(msg, prefix))
		{
			return SEARCH_USER_RESULT::FAILED;
		}

		try
		{
			std::string str = msg;
			size_t cmdSize = strlen(prefix);
			std::string targetUsername = str.substr(cmdSize);

			bool found = false;
			try
			{
				found = Utils::findUser(stoul(targetUsername), guests, guestCount, targetUser);
			}
			catch (const std::exception&) {}

			if (!found)
			{
				found = Utils::findUser(targetUsername, guests, guestCount, targetUser);
			}
			
			return found ? SEARCH_USER_RESULT::FOUND : SEARCH_USER_RESULT::NOT_FOUND;
		}
		catch (const std::exception&)
		{
			return SEARCH_USER_RESULT::FAILED;
		}
	}
};

