#pragma once

#include <vector>
#include <sstream>
#include <mutex>
#include "parsec.h"
#include "Guest.h"
#include "Stringer.h"
#include <functional>

using namespace std;

#define GUESTLIST_MAX_GUESTS 64

/**
* A concrete and in-depth guest list.
* Contains ONLINE guests and must be fetched directly from Parsec SDK.
*/
class GuestList
{
public:
	void setGuests(ParsecGuest* guests, int guestCount);
	void getGuests(function<void(vector<Guest>& guests)> callback);
	void clear();

	const bool find(const char* targetName, function<void(Guest* result)> callback = nullptr);
	const bool find(string targetName, function<void(Guest* result)> callback = nullptr);

	const bool find(uint32_t userID, function<void(Guest* result)> callback = nullptr);
	const bool findByIndex(uint32_t index, function<void(Guest* result)> callback = nullptr);

private:
	const bool findIterator(uint32_t userID, function<void(vector<Guest>::iterator)> callback);
	const bool findIterator(string targetName, function<void(vector<Guest>::iterator)> callback);

	vector<Guest> _guests;

	mutex _mutex;
};

