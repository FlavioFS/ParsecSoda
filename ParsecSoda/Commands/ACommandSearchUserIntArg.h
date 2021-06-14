#pragma once

#include "ACommandPrefix.h"
#include "parsec-dso.h"
#include "../Stringer.h"
#include "../GuestList.h"

class ACommandSearchUserIntArg : public ACommandPrefix
{
public:
	ACommandSearchUserIntArg(const char* msg, vector<const char*> prefixes, GuestList& guests)
		: ACommandPrefix(msg, prefixes), _guests(guests), _searchResult(SEARCH_USER_RESULT::FAILED)
	{}

	bool run()
	{
		if (!ACommandPrefix::run())
		{
			_searchResult = SEARCH_USER_RESULT::FAILED;
			return false;
		}

		try
		{
			std::string str = _msg;

			std::string argNumber = std::string() + str.back();
			_intArg = std::stoi(argNumber);

			std::string args = str.substr(strlen(_prefix));
			std::string targetUsername = args.substr(0, args.size() - 2);


			bool found = false;
			try
			{
				found = _guests.find(stoul(targetUsername), &_targetGuest);
			}
			catch (const std::exception&) {}

			if (!found)
			{
				found = _guests.find(targetUsername, &_targetGuest);
			}

			_searchResult = found ? SEARCH_USER_RESULT::FOUND : SEARCH_USER_RESULT::NOT_FOUND;
			return true;
		}
		catch (const std::exception&)
		{
			_searchResult = SEARCH_USER_RESULT::FAILED;
			return false;
		}
	}

protected:
	SEARCH_USER_RESULT _searchResult;
	GuestList& _guests;
	Guest _targetGuest;
	int _intArg;
};