#include "Guest.h"

Guest::Guest()
	: name(""), userID(0), id(0), status(Status::INVALID)
{}

Guest::Guest(string name, uint32_t userID, uint32_t id, Status status)
	: name(name), userID(userID), id(id), status(status)
{}

Guest::Guest(ParsecGuest guest)
	: name(guest.name), userID(guest.userID), id(guest.id), status(Status::OK)
{}

const bool Guest::isValid()
{
	return status == Status::OK;
}

Guest Guest::copy(const Guest& guest)
{
	name = guest.name;
	userID = guest.userID;
	id = guest.id;
	status = guest.status;
	return *this;
}