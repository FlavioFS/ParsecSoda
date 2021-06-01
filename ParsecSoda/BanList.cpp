#include "BanList.h"

BanList::BanList()
{
}

BanList::BanList(std::vector<BannedUser> bannedUsers)
{
	_bannedUsers = bannedUsers;
}

void BanList::ban(BannedUser user)
{
	_bannedUsers.push_back(user);
}

const bool BanList::unban(BannedUser user)
{
	std::vector<BannedUser>::iterator it;
	for (it = _bannedUsers.begin(); it != _bannedUsers.end(); ++it)
	{
		if ((*it).userId == user.userId)
		{
			_bannedUsers.erase(it);
			return true;
		}
	}

	return false;
}

const bool BanList::isBanned(const uint32_t userId)
{
	std::vector<BannedUser>::iterator it;
	for (it = _bannedUsers.begin(); it != _bannedUsers.end(); ++it)
	{
		if ((*it).userId == userId)
		{
			return true;
		}
	}

	return false;
}
