#pragma once

#include <functional>
#define STRESS_ITERATION_COUNT 100000

class StressTest
{
public:
	void test(std::function<void()>&& callback, size_t count = STRESS_ITERATION_COUNT) {
		if (callback != nullptr)
		{
			for (size_t i = 0; i < count; i++)
			{
				callback();
			}
		}
	}
};