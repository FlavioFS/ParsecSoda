#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"

class GuestListWidget
{
public:
	GuestListWidget(Hosting& hosting);
	bool render();

private:
	// Dependency injection
	Hosting& _hosting;
	
	// Attributes
	string _logBuffer;
	vector<Guest>& _guests;
};

