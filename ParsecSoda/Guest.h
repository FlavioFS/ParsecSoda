#pragma once

#include <iostream>
#include "parsec.h"

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
	Guest(string name, uint32_t userID, uint32_t id, bool isHost = false);
	
	/**
	 * Compatibility constructor.
	 * @param guest ParsecGuest object.
	 */
	Guest(ParsecGuest guest, bool isHost = false);

	const bool isValid();

	string name;
	int64_t userID;
	int64_t id;
	bool isHost;
	int index;
};