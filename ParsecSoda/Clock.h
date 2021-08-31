#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <chrono>
#include <ctime>
#include "Timespan.h"

class Clock
{
public:
	static std::time_t now();
	static std::string timestamp();
};

