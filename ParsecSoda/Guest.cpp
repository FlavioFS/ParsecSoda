#include "Guest.h"

Guest::Guest()
	: name(""), userID((uint64_t)GUEST_ID_ERRORS::NONE), id((uint64_t)GUEST_ID_ERRORS::NONE)
{}


Guest::Guest(string name, uint32_t userID, uint32_t id)
	: name(name), userID(userID), id(id)
{}

Guest::Guest(ParsecGuest guest)
	: name(guest.name), userID(guest.userID), id(guest.id)
{}

const bool Guest::isValid()
{
	return userID != (uint64_t)GUEST_ID_ERRORS::NONE;
}