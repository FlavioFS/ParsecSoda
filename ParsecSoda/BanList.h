#pragma once

#include <iostream>
#include <vector>

typedef struct BannedUser {
	uint32_t userId;
	std::string name;
} BannedUser;

class BanList
{
public:
	BanList();
	BanList(const std::vector<BannedUser> bannedUsers);
	void ban (const BannedUser user);
	const bool unban(const BannedUser user);
	const bool isBanned(const uint32_t userId);

private:
	std::vector<BannedUser> _bannedUsers;
};

