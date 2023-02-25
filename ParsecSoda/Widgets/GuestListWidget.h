#pragma once

#include "../imgui/imgui.h"
#include "../imgui/implot.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../ActionQueue.h"
#include "../CircularMetrics.h"
#include <functional>
#include "IconButton.h"
#include "BadgeButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "AbstractTooltipWidget.h"
#include "ConfirmPopupWidget.h"
#include "GuestMetricsHistoryGraphWidget.h"
#include "ToggleIconButtonWidget.h"

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

	bool isGuestFilterMatch(const string name, const uint32_t userID, const string filterTextStr);
	void sendHostMessage(const string command, const uint32_t userID);
	ImVec4 getLatencyColor(float latency);
	void renderPopupButton(const string title, const string command, const GuestData& guest, const size_t& index, bool& showPopup, string& popupTitle, Icon btnIcon, Action action);

	void renderGuestMetricsTooltip(GuestData guest);

	// Dependency injection
	Hosting& _hosting;
	BanList& _banList;
	GuestDataList& _historyList;
	GuestDataList& _mutedGuests;
	GuestMetricsHistory& _metricsHistory;
	
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