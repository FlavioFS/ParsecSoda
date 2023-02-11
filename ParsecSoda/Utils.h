#pragma once

#include <string>
#include <algorithm>
#include "parsec.h"
#include "Stringer.h"
#include <Windows.h>

namespace Utils {
	void removeCharFromString(std::string *str, char c);
	bool findUser(uint32_t targetUserID, ParsecGuest* guests, int guestCount, ParsecGuest* targetUser);
	bool findUser(std::string targetUserName, ParsecGuest* guests, int guestCount, ParsecGuest* targetUser);
	void copyToClipboard(const string& text);
}

