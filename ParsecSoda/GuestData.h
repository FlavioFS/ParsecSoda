#pragma once

#include <iostream>

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
		userId = 0;
	}

	/**
	 * Full constructor.
	 * @param name Guest username.
	 * @param userId Unique user ID.
	 */
	GuestData(std::string name, uint32_t userId)
	{
		this->name = name;
		this->userId = userId;
	}
	std::string name;
	uint32_t userId;
};

