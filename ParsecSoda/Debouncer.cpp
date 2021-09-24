#include "Debouncer.h"

Debouncer::Debouncer(uint32_t delay, std::function<void()> callback)
{
	setDelay(delay);
	setCallback(callback);
}

void Debouncer::start()
{
	if (!_stopwatch.isRunning())
	{
		_stopwatch.start();
		_thread = std::thread([&] {

			uint32_t sleepTime = _stopwatch.getRemainingTime();

			while (!_stopwatch.isFinished() && sleepTime > 0)
			{
				Sleep(sleepTime);
				sleepTime = _stopwatch.getRemainingTime();
			}

			_stopwatch.stop();
			if (_callback != nullptr)
			{
				_callback();
			}
			_thread.detach();
		});
	}
	else
	{
		_stopwatch.reset();
	}
}

void Debouncer::reset(uint32_t delay)
{
	_stopwatch.reset(delay);
}

void Debouncer::setDelay(uint32_t delay)
{
	_stopwatch.setDuration(delay);
}

void Debouncer::setCallback(std::function<void()> callback)
{
	_callback = callback;
}

void Debouncer::reset()
{
	_stopwatch.reset();
}