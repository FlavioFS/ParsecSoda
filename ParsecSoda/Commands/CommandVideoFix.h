#pragma once

#include "ACommand.h"
#include "../DX11.h"
#include <iostream>

class CommandVideoFix : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::VIDEOFIX; }

	CommandVideoFix(DX11 &dx11)
		: _dx11(dx11)
	{}

	bool run() override
	{
		_replyMessage = "[ChatBot]\tRefreshing Directx11...\0";
		_dx11.recover();
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!videofix" };
	}

protected:
	DX11& _dx11;
};