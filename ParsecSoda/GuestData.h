#pragma once

#include <iostream>

using namespace std;

/**
 *  A reduced data structure to hold basic guest data.
 */
class GuestData
{
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

