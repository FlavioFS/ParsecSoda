#include "GuestDataList.h"

GuestDataList::GuestDataList()
{
}

GuestDataList::GuestDataList(const std::vector<GuestData> guests)
{
	init(guests);
}

void GuestDataList::init(const std::vector<GuestData> guests)
{
	m_guests = guests;
}

bool GuestDataList::add(GuestData guest)
{
	const std::lock_guard<std::mutex> lock(m_mutex);

	bool found = findIterator(guest.userID);

	if (!found) m_guests.push_back(guest);

	return !found;
}

bool GuestDataList::find(uint32_t userID, function<void(GuestData& guest)> callback)
{
	const std::lock_guard<std::mutex> lock(m_mutex);

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
	const std::lock_guard<std::mutex> lock(m_mutex);

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
	const std::lock_guard<std::mutex> lock(m_mutex);

	bool found = findIterator(userID, [&](vector<GuestData>::iterator gi) {
		if (callback != nullptr)
		{
			callback(*gi);
		}
		m_guests.erase(gi);
	});

	return found;
}

bool GuestDataList::pop(string guestName, function<void(GuestData& guest)> callback)
{
	const std::lock_guard<std::mutex> lock(m_mutex);

	bool found = findIterator(guestName, [&](vector<GuestData>::iterator gi) {
		if (callback != nullptr)
		{
			callback(*gi);
		}

		m_guests.erase(gi);
	});

	return found;
}

void GuestDataList::getGuests(function<void(vector<GuestData>&)> callback)
{
	const std::lock_guard<std::mutex> lock(m_mutex);

	if (callback)
	{
		callback(m_guests);
	}
}


// =============================================================
//
//  Private
//
// =============================================================
bool GuestDataList::findIterator(uint32_t userID, function<void(vector<GuestData>::iterator guest)> callback)
{
	vector<GuestData>::iterator it = m_guests.begin();

	for (; it != m_guests.end(); ++it)
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
	vector<GuestData>::iterator it = m_guests.begin();

	for (; it != m_guests.end(); ++it)
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
