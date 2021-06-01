#include "Dice.h"

void Dice::init()
{
	_dist = std::uniform_int_distribution<size_t>(0, 100);
}

bool Dice::roll(int chance)
{
	return _dist(_eng) <= chance;
}
