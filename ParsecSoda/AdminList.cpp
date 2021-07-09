#include "AdminList.h"

AdminList::AdminList()
{
}

AdminList::AdminList(const vector<GuestData> admins)
	: GuestDataList(admins)
{
}

bool AdminList::isAdmin(uint32_t guestID)
{
	return find(guestID);
}
