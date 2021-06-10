#pragma once

#include <vector>
#include "parsec.h"
#include "Guest.h"

using namespace std;

class GuestList
{
public:
	void setGuests(ParsecGuest* guests, int guestCount);
	const vector<Guest> getGuests() const;

private:
	vector<Guest> _guests;
};

