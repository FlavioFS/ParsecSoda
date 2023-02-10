#include "Bitwise.h"

void Bitwise::setValue(unsigned short& source, const unsigned short bitMask, const bool value)
{
	if (value)
	{
		source |= bitMask;
	}
	else
	{
		source &= ~bitMask;
	}
}

const bool Bitwise::getBit(const unsigned short& source, const unsigned short bitMask)
{
	return (source & bitMask) != 0;
}
