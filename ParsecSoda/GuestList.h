#pragma once

#include <vector>
#include "parsec.h"
#include "Guest.h"
#include "Stringer.h"

using namespace std;

class GuestList
{
public:
	void setGuests(ParsecGuest* guests, int guestCount);
	const vector<Guest> &getGuests() const;
	const bool find(uint32_t targetGuestID, Guest *result);
	const bool find(const char* targetName, Guest* result);
	const bool find(string targetName, Guest* result);

private:
	vector<Guest> _guests;
};

