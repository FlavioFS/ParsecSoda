#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../ActionQueue.h"
#include <functional>
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

	void renderOneOnlineGuest(Guest& guest, size_t index);
	void renderOneBannedGuest(GuestData& guest, size_t index);
	void renderOneHistoryGuest(GuestData& guest, size_t index);

	void renderAnyGuestList(GuestList& guestList, function<void(Guest&, size_t index)> renderOneGuest);
	void renderAnyGuestDataList(GuestDataList& guestDataList, function<void(GuestData&, size_t index)> renderOneGuest);


	bool isGuestFilterMatch(string name, uint32_t userID, string filterTextStr);

	void sendHostMessage(const char* command, uint32_t userID);

	// Dependency injection
	Hosting& _hosting;
	BanList& _banList;
	GuestDataList& _historyList;
	
	// Attributes
	char _filterText[256] = "";
	string _logBuffer;

	/**
	* Why does this exist?
	* The calls to get guests from modules already lock their mutexes,
	* a second call to change these data structures would lock again
	* causing a deadlock crash. Therefore, the UI:
	* 1. Renders all components first while enqueues requests.
	* 2. Performs at once all actions requested by the user during this frame.
	 */
	ActionQueue _actionQueue;
};