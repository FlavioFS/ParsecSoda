#include "CommandBonk.h"

Stopwatch CommandBonk::_stopwatch;

void CommandBonk::init()
{
	_stopwatch.setDuration(5000);
	_stopwatch.start();
}