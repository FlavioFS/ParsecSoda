#include "Thumbnail.h"

Thumbnail::Thumbnail()
	: gameId(""), name(""), saved(false)
{}

Thumbnail::Thumbnail(std::string gameId, std::string name, bool saved)
	: gameId(gameId.c_str()), name(name.c_str()), saved(saved)
{
}

Thumbnail::Thumbnail(char* gameId, char* name, bool saved)
	: gameId(gameId), name(name), saved(saved)
{}

bool Thumbnail::isValid()
{
	return (gameId.size() > GAMEID_MINIMUM_LENGTH && name.size() > 0);
}
