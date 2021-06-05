#include "Bitwise.h"

void Bitwise::setValue(unsigned short* source, unsigned short bit, bool value)
{
	if (value)
	{
		*source |= bit;
	}
	else
	{
		*source &= ~bit;
	}
}