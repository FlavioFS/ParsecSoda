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

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 280), ImVec2(500, 600));
    ImGui::Begin("Video");


    AppStyle::pushLabel();

    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();

    // =========================================================
    // Monitors
    // =========================================================
    static UINT currentScreen = 0;
    currentScreen = _dx11.getScreen();

    static vector<string> screens = _dx11.listScreens();
    static string screenName = (currentScreen < screens.size()) ? screens[currentScreen] : "";

    ImGui::SetNextItemWidth(size.x);

    AppStyle::pushLabel();
    ImGui::Text("Monitors");
    AppStyle::pop();
    AppFonts::pushInput();
    ImGui::SetNextItemWidth(size.x);
    if (ImGui::BeginCombo("##Monitor list", screenName.c_str()))
    {
        for (UINT i = 0; i < screens.size(); ++i)
        {
            bool isSelected = (currentScreen == i);
            if (ImGui::Selectable(screens[i].c_str(), isSelected))
            {
                currentScreen = i;
                screenName = screens[currentScreen];
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
    TitleTooltipWidget::render("Monitor List", "Choose the screen you want to display.\n\n* Avoid changing this while stream is already running.");
    
    ImGui::Dummy(dummySize);

    // =========================================================
    // Adapters
    // =========================================================
    static size_t currentGPU = 0;
    currentGPU = _dx11.getGPU();

    static vector<string> gpus = _dx11.listGPUs();
    static string gpuName = (currentGPU < gpus.size()) ? gpus[currentGPU] : "";

    ImGui::SetNextItemWidth(size.x);

    AppStyle::pushLabel();
    ImGui::Text("Adapters");
    AppStyle::pop();
    AppFonts::pushInput();
    ImGui::SetNextItemWidth(size.x);
    if (ImGui::BeginCombo("##Adapter list", gpuName.c_str()))
    {
        for (UINT i = 0; i < gpus.size(); ++i)
        {
            bool isSelected = (currentGPU == i);
            if (ImGui::Selectable(gpus[i].c_str(), isSelected))
            {
                currentGPU = i;
                gpuName = gpus[currentGPU];
                _dx11.setGPU(i);
                screens = _dx11.listScreens();
                _dx11.setScreen(currentScreen);
                currentScreen = _dx11.getScreen();
                screenName = currentScreen >= screens.size() ? "" : screens[currentScreen];
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    AppFonts::pop();
    TitleTooltipWidget::render(
        "Adapter List",
        (
            string() +
            "Choose the GPU that renders your desktop.\n" +
            "Picking the wrong GPU may result in black screen.\n\n" +
            "* Avoid changing this while stream is already running."
        ).c_str()
    );

    ImGui::Dummy(dummySize);

    // =========================================================
    // Bandwidth
    // =========================================================
    static uint32_t previousBandwidth;
    static uint32_t previousFps;
    previousBandwidth = _bandwidth;
    previousFps = _fps;

    cursor = ImGui::GetCursorPos();

    ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::Text("Bandwidth (Mbps)");
    AppStyle::pop();

    const static auto intRangeReleaseCallback = [&]() {
        _hosting.setHostVideoConfig(_fps, _bandwidth);
        _hosting.applyHostConfig();
        MetadataCache::savePreferences();
    };

    if (IntRangeWidget::render<uint32_t>("Bandwidth (Mbps)", _bandwidth, 1, 1000, 1, 75.0f, intRangeReleaseCallback))
    {
        TitleTooltipWidget::render("Encoder Bandwidth", "Limits the encoder bandwidth usage.\n(This value is split between guests.)");
    }
    ImGui::EndGroup();

    ImGui::SetCursorPos(ImVec2(cursor.x + size.x - 105, cursor.y));

    // =========================================================
    // FPS
    // =========================================================
    ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::Text("FPS");
    AppStyle::pop();
    if (IntRangeWidget::render<uint32_t>("Encoder FPS", _fps, 10, 250, 0.5f, 75.0f, intRangeReleaseCallback))
    {
        TitleTooltipWidget::render("Encoder FPS", "Limits the amount of frames per second.");
    }
    ImGui::EndGroup();

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    //static Debouncer debouncer(DEBOUNCER_TIME_MS, [&]() {
    //    _hosting.setHostVideoConfig(_fps, _bandwidth);
    //    _hosting.applyHostConfig();
    //    MetadataCache::savePreferences();
    //});
    //if (previousFps != _fps || previousBandwidth != _bandwidth)
    //{
    //    debouncer.start();
    //}

    return true;
}