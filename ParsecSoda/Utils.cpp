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

	ParsecGuest it;
	for (int i = 0; i < guestCount; i++)
	{
		it = guests[i];
		di = Stringer::fuzzyDistance(it.name, targetUserName);
		if (di <= closestDistance && di <= STRINGER_DISTANCE_CHARS(minimum_bonk_match))
		{
			closestDistance = di;
			*targetUser = guests[i];
			result = true;
		}
	}

	return result;
}
