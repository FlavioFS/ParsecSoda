#include "Guest.h"

Guest::Guest()
{
	name = "";
	userId = (uint64_t) GUEST_ID_ERRORS::NONE;
	id = (uint64_t) GUEST_ID_ERRORS::NONE;
}


Guest::Guest(string name, uint32_t userId, uint32_t id)
{
	this->name = name;
	this->userId = userId;
	this->id = id;
}


const string Guest::getName()
{
	return name;
}

const int64_t Guest::getUserID()
{
	return userId;
}

const int64_t Guest::getID()
{
	return id;
}
