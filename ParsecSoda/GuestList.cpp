#include "GuestList.h"

void GuestList::setGuests(ParsecGuest* guests, int guestCount)
{
	_guests.clear();
	for (size_t i = 0; i < guestCount; i++)
	{
		_guests.push_back
		(
			Guest
			(
				guests[i].name,
				guests[i].userID,
				guests[i].id
			)
		);
	}
}

const vector<Guest> GuestList::getGuests() const
{
    return _guests;
}