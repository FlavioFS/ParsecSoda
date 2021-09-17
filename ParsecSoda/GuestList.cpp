#include "GuestList.h"

void GuestList::setGuests(ParsecGuest* guests, int guestCount)
{
	stringstream comboStringStream;
	_guests.clear();

	for (size_t i = 0; i < guestCount; i++)
	{
		_guests.push_back
		(
			Guest
			(
				guests[i].name,
				guests[i].userID,
				guests[i].id
			)
		);

		if (i < GUESTLIST_MAX_GUESTS)
		{
			guestNames[i] = guests[i].name;
		}
	}
}

vector<Guest>& GuestList::getGuests()
{
    return _guests;
}

void GuestList::clear()
{
	_guests.clear();
}

const bool GuestList::find(uint32_t targetGuestID, Guest* result)
{
	vector<Guest>::iterator i;
	for (i = _guests.begin(); i != _guests.end(); ++i)
	{
		if ((*i).userID == targetGuestID)
		{
			*result = *i;
			return true;
		}
	}

	return false;
}

const bool GuestList::find(const char* targetName, Guest* result)
{
	return find(string(targetName), result);
}

const bool GuestList::find(string targetName, Guest* result)
{
	static const uint64_t MINIMUM_MATCH = 3;
	uint64_t closestDistance = STRINGER_MAX_DISTANCE;
	uint64_t distance = STRINGER_MAX_DISTANCE;
	bool found = false;

	if (targetName.length() < MINIMUM_MATCH)
	{
		return false;
	}

	vector<Guest>::iterator gi;
	for (gi = _guests.begin(); gi != _guests.end(); ++gi)
	{
		distance = Stringer::fuzzyDistance((*gi).name, targetName);
		if (distance <= closestDistance && distance <= STRINGER_DISTANCE_CHARS(MINIMUM_MATCH))
		{
			// If this is a draw, choose one based on following criteria...
			if (distance == closestDistance)
			{
				std::string candidateName = (*gi).name;
				std::string currentName = result->name;

				// If search tag is shorter than both
				if (targetName.length() < candidateName.length() && targetName.length() < currentName.length())
				{
					// Pick the shortest
					if (candidateName.length() < currentName.length())
					{
						*result = *gi;
					}
				}
				// If search tag is larger than any of them
				else
				{
					// Pick the largest
					if (candidateName.length() > currentName.length())
					{
						*result = *gi;
					}
				}
			}
			else
			{
				*result = *gi;
			}

			closestDistance = distance;
			found = true;
		}
	}

	return found;
}
