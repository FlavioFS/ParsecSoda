#include "Clock.h"

std::time_t Clock::now()
{
    auto chronoNow = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(chronoNow);

    return timeNow;
}

std::string Clock::timestamp()
{
    std::time_t now = Clock::now();
    std::string result = std::ctime(&now);

    return result;
}