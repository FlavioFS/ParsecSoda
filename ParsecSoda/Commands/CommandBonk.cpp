#include "CommandBonk.h"

Stopwatch CommandBonk::_stopwatch;

void CommandBonk::init()
{
	_stopwatch.setDurationSec(5);
	_stopwatch.start();
}