#pragma once

#include <vector>
#include <sstream>
#include "parsec.h"
#include "Guest.h"
#include "Stringer.h"

using namespace std;

#define GUESTLIST_MAX_GUESTS 64

class GuestList
{
public:
	void setGuests(ParsecGuest* guests, int guestCount);
	vector<Guest> &getGuests();
	void clear();
	const bool find(uint32_t targetGuestID, Guest *result);
	const bool find(const char* targetName, Guest* result);
	const bool find(string targetName, Guest* result);

	const char* guestNames[GUESTLIST_MAX_GUESTS];
private:
	vector<Guest> _guests;
};

