#include "Thumbnail.h"

Thumbnail::Thumbnail()
	: gameId(""), name(""), saved(false), edit(false)
{}

Thumbnail::Thumbnail(std::string gameId, std::string name, bool saved, bool edit)
	: gameId(gameId.c_str()), name(name.c_str()), saved(saved), edit(edit)
{
}

Thumbnail::Thumbnail(char* gameId, char* name, bool saved, bool edit)
	: gameId(gameId), name(name), saved(saved), edit(edit)
{}

bool Thumbnail::isValid()
{
	return (gameId.size() > GAMEID_MINIMUM_LENGTH && name.size() > 0);
}
