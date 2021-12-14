#pragma once

#include "AGamepad.h"

using namespace std;

class XBoxGamepad : public AGamepad
{
public:
	const AGamepad::Type type() const override { return AGamepad::Type::XBOX; }

	XBoxGamepad();
	XBoxGamepad(ParsecDSO* parsec, PVIGEM_CLIENT client);


	// State mesages
	virtual bool alloc();
	virtual bool connect();
	virtual void clearState();
	virtual void clearOwner();
	virtual bool refreshIndex();
	virtual void setState(XINPUT_STATE state);
	virtual void setStateSafe(XINPUT_STATE state);
};