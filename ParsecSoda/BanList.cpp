#include "BanList.h"
#include "Stringer.h"

BanList::BanList()
{
}

BanList::BanList(std::vector<GuestData> bannedUsers)
	: GuestDataList(bannedUsers)
{
}

bool BanList::ban(GuestData user)
{
	bool added = GuestDataList::add(user);
	if (added)
	{
		MetadataCache::saveBannedUsers(_guests);
	}
	return added;
}

const bool BanList::unban(const uint32_t userID, function<void(GuestData&)> callback)
{
	bool found = GuestDataList::pop(userID, callback);
	if (found)
	{
		MetadataCache::saveBannedUsers(_guests);
	}
	return found;
}

const bool BanList::unban(string guestName, function<void(GuestData&)> callback)
{
	bool found = GuestDataList::pop(guestName, callback);
	if (found)
	{
		MetadataCache::saveBannedUsers(_guests);
	}
	return found;
}

const bool BanList::isBanned(const uint32_t userID)
{
	return find(userID);
}

vector<GuestData>& BanList::getGuests()
{
	return GuestDataList::getGuests();
}
