#include "Utils.h"

void Utils::removeCharFromString(std::string *str, char c)
{
	str->erase(std::remove(str->begin(), str->end(), c), str->end());
}

bool Utils::findUser(uint32_t targetUserID, ParsecGuest* guests, int guestCount, ParsecGuest* targetUser)
{
	ParsecGuest it;
	for (int i = 0; i < guestCount; i++)
	{
		it = guests[i];
		if (it.userID == targetUserID)
		{
			*targetUser = guests[i];
			return true;
		}
	}
	return false;
}

bool Utils::findUser(std::string targetUserName, ParsecGuest* guests, int guestCount, ParsecGuest* targetUser)
{
	const uint64_t minimum_bonk_match = 3;
	uint64_t closestDistance = STRINGER_MAX_DISTANCE;
	uint64_t di = STRINGER_MAX_DISTANCE;
	bool result = false;

	if (targetUserName.length() < minimum_bonk_match)
	{
		return false;
	}

	ParsecGuest it;
	for (int i = 0; i < guestCount; i++)
	{
		it = guests[i];
		di = Stringer::fuzzyDistance(it.name, targetUserName);
		if (di <= closestDistance && di <= STRINGER_DISTANCE_CHARS(minimum_bonk_match))
		{
			// If this is a draw, choose one based on following criteria...
			if (di == closestDistance)
			{
				std::string candidateName = it.name;
				std::string currentName = targetUser->name;
				
				// If search tag is shorter than both
				if (targetUserName.length() < candidateName.length() && targetUserName.length() < currentName.length())
				{
					// Pick the shortest
					if (candidateName.length() < currentName.length())
					{
						*targetUser = guests[i];
					}
				}
				// If search tag is larger than any of them
				else
				{
					// Pick the largest
					if (candidateName.length() > currentName.length())
					{
						*targetUser = guests[i];
					}
				}
			}
			else
			{
				*targetUser = guests[i];
			}

			closestDistance = di;
			result = true;
		}
	}

	return result;
}
