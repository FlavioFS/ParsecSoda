#include "AdminList.h"

AdminList::AdminList()
{
}

AdminList::AdminList(const std::vector<int> admins)
{
	_admins = admins;
}

bool AdminList::isAdmin(int guestID)
{
	for (size_t i = 0; i < _admins.size(); i++)
	{
		if (_admins[i] == guestID) { return true; }
	}
	return false;
}
