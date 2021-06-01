#include "Utils.h"

using namespace ParsecSoda;

void Utils::removeCharFromString(std::string *str, char c)
{
	str->erase(std::remove(str->begin(), str->end(), c), str->end());
}
