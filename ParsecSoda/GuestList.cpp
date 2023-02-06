#include "GuestList.h"

void GuestList::setGuestsSafe(ParsecGuest* guests, int guestCount)
{
	const std::lock_guard<std::mutex> lock(_mutex);
	setGuestsUnsafe(guests, guestCount);
}
void GuestList::setGuestsUnsafe(ParsecGuest* guests, int guestCount)
{
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
	}
}

void GuestList::getGuestsSafe(GuestList::ListCallback callback)
{
	const std::lock_guard<std::mutex> lock(_mutex);
	getGuestsUnsafe(callback);
}
void GuestList::getGuestsUnsafe(GuestList::ListCallback callback)
{
	if (callback)
	{
		callback(_guests);
	}
}

void GuestList::clearSafe()
{
	const std::lock_guard<std::mutex> lock(_mutex);
	clearUnsafe();
}
void GuestList::clearUnsafe()
{
	_guests.clear();
}


const bool GuestList::findSafe(const char* targetName, GuestList::ItemCallback callback)
{
	const std::lock_guard<std::mutex> lock(_mutex);
	return findUnsafe(targetName, callback);
}
const bool GuestList::findUnsafe(const char* targetName, GuestList::ItemCallback callback)
{
	return findSafe(string(targetName), callback);
}


const bool GuestList::findSafe(string targetName, GuestList::ItemCallback callback)
{
	const std::lock_guard<std::mutex> lock(_mutex);
	return findUnsafe(targetName, callback);
}
const bool GuestList::findUnsafe(string targetName, GuestList::ItemCallback callback)
{
	bool found = findIteratorUnsafe(targetName, [&](vector<Guest>::iterator gi) {
		if (callback)
		{
			callback(&*gi);
		}
		});

	return found;
}


const bool GuestList::findSafe(uint32_t userID, GuestList::ItemCallback callback)
{
	const std::lock_guard<std::mutex> lock(_mutex);
	return findUnsafe(userID, callback);
}
const bool GuestList::findUnsafe(uint32_t userID, GuestList::ItemCallback callback)
{
	bool found = findIteratorUnsafe(userID, [&](vector<Guest>::iterator gi) {
		if (callback)
		{
			callback(&*gi);
		}
		});

	return found;
}

const bool GuestList::findByIndexSafe(uint32_t index, GuestList::ItemCallback callback)
{
	const std::lock_guard<std::mutex> lock(_mutex);

	bool found = (index >= 0 && index < _guests.size());
	
	if (found && callback)
	{
		callback(&_guests[index]);
	}

	return found;
}

void GuestList::runThreadSafe(GuestList::Action callback)
{
	if (callback)
	{
		const std::lock_guard<std::mutex> lock(_mutex);
		callback();
	}
}


// ========================================================
// PRIVATE
// ========================================================
const bool GuestList::findIteratorUnsafe(uint32_t userID, GuestList::IteratorCallback callback)
{
	vector<Guest>::iterator gi = _guests.begin();

	for (; gi != _guests.end(); gi++)
	{
		if ((*gi).userID == userID)
		{
			if (callback) { callback(gi); }
			return true;
		}
	}

	return false;
}

const bool GuestList::findIteratorUnsafe(string targetName, GuestList::IteratorCallback callback)
{
	static const uint64_t MINIMUM_MATCH = 3;
	uint64_t closestDistance = STRINGER_MAX_DISTANCE;
	uint64_t distance = STRINGER_MAX_DISTANCE;
	bool found = false;

	if (targetName.length() < MINIMUM_MATCH)
	{
		return false;
	}


	string currentName = "";
	vector<Guest>::iterator result = _guests.end();
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

				// If search tag is shorter than both
				if (targetName.length() < candidateName.length() && targetName.length() < currentName.length())
				{
					// Pick the shortest
					if (candidateName.length() < currentName.length())
					{
						currentName = (*gi).name;
						result = gi;
					}
				}
				// If search tag is larger than any of them
				else
				{
					// Pick the largest
					if (candidateName.length() > currentName.length())
					{
						currentName = (*gi).name;
						result = gi;
					}
				}
			}
			else
			{
				currentName = (*gi).name;
				result = gi;
			}

			closestDistance = distance;
			found = true;
		}
	}

	if (found && callback && result != _guests.end())
	{
		callback(result);
	}

	return found;
}