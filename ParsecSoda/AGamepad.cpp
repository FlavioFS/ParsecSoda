#include "AGamepad.h"

AGamepad::AGamepad()
	: parsec(nullptr)
{
	_client = nullptr;
	_isAlive = false;
	_index = GAMEPAD_INDEX_ERROR;
	_isConnected = false;
	clearOwner();
}

AGamepad::AGamepad(ParsecDSO* parsec, PVIGEM_CLIENT client)
	: parsec(parsec)
{
	_client = client;
	_isAlive = false;
	_index = GAMEPAD_INDEX_ERROR;
	_isConnected = false;
	clearOwner();
}

bool AGamepad::realloc()
{
	release();
	return alloc();
}

bool AGamepad::disconnect()
{
	if (!_isAlive || _client == nullptr)
	{
		_isConnected = false;
		return false;
	}

	if (!VIGEM_SUCCESS(vigem_target_remove(_client, _pad)))
	{
		return false;
	}

	_index = -1;
	_isConnected = false;
	clearOwner();
	return true;
}

void AGamepad::release()
{
	if (_isAlive)
	{
		disconnect();
		vigem_target_free(_pad);
		clearOwner();
		_isAlive = false;
		_isConnected = false;
	}
}

bool AGamepad::isAttached()
{
	if (_isAlive)
	{
		return vigem_target_is_attached(_pad);
	}

	return false;
}

void AGamepad::setIndex(ULONG index)
{
	_index = index;
}

ULONG AGamepad::getIndex() const
{
	return _index;
}

XINPUT_STATE AGamepad::getState()
{
	return _currentState;
}

AGamepad::Keyboard& AGamepad::getKeyboard()
{
	return _keyboard;
}

void AGamepad::setOwner(Guest& guest, uint32_t deviceID, bool isKeyboard)
{
	owner.guest.copy(guest);
	owner.deviceID = deviceID;
	owner.isKeyboard = isKeyboard;
}

void AGamepad::copyOwner(AGamepad* pad)
{
	if (pad != nullptr) owner.copy(pad->owner);
}

void AGamepad::clearOwner()
{
	owner = GuestDevice();
}

const bool AGamepad::isOwned()
{
	return owner.guest.isValid();
}

bool AGamepad::isConnected() const
{
	return _isConnected;
}

const bool AGamepad::isLocked() const
{
	return _isLocked;
}

void AGamepad::setLocked(const bool value)
{
	_isLocked = value;
}

void AGamepad::toggleLocked()
{
	setLocked(!_isLocked);
}
