#pragma once

#include "ACommandPrefix.h"
#include "parsec.h"
#include "../Stringer.h"
#include "../Utils.h"


class ACommandSearchUserIntArg : public ACommandPrefix
{
public:
	bool run(const char * msg, const char* prefix, ParsecGuest* guests, int guestCount, ParsecGuest * targetUser, int * intArg)
	{
		if (!ACommandPrefix::run(msg, prefix))
		{
			return false;
		}

		try
		{
			std::string str = msg;

			std::string argNumber = std::string() + str.back();
			*intArg = std::stoi(argNumber);

			std::string args = str.substr(strlen(prefix));
			std::string targetUsername = args.substr(0, args.size() - 2);


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
			
			if (!found)
			{
				return false;
			}

			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}
};