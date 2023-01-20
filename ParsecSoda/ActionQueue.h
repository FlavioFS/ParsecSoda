#pragma once

#include <vector>
#include <functional>

/**
* A procedure, a void function. E.g.: a lambda function.
*/
typedef std::function<void(void)> Action;

class ActionQueue
{
public:
	/**
	* Pushes an Action to the end of queue line.
	*/
	void add(Action call);

	/** 
	* Runs all Actions and remove them from the queue,
	* one by one in chronological order.
	*/
	void runAll();

private:
	std::vector<Action> m_calls;
};