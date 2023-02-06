#pragma once

#include <iostream>
#include <chrono>

class Stopwatch
{
public:
	static const uint32_t MS_PER_SEC = 1000;

	Stopwatch(uint32_t duration = 1000);
	void start();
	void reset();
	void reset(uint32_t duration);
	void stop();
	
	/**
	 * Check if time is over.
	 * @return True if Stopwatch is running AND time elapsed exceeded duration. False otherwise.
	 */
	bool isRunComplete();

	/**
	 * Check if stopwatch is active.
	 * @return True if Stopwatch is running. False otherwise.
	 */
	bool isRunning();

	/**
	 * Set duration (in milliseconds).
	 * @param durationMs Time span in milliseconds.
	 */
	void setDuration(uint32_t durationMs);

	/**
	 * Set duration (in seconds).
	 * @param durationSec Time span in seconds.
	 */
	void setDurationSec(uint32_t durationSec);
	
	/**
	 * Get duration of a run cycle (in milliseconds).
	 * @return Stopwatch duration (in milliseconds).
	 */
	uint32_t getDuration();

	/**
	 * Get time left until the run is complete (in milliseconds).
	 * @return Remaining time (in milliseconds).
	 */
	uint32_t getRemainingTime();

private:
	void fetchTimer();

	/** 
	 * Check if time is over.
	 * @return True if time elapsed exceeded duration (even when Stopwatch is not running). False otherwise.
	 */
	bool isTimeout();

	uint32_t _remainingTime = 1000;
	uint32_t _duration = 1000;
	bool _isRunning = false;
	std::chrono::duration<double, std::milli> _elapsedDuration;
	std::chrono::system_clock::time_point _timeStart;
};

