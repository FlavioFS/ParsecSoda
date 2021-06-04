#pragma once

#include "ACommand.h"
#include "../DX11.h"
#include <iostream>

class CommandVideoFix : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::VIDEOFIX; }

	void run(DX11 *dx11)
	{
		_replyMessage = "[ChatBot] | Refreshing Directx11...\0";
		dx11->recover();
	}
};

