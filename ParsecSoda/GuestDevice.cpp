#include "GuestDevice.h"

GuestDevice::GuestDevice(Guest guest, uint32_t deviceID, bool isKeyboard)
	: guest(guest), deviceID(0), isKeyboard(false)
{
}

GuestDevice::GuestDevice(const GuestDevice& device)
{
	copy(device);
}

void GuestDevice::copy(const GuestDevice& device)
{
	guest = device.guest;
	deviceID = device.deviceID;
	isKeyboard = device.isKeyboard;
}
