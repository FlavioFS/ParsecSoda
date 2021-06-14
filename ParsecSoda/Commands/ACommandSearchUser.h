#pragma once

#include "ACommandPrefix.h"
#include "parsec-dso.h"
#include "../Stringer.h"
#include "../GuestList.h"

enum class SEARCH_USER_RESULT
{
	FAILED = -1,
	NOT_FOUND = 0,
	FOUND = 1
};

class ACommandSearchUser : public ACommandPrefix
{
public:
	ACommandSearchUser(const char* msg, vector<const char*> prefixes, GuestList &guests)
		: ACommandPrefix(msg, prefixes), _guests(guests),
		_searchResult(SEARCH_USER_RESULT::FAILED), _targetGuest(Guest())
	{}

	bool run() override
	{
		if (!ACommandPrefix::run())
		{
			_searchResult = SEARCH_USER_RESULT::FAILED;
			return false;
		}

		try
		{
			std::string str = _msg;
			size_t cmdSize = strlen(_prefix);
			std::string targetUsername = str.substr(cmdSize);

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
			return found;
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
};

