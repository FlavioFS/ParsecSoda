#include "ActionQueue.h"

void ActionQueue::add(Action call)
{
	m_calls.push_back(call);
}

void ActionQueue::runAll()
{
	std::vector<Action>::iterator call0;
	int watchdog = m_calls.size() + 1;

	while (m_calls.size() > 0)
	{
		call0 = m_calls.begin();

		if (call0 != m_calls.end())
		{
			if (*call0)
			{
				(*call0)();
			}
			m_calls.erase(call0);
		}

		if (watchdog-- < 0) break;
	}
}