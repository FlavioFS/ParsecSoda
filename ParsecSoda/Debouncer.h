#pragma once

#include <Windows.h>
#include <thread>
#include <functional>
#include "Stopwatch.h"

class Debouncer
{
public:
	/* Debounces inputs by delaying the result until a specified idle time duration elapses. */
	Debouncer(uint32_t delay, std::function<void()> callback);
	
	/* Resets time counter if debouncing has already started. */
	void start();
	
	void reset(uint32_t delay);
	void setDelay(uint32_t delay);
	void setCallback(std::function<void()> callback);

private:
	void reset();

	Stopwatch _stopwatch;
	std::function<void()> _callback;
	std::thread _thread;
};