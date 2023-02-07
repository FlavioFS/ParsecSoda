#include "HotseatWidget.h"

void HotseatWidget::render(Hosting& hosting)
{
	static bool isWindowLocked = false;
	static HotseatManager& hotseatManager = hosting.getHotseatManager();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 5));

	AppStyle::pushTitle();
	ImGui::SetNextWindowSizeConstraints(ImVec2(400, 280), ImVec2(800, 1100));
	ImGui::Begin("Hotseats", (bool*)0, isWindowLocked ? (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize) : 0);
	AppStyle::pushInput();

	renderTopBar(hotseatManager, isWindowLocked);
	
	ImGui::Separator();
	renderDummy();

	renderTimePickers(hotseatManager);
	
	if (hotseatManager.isEnabled())
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 3));
		hotseatManager.runThreadSafe([&]() {
			renderHotseats(hotseatManager);
			renderSeparator();	
			renderWaitingGuests(hotseatManager);
		});
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}


	AppStyle::pop();
	ImGui::End();
	AppStyle::pop();
	ImGui::PopStyleVar();
}

void HotseatWidget::renderTopBar(HotseatManager& hotseatManager, bool& isWindowLocked)
{
	renderDummy();
	ImGui::SameLine();

	// ======================
	// Buttons
	// ======================
	static bool isActive = hotseatManager.isEnabled();
	if (ToggleIconButtonWidget::render(AppIcons::switchOn, AppIcons::switchOff, isActive, AppColors::positive, AppColors::negative, ImVec2(30, 30)))
	{
		isActive = !isActive;
		hotseatManager.setEnabled(isActive);
	}
	if (isActive)	TitleTooltipWidget::render("Hotseats Enabled", "Module is active.\nGamepads automatically assigned to guests.");
	else			TitleTooltipWidget::render("Hotseats Disabled", "Module is inactive.\nYou must assign gamepads manually.");


	// ======================
	// Cursor
	// ======================
	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	ImGui::SetCursorPosY(31);
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 45);
	if (IconButton::render(
		AppIcons::move,
		isWindowLocked ? AppColors::negative : AppColors::positive,
		ImVec2(30, 30)
	))
	{
		isWindowLocked = !isWindowLocked;
	}
	if (isWindowLocked) TitleTooltipWidget::render("Window Locked", "This window cannot move or resize.");
	else TitleTooltipWidget::render("Window Unlocked", "This window can move and resize.");

	ImGui::SetCursorPos(cursor);
}

void HotseatWidget::renderTimePickers(HotseatManager& hotseatManager)
{
	static uint32_t timerMinutes = MetadataCache::preferences.hotseatDurationSeconds / 60;
	static uint32_t timerSeconds = MetadataCache::preferences.hotseatDurationSeconds % 60;

	static const function<void(void)> onTimerChanged = [&]() {
		if (timerMinutes <= 0 && timerSeconds <= 0)
		{
			timerSeconds = 1;
		}

		MetadataCache::preferences.hotseatDurationSeconds = 60 * timerMinutes + timerSeconds;
		MetadataCache::savePreferences();
		hotseatManager.runThreadSafe([&]() {
			hotseatManager.setTimer(MetadataCache::preferences.hotseatDurationSeconds);
		});
	};

	renderDummy();
	ImGui::SameLine();

	ImGui::BeginChild("##Timer Minutes", ImVec2(120.0f, 75.0f));
	AppStyle::pushLabel();
	ImGui::Text("Minutes");
	AppStyle::pop();
	if (IntRangeWidget::render<uint32_t>("hotseat timer minutes", timerMinutes, 0, 300, 0.1f, 75.0f, onTimerChanged))
	{
		TitleTooltipWidget::render("Hotseat Timer (minutes)", "Seats will auto-rotate on cooldown timeout.");
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("##Timer Seconds", ImVec2(120.0f, 75.0f));
	AppStyle::pushLabel();
	ImGui::Text("Seconds");
	AppStyle::pop();
	if (IntRangeWidget::render<uint32_t>("hotseat timer seconds", timerSeconds, 0, 59, 0.1f, 75.0f, onTimerChanged))
	{
		TitleTooltipWidget::render("Hotseat Timer (seconds)", "Seats will auto-rotate on cooldown timeout.");
	}
	ImGui::EndChild();
}

void HotseatWidget::renderHotseats(HotseatManager& hotseatManager)
{
	vector<Hotseat> seats = hotseatManager.getSeats();
	vector<Hotseat>::iterator iSeat = seats.begin();
	static ImVec4 colorFinal;
	static size_t index;
	static float remainingTimeFraction;
	static string remainingTimeText;
	static float width;
	static const ImVec2 badgeSize(25, 25);
	static ImVec2 cursor;
	static string id;

	width = ImGui::GetWindowWidth();

	cursor = ImGui::GetCursorPos();
	AppFonts::pushTitle();
	AppColors::pushLabel();
	ImGui::SetCursorPosY(cursor.y + 1);
	ImGui::Text("Seats");
	ImGui::SameLine();
	ImGui::SetCursorPosY(cursor.y);
	ImGui::PushID("###Hotseats skip shortest cooldown");
	if (IconButton::render(AppIcons::skip, AppColors::primary, badgeSize))
	{
		hotseatManager.next();
	}
	ImGui::PopID();
	TitleTooltipWidget::render("Skip", "Press to skip the seat with least time available.\nSeating guest will be sent back to the end of waiting line.\nNext guest will take over the seat.");
	AppColors::pop();
	AppFonts::pop();

	ImGui::Indent();
	index = 0;
	for (; iSeat != seats.end(); ++iSeat)
	{
		remainingTimeFraction = (float) iSeat->timer.getRemainingTime() / (float) iSeat->timer.getDuration();
		remainingTimeText = to_string(iSeat->timer.getRemainingTime() / Stopwatch::MS_PER_SEC) + " s";
		colorFinal = GradientProgressBar::lerpColors3(remainingTimeFraction);

		ImGui::Image(AppIcons::seat, ICONBUTTON_SIZE, ImVec2(0, 0), ImVec2(1, 1), colorFinal);

		if (!iSeat->isEmpty)
		{
			ImGui::SameLine();

			ImGui::BeginGroup();
			AppStyle::pushLabel();
			ImGui::Text((string("#") + to_string(iSeat->guest.guest.userID)).c_str());
			AppStyle::pop();

			AppStyle::pushInput();
			ImGui::Text(iSeat->guest.guest.name.c_str());
			AppStyle::pop();
			ImGui::EndGroup();
		}

		id = "###Hotseats cooldown refresh " + to_string(index);
		ImGui::PushID(id.c_str());
		if (IconButton::render(AppIcons::timerRefresh, AppColors::primary, badgeSize))
		{
			hotseatManager.refresh(index);
		}
		TitleTooltipWidget::render("Restart cooldown", "Press to refresh cooldown timer for this seat.");
		ImGui::PopID();
		
		ImGui::SameLine();
		
		id = "###Hotseats skip seat " + to_string(index);
		ImGui::PushID(id.c_str());
		if (IconButton::render(AppIcons::skip, AppColors::primary, badgeSize))
		{
			hotseatManager.next(index);
		}
		TitleTooltipWidget::render("Skip", "Press to skip timer.\nSeating guest will be sent back to the end of waiting line.\nNext guest will take over the seat.");
		ImGui::PopID();

		ImGui::SameLine();

		id = "###Hotseats spectate seat " + to_string(index);
		ImGui::PushID(id.c_str());
		if (IconButton::render(AppIcons::remove, AppColors::primary, badgeSize))
		{
			hotseatManager.spectateSeat(index);
		}
		ImGui::PopID();
		TitleTooltipWidget::render("Force spectate", "Press to remove this guest from seat AND waiting line.");

		GradientProgressBar::renderSingle(remainingTimeFraction, ImVec2(width - 60,20), remainingTimeText.c_str(), colorFinal);

		renderDummy(2, 4);

		++index;
	}
	ImGui::Unindent();
}

void HotseatWidget::renderWaitingGuests(HotseatManager& hotseatManager)
{
	const vector<HotseatGuest> guests = hotseatManager.getWaitingGuests();
	vector<HotseatGuest>::const_iterator iGuest = guests.begin();

	const vector<Hotseat> seats = hotseatManager.getSeats();
	vector<Hotseat>::const_iterator iSeat = seats.begin();

	static size_t index;
	static ImVec2 badgeSize(25, 25);
	static ImVec2 windowSize;
	windowSize = ImGui::GetWindowSize();
	static ImVec2 cursor, masterButtonPosition;
	static bool containsMaster;
	containsMaster = false;
	static string id;

	AppFonts::pushTitle();
	AppColors::pushLabel();
	ImGui::Text("Waiting Line");
	ImGui::SameLine();
	masterButtonPosition = ImGui::GetCursorPos();
	renderDummy(0, 0);
	AppColors::pop();
	AppFonts::pop();

	renderDummy();

	ImGui::Indent();
	index = 0;
	for (; iGuest != guests.end(); ++iGuest)
	{
		AppStyle::pushLabel();
		ImGui::Text((string("#") + to_string(iGuest->guest.userID)).c_str());
		AppStyle::pop();

		AppStyle::pushInput();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 100);
		ImGui::Text(iGuest->guest.name.c_str());
		AppStyle::pop();


		id = "###Hotseats cut line " + to_string(index);
		ImGui::PushID(id.c_str());
		if (IconButton::render(AppIcons::sendTop, AppColors::primary, badgeSize))
		{
			hotseatManager.cutLine(index);
		}
		TitleTooltipWidget::render(
			"Cut Line",
			(string("Instantly send ") + iGuest->guest.name + " to a seat?").c_str()
		);
		ImGui::PopID();

		ImGui::SameLine();

		id = "###Hotseats spectate line " + to_string(index);
		ImGui::PushID(id.c_str());
		if (IconButton::render(AppIcons::remove, AppColors::primary, badgeSize))
		{
			hotseatManager.spectateGuest(index);
		}
		TitleTooltipWidget::render("Force spectate", "Press to remove this guest from waiting line.");
		ImGui::PopID();

		if (!iGuest->isOnline)
		{
			ImGui::SameLine();
			ImGui::Image(AppIcons::wifiOff, badgeSize, ImVec2(0, 0), ImVec2(1, 1), AppColors::negative);
			TitleTooltipWidget::render(
				"Offline",
				"Guest has left the room while in line.\nOffline guests remain in line until their turn (in case they return).\nIf still offline when receiving the seat, they are removed from waiting line."
			);
		}

		if (iGuest->isMultitap)
		{
			ImGui::SameLine();
			ImGui::Image(AppIcons::multitap, badgeSize, ImVec2(0, 0), ImVec2(1, 1), AppColors::primary);
			TitleTooltipWidget::render(
				"Multitap",
				"Multiple gamepad support is enabled for this guest (deviceID is active).\nThey may plug multiple inputs in their machine and pick more than one puppet."
			);
		}

		if (iGuest->isMaster)
		{
			containsMaster = true;
			ImGui::SameLine();
			ImGui::Image(AppIcons::puppet, badgeSize, ImVec2(0, 0), ImVec2(1, 1), AppColors::primary);
			TitleTooltipWidget::render(
				"Master of Puppets",
				"Host playing through master of puppets."
			);
		}

		++index;
	}

	for (; iSeat != seats.end(); ++iSeat)
	{
		if (iSeat->guest.isMaster)
		{
			containsMaster = true;
			break;
		}
	}

	// Master
	cursor = ImGui::GetCursorPos();
	ImGui::SetCursorPos(masterButtonPosition);
	if (!containsMaster)
	{
		if (IconButton::render(AppIcons::puppetAdd, AppColors::primary, badgeSize))
		{
			containsMaster = true;
			hotseatManager.enqueue(HotseatGuest(MetadataCache::host, 0, false, true, true));
		}
		TitleTooltipWidget::render("Join as Master", "Enter waiting queue through Master of Puppets.");
	}
	ImGui::SetCursorPos(cursor);

	ImGui::Unindent();
}

void HotseatWidget::renderDummy(float x, float y)
{
	ImGui::Dummy(ImVec2(x, y));
}

void HotseatWidget::renderSeparator()
{
	renderDummy();
	ImGui::Separator();
	renderDummy();
}

