#include "Stopwatch.h"

Stopwatch::Stopwatch(uint32_t duration)
	: _duration(duration),
	_isRunning(false), _elapsedDuration(0), _remainingTime(1000),
	_timeStart(std::chrono::system_clock::now())
{
}

void Stopwatch::start()
{
	if (!_isRunning)
	{
		_isRunning = true;
		reset();
	}
}

void Stopwatch::reset()
{
	_timeStart = std::chrono::system_clock::now();
	_remainingTime = _duration;
}

void Stopwatch::reset(uint32_t duration)
{
	setDuration(duration);
	reset();
}

void Stopwatch::stop()
{
	_isRunning = false;
}

bool Stopwatch::isFinished()
{
	fetchTimer();
	return _elapsedDuration.count() >= _duration;
}

bool Stopwatch::isRunning()
{
    return _isRunning;
}

void Stopwatch::setDuration(uint32_t duration)
{
	_duration = duration;
}

uint32_t Stopwatch::getDuration()
{
	return _duration;
}

uint32_t Stopwatch::getRemainingTime()
{
	return isFinished() ? 0 : _duration - (uint32_t)_elapsedDuration.count();
}

void Stopwatch::fetchTimer()
{
	_elapsedDuration = std::chrono::system_clock::now() - _timeStart;
}
