#pragma once

#include <iostream>
#include <vector>
#include "GuestData.h"

class BanList
{
public:
	BanList();
	BanList(const std::vector<GuestData> bannedUsers);
	void ban (const GuestData user);
	const bool unban(const char* guestName, GuestData * unbannedGuest = nullptr);
	const bool unban(std::string guestName, GuestData* unbannedGuest = nullptr);
	const bool unban(const uint32_t userId, GuestData * unbannedGuest = nullptr);
	const bool isBanned(const uint32_t userId);

private:
	std::vector<GuestData> _bannedUsers;
};

