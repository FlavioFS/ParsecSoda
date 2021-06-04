#include "BanList.h"
#include "Stringer.h"

BanList::BanList()
{
}

BanList::BanList(std::vector<GuestData> bannedUsers)
{
	_bannedUsers = bannedUsers;
}

void BanList::ban(GuestData user)
{
	_bannedUsers.push_back(user);
}

const bool BanList::unban(const char * guestName, GuestData* unbannedGuest)
{
	std::vector<GuestData>::iterator it;
	for (it = _bannedUsers.begin(); it != _bannedUsers.end(); ++it)
	{
		if ( Stringer::isCloseEnough((*it).name.c_str(), guestName) )
		{
			if (unbannedGuest != nullptr)
			{
				*unbannedGuest = *it;
			}
			_bannedUsers.erase(it);
			return true;
		}
	}

	return false;
}

const bool BanList::unban(std::string guestName, GuestData* unbannedGuest)
{
	return unban(guestName.c_str(), unbannedGuest);
}

const bool BanList::unban(uint32_t guestId, GuestData* unbannedGuest)
{
	std::vector<GuestData>::iterator it;
	for (it = _bannedUsers.begin(); it != _bannedUsers.end(); ++it)
	{
		if ((*it).userId == guestId)
		{
			*unbannedGuest = *it;
			_bannedUsers.erase(it);
			return true;
		}
	}

	return false;
}

const bool BanList::isBanned(const uint32_t userId)
{
	std::vector<GuestData>::iterator it;
	for (it = _bannedUsers.begin(); it != _bannedUsers.end(); ++it)
	{
		if ((*it).userId == userId)
		{
			return true;
		}
	}

	return false;
}
