#pragma once

#include "Guest.h"

class GuestDevice
{
public:
	GuestDevice(Guest guest = Guest(), uint32_t deviceID = 0, bool isKeyboard = false);
	GuestDevice(const GuestDevice& device);
	
	void copy(const GuestDevice& device);

	Guest guest = Guest();
	uint32_t deviceID = 0;
	bool isKeyboard = false;
};

