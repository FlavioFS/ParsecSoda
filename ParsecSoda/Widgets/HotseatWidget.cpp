#include "HotseatWidget.h"

void HotseatWidget::render()
{
	static Stopwatch timer(300 * 1000);
	static Stopwatch cooldown(5 * 1000);
	timer.start();

	static float t = 0.5f;
	static string overlay;

	AppStyle::pushTitle();
	ImGui::SetNextWindowSizeConstraints(ImVec2(400, 280), ImVec2(800, 1100));
	ImGui::Begin("Hotseats");
	AppStyle::pushInput();

	//ImGui::DragFloat("GradientProgressBar test", &t, 0.005f, 0, 1);
	
	t = (float)timer.getRemainingTime() / (float)timer.getDuration();
	overlay = to_string(timer.getRemainingTime() / 1000) + " s";
	GradientProgressBar::render(t, ImVec2(300, 20), overlay.c_str());

	if (timer.isRunComplete() && !cooldown.isRunning())
	{
		cooldown.start();
	}

	if (cooldown.isRunComplete())
	{
		timer.reset();
		cooldown.stop();
		cooldown.reset();
	}

	AppStyle::pop();
	ImGui::End();
	AppStyle::pop();
}

