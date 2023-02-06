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
		hotseatManager.runThreadSafe([&]() {
			renderHotseats(hotseatManager);
			renderSeparator();	
			renderWaitingGuests(hotseatManager);
		});
	}


	AppStyle::pop();
	ImGui::End();
	AppStyle::pop();
	ImGui::PopStyleVar();


	//static Stopwatch timer(300 * Stopwatch::MS_PER_SEC);
	//static Stopwatch cooldown(5 * Stopwatch::MS_PER_SEC);
	//timer.start();

	//static float t = 0.5f;
	//static string overlay;

	//AppStyle::pushTitle();
	//ImGui::SetNextWindowSizeConstraints(ImVec2(400, 280), ImVec2(800, 1100));
	//ImGui::Begin("Hotseats");
	//AppStyle::pushInput();

	////ImGui::DragFloat("GradientProgressBar test", &t, 0.005f, 0, 1);
	//
	//t = (float)timer.getRemainingTime() / (float)timer.getDuration();
	//overlay = to_string(timer.getRemainingTime() / Stopwatch::MS_PER_SEC) + " s";
	//GradientProgressBar::render(t, ImVec2(300, 20), overlay.c_str());

	//if (timer.isRunComplete() && !cooldown.isRunning())
	//{
	//	cooldown.start();
	//}

	//if (cooldown.isRunComplete())
	//{
	//	timer.reset();
	//	cooldown.stop();
	//	cooldown.reset();
	//}

	//AppStyle::pop();
	//ImGui::End();
	//AppStyle::pop();
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

	width = ImGui::GetWindowWidth();

	AppFonts::pushTitle();
	AppColors::pushLabel();
	ImGui::Text("Seats");
	ImGui::SameLine();
	if (IconButton::render(AppIcons::skip, AppColors::primary, ImVec2(30, 30)))
	{
		hotseatManager.next();
	}
	AppColors::pop();
	AppFonts::pop();

	index = 0;
	for (; iSeat != seats.end(); ++iSeat)
	{
		remainingTimeFraction = (float) iSeat->timer.getRemainingTime() / (float) iSeat->timer.getDuration();
		colorFinal = GradientProgressBar::lerpColors3(remainingTimeFraction);

		ImGui::Image(AppIcons::seat, ICONBUTTON_SIZE, ImVec2(0, 0), ImVec2(1, 1), colorFinal);

		if (!iSeat->isEmpty)
		{
			ImGui::SameLine();

			//ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 100);
			ImGui::BeginGroup();
			AppStyle::pushLabel();
			ImGui::TextWrapped((string("#") + to_string(iSeat->guest.guest.userID)).c_str());
			AppStyle::pop();

			AppStyle::pushInput();
			ImGui::TextWrapped(iSeat->guest.guest.name.c_str());
			AppStyle::pop();
			ImGui::EndGroup();
		}

		if (IconButton::render(AppIcons::timerRefresh, AppColors::primary, ImVec2(30, 30)))
		{
			hotseatManager.refresh(index);
		}
		ImGui::SameLine();
		if (IconButton::render(AppIcons::skip, AppColors::primary, ImVec2(30, 30)))
		{
			hotseatManager.next(*iSeat);
		}

		remainingTimeText = to_string(iSeat->timer.getRemainingTime() / Stopwatch::MS_PER_SEC) + " s";
		GradientProgressBar::renderSingle(remainingTimeFraction, ImVec2(width - 20,20), remainingTimeText.c_str(), colorFinal);

		++index;
	}
}

void HotseatWidget::renderWaitingGuests(HotseatManager& hotseatManager)
{
	const vector<HotseatGuest> guests = hotseatManager.getWaitingGuests();
	vector<HotseatGuest>::const_iterator iGuest = guests.begin();
	static size_t index;

	AppFonts::pushTitle();
	AppColors::pushLabel();
	ImGui::Text("Waiting Line");
	AppColors::pop();
	AppFonts::pop();

	index = 0;
	for (; iGuest != guests.end(); ++iGuest)
	{
		ImGui::SameLine();
		AppStyle::pushLabel();
		ImGui::TextWrapped(to_string(iGuest->guest.userID).c_str());
		AppStyle::pop();

		AppStyle::pushInput();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 100);
		ImGui::Text(iGuest->guest.name.c_str());
		AppStyle::pop();

		if (IconButton::render(AppIcons::sendTop))
		{
			hotseatManager.cutLine(index);
		}
		TitleTooltipWidget::render(
			"Cut Line",
			(string("Instantly send ") + iGuest->guest.name + "to a seat?").c_str()
		);

		if (!iGuest->isOnline)
		{
			ImGui::Image(AppIcons::wifiOff, ImVec2(40, 40), ImVec2(0, 0), ImVec2(1, 1), AppColors::negative);
		}

		++index;
	}
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

