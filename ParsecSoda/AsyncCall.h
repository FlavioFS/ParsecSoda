#pragma once
#include <functional>
#include <thread>

class AsyncCall
{
public:
	static void run(std::function<void(void)> procedure);
};

