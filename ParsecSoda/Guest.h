#pragma once

#include <iostream>

using namespace std;

enum class GUEST_ID_ERRORS
{
	NONE = -1,
	WEB = 0
};

class Guest
{
public:
	/**
	 *  Empty constructor.
	 */
	Guest();

	/**
	 * Full constructor.
	 * @param name Guest username.
	 * @param userId Unique user ID.
	 * @param id Internal id for specific methods.
	 */
	Guest(string name, uint32_t userId, uint32_t id);

	const string getName();
	const int64_t getUserID();
	const int64_t getID();

private:
	string name;
	int64_t userId;
	int64_t id;
};