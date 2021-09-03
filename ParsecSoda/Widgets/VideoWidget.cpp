#include "VideoWidget.h"

VideoWidget::VideoWidget(Hosting& hosting)
    : _hosting(hosting), _dx11(_hosting.getDX11())
{
    _fps = (int) MetadataCache::preferences.fps;
    _bandwidth = (int)MetadataCache::preferences.bandwidth;
}

bool VideoWidget::render()
{
    static float indentSize = 0;
    static ImVec2 dummySize = ImVec2(0.0f, 15.0f);
    static ImVec2 cursor;

    AppStyle::pushTitle();

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 210), ImVec2(500, 600));
    ImGui::Begin("Video");


    AppStyle::pushLabel();

    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();

    // =============================================================
    //  Input devices
    // =============================================================
    static UINT currentScreen = 0;
    currentScreen = _dx11.getScreen();

    static vector<string> screens = _dx11.listScreens();
    static string name = (currentScreen < screens.size()) ? screens[currentScreen] : "";

    ImGui::SetNextItemWidth(size.x);

    AppStyle::pushLabel();
    ImGui::Text("Monitors");
    AppStyle::pop();
    AppFonts::pushInput();
    ImGui::SetNextItemWidth(size.x);
    if (ImGui::BeginCombo("##Monitor list", name.c_str()))
    {
        for (UINT i = 0; i < screens.size(); ++i)
        {
            bool isSelected = (currentScreen == i);
            if (ImGui::Selectable(screens[i].c_str(), isSelected))
            {
                currentScreen = i;
                name = screens[currentScreen];
                _dx11.setScreen(i);
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    AppFonts::pop();
    TitleTooltipWidget::render("Monitor List", "Choose the screen you want to display.\nAvoid changing this while stream is running.");
    
    ImGui::Dummy(dummySize);

    static int previousBandwidth;
    static int previousFps;
    previousBandwidth = _bandwidth;
    previousFps = _fps;

    cursor = ImGui::GetCursorPos();

    ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::Text("Bandwidth (Mbps)");
    AppStyle::pop();
    if (IntRangeWidget::render("Bandwidth (Mbps)", _bandwidth, 10, 200, 1))
    {
        TitleTooltipWidget::render("Encoder Bandwidth", "Limits the encoder bandwidth usage.\n(This value is split between guests.)");
    }
    ImGui::EndGroup();

    ImGui::SetCursorPos(ImVec2(cursor.x + size.x - 105, cursor.y));

    ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::Text("FPS");
    AppStyle::pop();
    if (IntRangeWidget::render("Encoder FPS", _fps, 10, 250, 0.5f))
    {
        TitleTooltipWidget::render("Encoder FPS", "Limits the amount of frames per second.");
    }
    ImGui::EndGroup();

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    static Debouncer debouncer(DEBOUNCER_TIME_MS, [&]() {
        _hosting.setHostVideoConfig(_fps, _bandwidth);
        _hosting.applyHostConfig();
        MetadataCache::savePreferences();
    });
    if (previousFps != _fps || previousBandwidth != _bandwidth)
    {
        debouncer.start();
    }

    return true;
}