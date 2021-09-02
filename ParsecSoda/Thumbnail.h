#pragma once

#include <string>

class Thumbnail
{
public:
	Thumbnail();
	Thumbnail(std::string gameId, std::string name, bool saved = false);
	Thumbnail(char* gameId, char* name, bool saved = false);
	bool isValid();


	Thumbnail& operator=(const Thumbnail& other) {
		Thumbnail thumb = Thumbnail(other.gameId, other.name, other.saved);
		this->gameId = thumb.gameId;
		this->name = thumb.name;
		this->saved = thumb.saved;
		return thumb;
	};

	std::string gameId;
	std::string name;
	bool saved;

private:
	const uint32_t GAMEID_MINIMUM_LENGTH = 25;
};

