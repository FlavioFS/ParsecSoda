#pragma once

#include <iostream>

using namespace std;

/**
 *  A reduced data structure to hold basic guest data.
 */
class GuestData
{
	/*
	*  This class is expected to hold constant guest data only,
	*  for remembering accounts and populating persistent data lists.
	*  Runtime data such as metrics should not be added here.
	*/

public:
	/**
	 *  Empty constructor.
	 */
	GuestData()
	{
		name = "";
		userID = 0;
	}

	/**
	 * Full constructor.
	 * @param name Guest username.
	 * @param userID Unique user ID.
	 */
	GuestData(string name, uint32_t userID)
	{
		set(name, userID);
	}

	void set(string name, uint32_t userID)
	{
		this->name = name;
		this->userID = userID;
	}

	std::string name;
	uint32_t userID;
};

