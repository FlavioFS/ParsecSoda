#include "HotseatWidget.h"

void HotseatWidget::render(Hosting& hosting)
{
	static bool isWindowLocked = false;
	static HotseatManager& hotseatManager = hosting.getHotseatManager();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 5));

	AppStyle::pushTitle();
	ImGui::SetNextWindowSizeConstraints(ImVec2(280, 280), ImVec2(800, 1100));
	ImGui::Begin("Hotseats", (bool*)0, isWindowLocked ? (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize) : 0);
	AppStyle::pushInput();

	renderTopBar(hotseatManager, isWindowLocked);
	
	ImGui::Separator();
	renderDummy();

	renderTimePickers(hotseatManager);
	
	if (hotseatManager.isEnabled())
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 0));
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
	static string id, badgeTooltip;

	width = ImGui::GetWindowWidth();

	cursor = ImGui::GetCursorPos();
	AppFonts::pushTitle();
	AppColors::pushLabel();
	ImGui::SetCursorPosY(cursor.y + 1);
	ImGui::Text("Seats");
	ImGui::SameLine();
	ImGui::SetCursorPosY(cursor.y);
	
	ImGui::BeginGroup();	
	badgeTooltip = "Press to refresh cooldown timer for all seats.";
	id = "###Hotseats refresh all cooldowns";
	if (renderBadgeButton(AppIcons::timerRefresh, "Restart all cooldowns", badgeTooltip.c_str(), id.c_str()))
	{
		hotseatManager.refreshAll();
	}

	ImGui::SameLine();

	badgeTooltip = string("")
		+ "Press to skip the seat with least time available.\n"
		+ "Seating guest will be sent back to the end of waiting line.\n"
		+ "Next guest will take over the seat.";
	id = "###Hotseats skip shortest cooldown";
	if (renderBadgeButton(AppIcons::skip, "Skip", badgeTooltip.c_str(), id.c_str()))
	{
		hotseatManager.next();
	}
	ImGui::EndGroup();
	
	AppColors::pop();
	AppFonts::pop();

	renderDummy(1, 6);

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

		badgeTooltip = "Press to refresh cooldown timer for this seat.";
		id = "###Hotseats cooldown refresh " + to_string(index);
		if (renderBadgeButton(AppIcons::timerRefresh, "Restart cooldown", badgeTooltip.c_str(), id.c_str()))
		{
			hotseatManager.refresh(index);
		}
		
		ImGui::SameLine();
		
		badgeTooltip = string("")
			+ "Press to skip timer.\n"
			+ "Seating guest will be sent back to the end of waiting line.\n"
			+ "Next guest will take over the seat.";
		id = "###Hotseats skip seat " + to_string(index);
		if (renderBadgeButton(AppIcons::skip, "Skip", badgeTooltip.c_str(), id.c_str()))
		{
			hotseatManager.next(index);
		}

		ImGui::SameLine();

		badgeTooltip = "Press to remove this guest from seat AND waiting line.";
		id = "###Hotseats spectate seat " + to_string(index);
		if (renderBadgeButton(AppIcons::remove, "Force spectate", badgeTooltip.c_str(), id.c_str()))
		{
			hotseatManager.spectateSeat(index);
		}

		if (iSeat->guest.isMultitap)
		{
			ImGui::SameLine();
			badgeTooltip = string("")
				+ "Multiple gamepad support is enabled for this guest (deviceID is active).\n"
				+ "They may plug multiple inputs in their machine and pick more than one puppet.";
			renderBadgeIcon(AppIcons::multitap, "Multitap", badgeTooltip.c_str());
		}

		if (iSeat->guest.isMaster)
		{
			ImGui::SameLine();
			badgeTooltip = "Host playing through master of puppets.";
			renderBadgeIcon(AppIcons::puppet, "Master of Puppets", badgeTooltip.c_str());
		}

		renderDummy(1, 4);
		AppFonts::pushCaption();
		GradientProgressBar::renderSingle(remainingTimeFraction, ImVec2(width - 60, 18), remainingTimeText.c_str(), colorFinal);
		AppFonts::pop();

		renderDummy(1, 5);

		++index;
	}
	ImGui::Unindent();

	renderDummy(2, 10);
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
	static string id, badgeTooltip;

	renderDummy(2, 10);

	AppFonts::pushTitle();
	AppColors::pushLabel();
	ImGui::Text("Waiting Line");
	ImGui::SameLine();
	masterButtonPosition = ImGui::GetCursorPos();
	renderDummy(0, 0);
	AppColors::pop();
	AppFonts::pop();

	renderDummy(2, 8);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 1));

	ImGui::Indent();
	index = 0;
	for (; iGuest != guests.end(); ++iGuest)
	{
		AppFonts::pushSugoiDekai();
		AppColors::pushLabel();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(to_string(index+1).c_str());
		AppColors::pop();
		AppFonts::pop();

		ImGui::SameLine();

		renderDummy(5, 1);

		ImGui::SameLine();

		ImGui::BeginGroup();
		renderDummy(1, 2);
		AppStyle::pushLabel();
		ImGui::Text((string("#") + to_string(iGuest->guest.userID)).c_str());
		AppStyle::pop();

		AppStyle::pushInput();
		ImGui::Text(iGuest->guest.name.c_str());
		AppStyle::pop();

		badgeTooltip = "Instantly send " + iGuest->guest.name + " to a seat?";
		id = "###Hotseats cut line " + to_string(index);
		if (renderBadgeButton(AppIcons::sendTop, "Cut Line", badgeTooltip.c_str(), id.c_str()))
		{
			hotseatManager.cutLine(index);
		}

		ImGui::SameLine();

		badgeTooltip = "Press to remove this guest from waiting line.";
		id = "###Hotseats spectate queue " + to_string(index);
		if (renderBadgeButton(AppIcons::remove, "Force spectate", badgeTooltip.c_str(), id.c_str()))
		{
			hotseatManager.spectateGuest(index);
		}

		if (!iGuest->isOnline)
		{
			ImGui::SameLine();
			badgeTooltip = string("")
				+ "Guest has left the room while in line.\n"
				+ "Offline guests remain in line until their turn (in case they return).\n" 
				+ "If still offline when receiving the seat, they are removed from waiting line.";
			renderBadgeIcon(AppIcons::wifiOff, "Offline", badgeTooltip.c_str(), AppColors::negative);
		}

		if (iGuest->isMultitap)
		{
			ImGui::SameLine();
			badgeTooltip = string("")
				+ "Multiple gamepad support is enabled for this guest (deviceID is active).\n" 
				+ "They may plug multiple inputs in their machine and pick more than one puppet.";
			renderBadgeIcon(AppIcons::multitap, "Multitap", badgeTooltip.c_str());
		}

		if (iGuest->isMaster)
		{
			containsMaster = true;
			ImGui::SameLine();
			badgeTooltip = "Host playing through master of puppets.";
			renderBadgeIcon(AppIcons::puppet, "Master of Puppets", badgeTooltip.c_str());
		}
		ImGui::EndGroup();

		renderDummy(2, 5);

		++index;
	}
	ImGui::PopStyleVar();

	for (; iSeat != seats.end(); ++iSeat)
	{
		if (iSeat->guest.isMaster)
		{
			containsMaster = true;
			break;
		}
	}

	renderDummy(2, 5);

	ImGui::BeginGroup();
	AppColors::pushColor(AppColors::gray40);
	AppFonts::pushBigHeader();
	TextCenteredWidget::render("Press [A, B, X, Y]");
	AppFonts::pop();
	AppFonts::pushTitle();
	TextCenteredWidget::render("to join the waiting list.");
	AppFonts::pop();
	AppColors::pop();
	ImGui::EndGroup();
	TitleTooltipWidget::render("Message for guests only", "If you are the Host, use the puppet button.");

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

void HotseatWidget::renderBadgeIcon(
	Icon icon, const char* tooltipTitle, const char* tooltipDescription,
	ImVec4 tint, ImVec2 size
)
{
	ImGui::Image(icon, size, ImVec2(0, 0), ImVec2(1, 1), tint);
	TitleTooltipWidget::render(tooltipTitle, tooltipDescription);
}

bool HotseatWidget::renderBadgeButton(
	Icon icon,
	const char* tooltipTitle, const char* tooltipDescription, const char* id,
	ImVec2 size, ImVec4 tint
)
{
	static bool result;
	result = false;

	function<void(void)> renderButtonAndTooltip = [&]() {
		if (IconButton::render(icon, tint, size))
		{
			result = true;
		}
		TitleTooltipWidget::render(tooltipTitle, tooltipDescription);
	};

	if (id)
	{
		ImGui::PushID(id);
		renderButtonAndTooltip();
		ImGui::PopID();
	}
	else
	{
		renderButtonAndTooltip();
	}

	return result;
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

