#include "VideoWidget.h"

VideoWidget::VideoWidget(Hosting& hosting)
    : _hosting(hosting), _dx11(_hosting.getDX11())
{
}

bool VideoWidget::render()
{
    static float indentSize = 0;
    static ImVec2 dummySize = ImVec2(0.0f, 20.0f);
    static ImVec2 cursor;

    AppStyle::pushTitle();

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 150), ImVec2(500, 400));
    ImGui::Begin("Video");

    ImGui::Dummy(dummySize);

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

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}