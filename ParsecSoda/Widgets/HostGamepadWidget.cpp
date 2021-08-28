#include "HostGamepadWidget.h"

vector<Input::Gamepad> HostGamepadWidget::_systemGamepads;

bool HostGamepadWidget::render()
{
    static ImVec2 size;
    size = ImGui::GetWindowSize();

    //static int hostPadID = 0;

    //static XINPUT_STATE hostPad;
    //XInputGetState((DWORD)hostPadID, &hostPad);

    AppStyle::pushTitle();

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 120), ImVec2(400, 400));
    ImGui::Begin("Master of Puppets##Master of Puppets");
    AppStyle::pushInput();

    //IntRangeWidget::render("Host Pad ID", hostPadID, 0, 8);

    AppStyle::pushInput();
    ImGui::Text("[!] Warning");
    AppStyle::pop();
    AppStyle::pushLabel();
    ImGui::TextWrapped("This window is under construction.");
    AppStyle::pop();

    ImGui::Dummy(ImVec2(0, 10));

    //static XINPUT_STATE hostPads[8];

    ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::TextWrapped("Host Index");
    AppStyle::pop();
    static int hostIndex = 0, previousHostIndex = 0;
    IntRangeWidget::render("Host Gamepad Index", hostIndex, 0, _systemGamepads.size());

    ImGui::Dummy(ImVec2(0, 5));

    AppStyle::pushLabel();
    ImGui::TextWrapped("Puppet Index");
    AppStyle::pop();
    static int puppetIndex = 0, previousPuppetIndex = 0;
    IntRangeWidget::render("Puppet Gamepad Index", puppetIndex, 0, _systemGamepads.size());
    ImGui::EndGroup();
    
    ImGui::SameLine();

    ImGui::Dummy(ImVec2(20, 0));

    ImGui::SameLine();

    ImGui::BeginGroup();
    for (int i = 0; i < _systemGamepads.size(); ++i)
    {
    //    XInputGetState(i, &hostPads[i]);

        ImGui::Text("[%d]: ", i);
        ImGui::SameLine();

        static Input::GamepadReading reading;
        reading = _systemGamepads[i].GetCurrentReading();

        ImGui::Text(((uint32_t)reading.Buttons & (uint32_t)Input::GamepadButtons::A) != 0 ? "A" : "?");
        ImGui::SameLine();

        ImGui::Text(((uint32_t)reading.Buttons & (uint32_t)Input::GamepadButtons::B) != 0 ? "B" : "?");
        ImGui::SameLine();

        ImGui::Text(((uint32_t)reading.Buttons & (uint32_t)Input::GamepadButtons::X) != 0 ? "X" : "?");
        ImGui::SameLine();

        ImGui::Text(((uint32_t)reading.Buttons & (uint32_t)Input::GamepadButtons::Y) != 0 ? "Y" : "?");
        ImGui::SameLine();

        ImGui::Text("%d", (uint32_t)reading.Buttons);
    }
    ImGui::EndGroup();

    if (IconButton::render(AppIcons::refresh, AppColors::primary))
    {
        fetchSystemGamepads();
    }

    if (
        hostIndex > 0 && puppetIndex > 0 &&
        (hostIndex != previousHostIndex || puppetIndex != previousPuppetIndex)
        )
    {
        ImGui::SameLine();
        if (IconButton::render(AppIcons::submit, AppColors::primary))
        {

        }
    }

    AppStyle::pop();
    AppStyle::pop();
    ImGui::End();

    return true;
}

bool HostGamepadWidget::fetchSystemGamepads()
{
    static concurrency::critical_section _padLock{};

    _systemGamepads.clear();

    for (auto const& gamepad : Input::Gamepad::Gamepads())
    {
        concurrency::critical_section::scoped_lock lock{ _padLock };
        auto it{ std::find(begin(_systemGamepads), end(_systemGamepads), gamepad) };

        if (it == end(_systemGamepads))
        {
            _systemGamepads.push_back(gamepad);
        }
    }

    return false;
}