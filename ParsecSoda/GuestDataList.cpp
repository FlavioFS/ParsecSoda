#include "GuestDataList.h"

GuestDataList::GuestDataList()
{
}

GuestDataList::GuestDataList(const std::vector<GuestData> guests)
{
	_guests = guests;
}

bool GuestDataList::add(GuestData guest)
{
	bool found = find(guest.userID);
	if (!found)
	{
		_guests.push_back(guest);
		return true;
	}

	return false;
}

bool GuestDataList::find(uint32_t userID, function<void(GuestData& guest)> callback)
{
	bool found = findIterator(userID, [&](vector<GuestData>::iterator gi) {
		if (callback != nullptr)
		{
			callback(*gi);
		}
	});

	return found;
}

bool GuestDataList::find(string guestName, function<void(GuestData& guest)> callback)
{
	bool found = findIterator(guestName, [&](vector<GuestData>::iterator gi) {
		if (callback != nullptr)
		{
			callback(*gi);
		}
	});

	return found;
}

bool GuestDataList::pop(uint32_t userID, function<void(GuestData& guest)> callback)
{
	bool found = findIterator(userID, [&](vector<GuestData>::iterator gi) {
		if (callback != nullptr)
		{
			callback(*gi);
		}
		_guests.erase(gi);
	});

	return found;
}

bool GuestDataList::pop(string guestName, function<void(GuestData& guest)> callback)
{
	bool found = findIterator(guestName, [&](vector<GuestData>::iterator gi) {
		if (callback != nullptr)
		{
			callback(*gi);
		}
		_guests.erase(gi);
	});

	return found;
}

vector<GuestData>& GuestDataList::getGuests()
{
	return _guests;
}


// =============================================================
//
//  Private
//
// =============================================================
bool GuestDataList::findIterator(uint32_t userID, function<void(vector<GuestData>::iterator guest)> callback)
{
	vector<GuestData>::iterator it = _guests.begin();

	for (; it != _guests.end(); ++it)
	{
		if (userID == (*it).userID)
		{
			if (callback != nullptr)
			{
				callback(it);
			}
			return true;
		}
	}

	return false;
}

bool GuestDataList::findIterator(string guestName, function<void(vector<GuestData>::iterator guest)> callback)
{
	vector<GuestData>::iterator it = _guests.begin();

	for (; it != _guests.end(); ++it)
	{
		if (Stringer::isCloseEnough((*it).name.c_str(), guestName))
		{
			if (callback != nullptr)
			{
				callback(it);
			}
			return true;
		}
	}

	return false;
}
