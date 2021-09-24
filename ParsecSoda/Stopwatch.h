#pragma once

#include <iostream>
#include <chrono>

class Stopwatch
{
public:
	Stopwatch(uint32_t duration = 1000);
	void start();
	void reset();
	void reset(uint32_t duration);
	void stop();
	bool isFinished();
	bool isRunning();
	void setDuration(uint32_t duration);
	uint32_t getDuration();
	uint32_t getRemainingTime();

private:
	void fetchTimer();

	uint32_t _remainingTime = 1000;
	uint32_t _duration = 1000;
	bool _isRunning = false;
	std::chrono::duration<double, std::milli> _elapsedDuration;
	std::chrono::system_clock::time_point _timeStart;
};

