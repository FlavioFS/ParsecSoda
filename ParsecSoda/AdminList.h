#pragma once

#include "GuestDataList.h"

class AdminList : GuestDataList
{
public:
	AdminList();
	AdminList(const vector<GuestData> admins);
	bool isAdmin(uint32_t guestID);
};

