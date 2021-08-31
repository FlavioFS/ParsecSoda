#include "Timespan.h"

Timespan Timespan::fromSeconds(uint32_t seconds)
{
	Timespan result;

	uint32_t remainder = seconds;

	result.hours = remainder / SECONDS_PER_HOUR;		// Integer division, remainder is truncated
	remainder -= HOURS(result.hours);

	result.minutes = remainder / SECONDS_PER_MINUTE;	// Integer division, remainder is truncated
	remainder -= MINUTES(result.minutes);

	result.seconds = remainder;

	return result;
}

std::string Timespan::toString()
{
	char buffer [50];

	sprintf_s(buffer, sizeof(buffer), "%dh %02dm %02ds", hours, minutes, seconds);

	return std::string(buffer);
}
