#pragma once

#include <vector>
#include <sstream>
#include <mutex>
#include <functional>
#include "parsec.h"
#include "Guest.h"
#include "Stringer.h"

using namespace std;

#define GUESTLIST_MAX_GUESTS 64

/**
* A concrete and in-depth guest list.
* Contains real-time ONLINE guests and must be fetched directly from Parsec SDK.
*/
class GuestList
{
	typedef vector<Guest>::iterator Iterator;
	typedef function<void(void)> Action;
	typedef function<void(Guest*)> ItemCallback;
	typedef function<void(vector<Guest>&)> ListCallback;
	typedef function<void(vector<Guest>::iterator)> IteratorCallback;

public:
	void setGuestsSafe(ParsecGuest* guests, int guestCount);
	void setGuestsUnsafe(ParsecGuest* guests, int guestCount);
	void getGuestsSafe(GuestList::ListCallback callback);
	void getGuestsUnsafe(GuestList::ListCallback callback);
	void clearSafe();
	void clearUnsafe();

	const bool findSafe(const char* targetName, GuestList::ItemCallback callback = nullptr);
	const bool findUnsafe(const char* targetName, GuestList::ItemCallback callback = nullptr);
	const bool findSafe(string targetName, GuestList::ItemCallback callback = nullptr);
	const bool findUnsafe(string targetName, GuestList::ItemCallback callback = nullptr);

	const bool findSafe(uint32_t userID, GuestList::ItemCallback callback = nullptr);
	const bool findUnsafe(uint32_t userID, GuestList::ItemCallback callback = nullptr);
	const bool findByIndexSafe(uint32_t index, GuestList::ItemCallback callback = nullptr);

	void getMutexLockContext(GuestList::Action callback);

private:
	const bool findIteratorUnsafe(uint32_t userID, GuestList::IteratorCallback callback);
	const bool findIteratorUnsafe(string targetName, GuestList::IteratorCallback callback);

	vector<Guest> _guests;

	mutex _mutex;
};

