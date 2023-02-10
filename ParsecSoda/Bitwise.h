#pragma once

namespace Bitwise
{
	void setValue(unsigned short& source, const unsigned short bitMask, const bool value);
	const bool getBit(const unsigned short& source, const unsigned short bitMask);
}
