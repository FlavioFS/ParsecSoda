#pragma once

#include <random>
#include <iostream>
#include <functional>

class Dice
{
public:
	void init();
	bool roll(int chance);

private:
	std::default_random_engine _eng;
	std::uniform_int_distribution<size_t> _dist;
};

