#include "GuestList.h"

void GuestList::setGuests(ParsecGuest* guests, int guestCount)
{
	_mutex.lock();

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

	_mutex.unlock();
}

void GuestList::getGuests(function<void(vector<Guest>& guests)> callback)
{
	if (callback)
	{
		_mutex.lock();
		callback(_guests);
		_mutex.unlock();
	}
}

void GuestList::clear()
{
	_mutex.lock();
	_guests.clear();
	_mutex.unlock();
}


const bool GuestList::find(const char* targetName, function<void(Guest* result)> callback)
{
	return find(string(targetName), callback);
}


const bool GuestList::find(string targetName, function<void(Guest* result)> callback)
{
	_mutex.lock();

	bool found = findIterator(targetName, [&](vector<Guest>::iterator gi) {
		if (callback)
		{
			callback(&*gi);
		}
	});

	_mutex.unlock();
	return found;
}



const bool GuestList::find(uint32_t userID, function<void(Guest* result)> callback)
{
	_mutex.lock();

	bool found = findIterator(userID, [&](vector<Guest>::iterator gi) {
		if (callback)
		{
			callback(&*gi);
		}
	});

	_mutex.unlock();
	return found;
}

const bool GuestList::findByIndex(uint32_t index, function<void(Guest* result)> callback)
{
	_mutex.lock();

	bool found = (index >= 0 && index < _guests.size());
	
	if (found && callback)
	{
		callback(&_guests[index]);
	}

	_mutex.unlock();
	return found;
}


// ========================================================
// PRIVATE
// ========================================================
const bool GuestList::findIterator(uint32_t userID, function<void(vector<Guest>::iterator)> callback)
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

const bool GuestList::findIterator(string targetName, function<void(vector<Guest>::iterator)> callback)
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