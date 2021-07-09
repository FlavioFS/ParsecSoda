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
	bool found = GuestDataList::add(user);
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

const bool BanList::isBanned(const uint32_t userId)
{
	return find(userId);
}

vector<GuestData>& BanList::getGuests()
{
	return GuestDataList::getGuests();
}
