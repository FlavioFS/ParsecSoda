#pragma once

#include <random>
#include <iostream>
#include <functional>

class Dice
{
public:
	static bool roll(int chance);
	static size_t number(size_t max);
	static size_t number(size_t min, size_t max);
};

