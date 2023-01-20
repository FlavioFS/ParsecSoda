#include "BanList.h"
#include "Stringer.h"

BanList::BanList()
{
}

bool BanList::ban(GuestData user)
{
	bool added = GuestDataList::add(user);
	if (added)
	{
		MetadataCache::saveBannedUsers(m_guests);
	}
	return added;
}

const bool BanList::unban(const uint32_t userID, function<void(GuestData&)> callback)
{
	bool found = GuestDataList::pop(userID, callback);
	if (found)
	{
		MetadataCache::saveBannedUsers(m_guests);
	}
	return found;
}

const bool BanList::unban(string guestName, function<void(GuestData&)> callback)
{
	bool found = GuestDataList::pop(guestName, callback);
	if (found)
	{
		MetadataCache::saveBannedUsers(m_guests);
	}
	return found;
}

const bool BanList::isBanned(const uint32_t userID)
{
	return GuestDataList::find(userID);
}
