#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "IconButton.h"
#include "TitleTooltipWidget.h"
#include "ConfirmPopupWidget.h"

class GuestListWidget
{
public:
	GuestListWidget(Hosting& hosting);
	bool render();

private:
	void renderOnlineGuests();
	void renderBannedGuests();
	void renderHistoryGuests();

	// Dependency injection
	Hosting& _hosting;
	BanList& _banList;
	
	// Attributes
	char _filterText[256] = "";
	string _logBuffer;
	//vector<Guest>& _guests;
	vector<GuestData>& _guestHistory;
};