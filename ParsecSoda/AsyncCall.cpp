#include "AsyncCall.h"

void AsyncCall::run(std::function<void(void)> procedure)
{
	if (procedure)
	{
		std::thread l_thread = std::thread([&]() {
			procedure();
			l_thread.detach();
		});
	}
}
