#include "Dice.h"

bool Dice::roll(int chance)
{
	static std::default_random_engine _eng;
	static std::uniform_int_distribution<size_t> _dist = std::uniform_int_distribution<size_t>(0, 100);

	return _dist(_eng) <= chance;
}

size_t Dice::number(size_t min, size_t max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(min, max);

	return (size_t)distrib(gen);
}

size_t Dice::number(size_t max)
{
	return number(0, max);
}