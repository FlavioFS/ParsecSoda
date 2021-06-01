#pragma once

#include <vector>

class AdminList
{
public:
	AdminList(const std::vector<int> admins);
	bool isAdmin(int guestID);

private:
	std::vector<int> _admins;
};

