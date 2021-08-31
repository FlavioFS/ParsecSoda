#pragma once

#include <iostream>
#include <string>


#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR 3600

#define MINUTES(n) n * SECONDS_PER_MINUTE	// Minutes measured in seconds.
#define HOURS(n) n * SECONDS_PER_HOUR		// Hours measured in seconds.

class Timespan
{
public:
	static Timespan fromSeconds(uint32_t seconds);

	std::string toString();

	uint32_t hours = 0;
	uint8_t minutes = 0;
	uint8_t seconds = 0;
};