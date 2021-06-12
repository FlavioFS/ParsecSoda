#include "Guest.h"

Guest::Guest()
	: name(""), userID((uint64_t)GUEST_ID_ERRORS::NONE), id((uint64_t)GUEST_ID_ERRORS::NONE), isHost(false)
{}


Guest::Guest(string name, uint32_t userID, uint32_t id, bool isHost)
	: name(name), userID(userID), id(id), isHost(false)
{}

Guest::Guest(ParsecGuest guest, bool isHost)
	: name(guest.name), userID(guest.userID), id(guest.id), isHost(false)
{}

const bool Guest::isValid()
{
	return userID != (uint64_t)GUEST_ID_ERRORS::NONE;
}