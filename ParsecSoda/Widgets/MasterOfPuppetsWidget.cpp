#include "MasterOfPuppetsWidget.h"

vector<MasterOfPuppetsWidget::Master> MasterOfPuppetsWidget::_masters;

bool MasterOfPuppetsWidget::render(GamepadClient& gamepadClient)
{
    static int masterIndex = -1;
    static ImVec2 cursor;
    static ImVec2 size;
    size = ImGui::GetWindowSize();
    static Stopwatch stopwatch = Stopwatch(5000);
    stopwatch.start();
    AppStyle::pushTitle();

    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 320), ImVec2(900, 740));
    ImGui::Begin("Master of Puppets##Master of Puppets");
    AppStyle::pushInput();

    cursor = ImGui::GetCursorPos();
    static float fill;
    fill = 1.0f - (float)stopwatch.getRemainingTime() / stopwatch.getDuration();
    ProgressCircularWidget::render(20, 10, fill);
    ImGui::SetCursorPos(cursor);

    if (stopwatch.isFinished())
    {
        eraseDetachedJoysticks();
        fetchSystemGamepads();
        stopwatch.reset();
    }
    if (ImGui::Button("### SDL Joysticks Refresh", ImVec2(40, 40)))
    {
        static Debouncer debouncer = Debouncer(500, [&]() {
            eraseDetachedJoysticks();
            fetchSystemGamepads();
            stopwatch.reset();
        });
        debouncer.start();
    }
    TitleTooltipWidget::render("Gamepad refresh", "Click to manually refresh the gamepad detection list.");
    
    if (masterIndex >= _masters.size()) {
        masterIndex = -1;

    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10, 0));
    ImGui::SameLine();

    ImGui::BeginGroup();
    AppStyle::pushInput();
    ImGui::Text("[!] Warning");
    AppStyle::pop();
    AppStyle::pushLabel();
    ImGui::TextWrapped("This window is under construction.");
    AppStyle::pop();
    ImGui::EndGroup();

    SDL_JoystickUpdate();

    ImGui::Dummy(ImVec2(0, 10));

    ImGui::BeginGroup();
    renderMasterJoysticks(masterIndex, gamepadClient);
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(50, 0));
    ImGui::SameLine();
    renderPuppets(gamepadClient.gamepads);
    ImGui::EndGroup();

    AppStyle::pop();
    AppStyle::pop();
    ImGui::End();

    mapInputs(masterIndex, gamepadClient.gamepads);

    return true;
}

void MasterOfPuppetsWidget::fetchSystemGamepads()
{
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        SDL_Joystick* joy = SDL_JoystickOpen(i);
        if (SDL_JoystickGetAttached(joy))
        {
            if (contains(joy))
            {
                continue;
            }

            _masters.push_back(Master(joy));
        }
    }
}

void MasterOfPuppetsWidget::renderMasterJoysticks(int& masterIndex, GamepadClient& gamepadClient)
{
    static ImVec2 cursor;

    ImGui::BeginGroup();

    AppFonts::pushSugoiDekai();
    ImGui::Text("Master");
    AppFonts::pop();

    ImGui::Dummy(ImVec2(380, 0));

    for (int i = 0; i < _masters.size(); ++i)
    {
        static float shift1 = 55.0f, shift2 = 55.0f, shift3 = 5.0f;
        cursor = ImGui::GetCursorPos();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
        AppFonts::pushSugoiDekai();
        ImGui::Text("%d ", i + 1);
        AppFonts::pop();

        ImGui::SetCursorPos(cursor);
        ImGui::SetCursorPosX(shift1);

        ImGui::PushID(i);
        if (IconButton::render(
            _masters[i].isXInput ? AppIcons::xinput : AppIcons::dinput,
            AppColors::primary)
            )
        {
            _masters[i].isXInput = !_masters[i].isXInput;
        }
        TitleTooltipWidget::render("Button Mapping", "Click to switch between default XInput vs. Dualshock mappings.");

        ImGui::SetCursorPos(cursor);
        ImGui::SetCursorPosX(shift1 + shift2);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

        if (ImGui::Selectable(
            (
                string() + "### Master Gamepad " + to_string(i)
            ).c_str(),
            masterIndex == i, 0, ImVec2(275, 40))
        )
        {
            setMaster(
                (masterIndex == i) ? -1 : i,
                masterIndex,
                gamepadClient
            );
        }
        TitleTooltipWidget::render(
            "Master Gamepad",
            (masterIndex == i) ?
                "Master of Puppets is pulling the strings." :
                "Click to set this gamepad as Master.\n\n* Sometimes SDL returns weird results.\n   In that case, just try reconnecting the gamepads."
        );

        ImGui::SetCursorPos(cursor);
        ImGui::SetCursorPosX(shift1 + shift2 + shift3);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

        AnimatedGamepadWidget::render(sdlJoystickToGamepad(_masters[i].joystick, _masters[i].isXInput));
        ImGui::PopStyleVar();

        ImGui::PopID();
    }
    ImGui::EndGroup();
}

void MasterOfPuppetsWidget::renderPuppets(vector<Gamepad>& gamepads)
{
    static ImVec2 cursor;

    ImGui::BeginGroup();

    AppFonts::pushSugoiDekai();
    ImGui::Text("Puppets");
    AppFonts::pop();

    ImGui::Dummy(ImVec2(0, 0));

    for (int i = 0; i < gamepads.size(); ++i)
    {
        static float shift1 = 30.0f, shift2 = 20.0f, shift3 = 5.0f;
        cursor = ImGui::GetCursorPos();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
        AppFonts::pushSugoiDekai();
        ImGui::Text("%d ", i + 1);
        AppFonts::pop();

        ImGui::SetCursorPos(cursor);
        ImGui::SetCursorPosX(cursor.x + shift1 + shift2);

        ImGui::PushID(i);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

        if (ImGui::Selectable(
            (
                string() + "### Puppet Gamepad " + to_string(i)
                ).c_str(),
            gamepads[i].isPuppet, 0, ImVec2(275, 40))
            )
        {
            gamepads[i].isPuppet = !gamepads[i].isPuppet;
            if (!gamepads[i].isPuppet)
            {
                gamepads[i].clearState();
            }
        }
        TitleTooltipWidget::render("Puppets (Virtual Gamepads)", "Click to mark and control this Puppet.");

        ImGui::SetCursorPos(cursor);
        ImGui::SetCursorPosX(cursor.x + shift1 + shift2 + shift3);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

        AnimatedGamepadWidget::render(gamepads[i].getState().Gamepad);
        ImGui::PopStyleVar();

        ImGui::PopID();
    }
    ImGui::EndGroup();
}

XINPUT_GAMEPAD MasterOfPuppetsWidget::sdlJoystickToGamepad(SDL_Joystick* joy, bool isXInput)
{
    XINPUT_GAMEPAD pad;
    
    pad.wButtons = 0;
    pad.bLeftTrigger = 0;
    pad.bRightTrigger = 0;
    pad.sThumbLX = 0;
    pad.sThumbLY = 0;
    pad.sThumbRX = 0;
    pad.sThumbRY = 0;


    // =====================================================
    // Shared
    // =====================================================
    pad.sThumbLX = SDL_JoystickGetAxis(joy, 0);
    pad.sThumbLY = SDL_JoystickGetAxis(joy, 1);
    pad.sThumbRX = SDL_JoystickGetAxis(joy, 2);
    pad.sThumbRY = SDL_JoystickGetAxis(joy, 3);

    if (pad.sThumbLY == SDL_JOYSTICK_AXIS_MIN) pad.sThumbLY = SDL_JOYSTICK_AXIS_MAX;
    else if (pad.sThumbLY == SDL_JOYSTICK_AXIS_MAX) pad.sThumbLY = SDL_JOYSTICK_AXIS_MIN;
    else     pad.sThumbLY = -pad.sThumbLY;

    if (pad.sThumbRY == SDL_JOYSTICK_AXIS_MIN) pad.sThumbRY = SDL_JOYSTICK_AXIS_MAX;
    else if (pad.sThumbRY == SDL_JOYSTICK_AXIS_MAX) pad.sThumbRY = SDL_JOYSTICK_AXIS_MIN;
    else     pad.sThumbRY = -pad.sThumbRY;

    Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_UP, (SDL_JoystickGetHat(joy, 0) & SDL_HAT_UP) != 0);
    Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, (SDL_JoystickGetHat(joy, 0) & SDL_HAT_DOWN) != 0);
    Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, (SDL_JoystickGetHat(joy, 0) & SDL_HAT_LEFT) != 0);
    Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, (SDL_JoystickGetHat(joy, 0) & SDL_HAT_RIGHT) != 0);


    // =====================================================
    // XInput
    // =====================================================
    if (isXInput)
    {
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_A, SDL_JoystickGetButton(joy, (Uint8)XInput::A) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_B, SDL_JoystickGetButton(joy, (Uint8)XInput::B) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_X, SDL_JoystickGetButton(joy, (Uint8)XInput::X) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_Y, SDL_JoystickGetButton(joy, (Uint8)XInput::Y) != 0);

        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_LEFT_SHOULDER,  SDL_JoystickGetButton(joy, (Uint8)XInput::LB) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_RIGHT_SHOULDER, SDL_JoystickGetButton(joy, (Uint8)XInput::RB) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_BACK,           SDL_JoystickGetButton(joy, (Uint8)XInput::BACK) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_START,          SDL_JoystickGetButton(joy, (Uint8)XInput::START) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_LEFT_THUMB,     SDL_JoystickGetButton(joy, (Uint8)XInput::L3) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_RIGHT_THUMB,    SDL_JoystickGetButton(joy, (Uint8)XInput::R3) != 0);
        
        pad.bLeftTrigger = SDL_JoystickGetAxis(joy, 4);
        pad.bRightTrigger = SDL_JoystickGetAxis(joy, 5);
    }

    // =====================================================
    // DInput
    // =====================================================
    else
    {
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_A, SDL_JoystickGetButton(joy, (Uint8)DInput::X) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_B, SDL_JoystickGetButton(joy, (Uint8)DInput::Circle) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_X, SDL_JoystickGetButton(joy, (Uint8)DInput::Square) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_Y, SDL_JoystickGetButton(joy, (Uint8)DInput::Triangle) != 0);

        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_LEFT_SHOULDER, SDL_JoystickGetButton(joy, (Uint8)DInput::L1) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_RIGHT_SHOULDER, SDL_JoystickGetButton(joy, (Uint8)DInput::R1) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_BACK, SDL_JoystickGetButton(joy, (Uint8)DInput::SELECT) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_START, SDL_JoystickGetButton(joy, (Uint8)DInput::START) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_LEFT_THUMB, SDL_JoystickGetButton(joy, (Uint8)DInput::L3) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_RIGHT_THUMB, SDL_JoystickGetButton(joy, (Uint8)DInput::R3) != 0);

        pad.bLeftTrigger  = (SDL_JoystickGetButton(joy, (Uint8)DInput::L2) != 0) ? 255 : 0;
        pad.bRightTrigger = (SDL_JoystickGetButton(joy, (Uint8)DInput::R2) != 0) ? 255 : 0;
    }

    return pad;
}

void MasterOfPuppetsWidget::eraseDetachedJoysticks()
{
    vector<Master>::iterator it;
    for (it = _masters.begin(); it != _masters.end(); ++it)
    {
        if (_masters.empty())
        {
            break;
        }

        if (!SDL_JoystickGetAttached((*it).joystick))
        {
            SDL_JoystickClose((*it).joystick);
            _masters.erase(it);

            if (_masters.empty())
            {
                break;
            }
            else
            {
                it = _masters.begin();
            }
        }
    }
}

bool MasterOfPuppetsWidget::contains(SDL_Joystick* joystick)
{
    vector<Master>::iterator it;
    for (it = _masters.begin(); it != _masters.end(); ++it)
    {
        if ((*it).joystick == joystick)
        {
            return true;
        }
    }

    return false;
}

void MasterOfPuppetsWidget::mapInputs(int& masterIndex, vector<Gamepad>& gamepads)
{
    if (masterIndex >= 0 && masterIndex < _masters.size())
    {
        XINPUT_STATE state;
        state.Gamepad = sdlJoystickToGamepad(_masters[masterIndex].joystick, _masters[masterIndex].isXInput);
        for (size_t i = 0; i < gamepads.size(); ++i)
        {
            if (gamepads[i].isPuppet)
            {
                gamepads[i].setState(state);
            }
        }
    }
}

void MasterOfPuppetsWidget::setMaster(int value, int& masterIndex, GamepadClient& gamepadClient)
{
    masterIndex = value;
    gamepadClient.isPuppetMaster = value >= 0;
    if (!gamepadClient.isPuppetMaster)
    {
        clearPuppets(gamepadClient.gamepads);
    }
}

void MasterOfPuppetsWidget::clearPuppets(vector<Gamepad>& gamepads)
{
    for (size_t i = 0; i < gamepads.size(); ++i)
    {
        if (gamepads[i].isPuppet)
        {
            gamepads[i].clearState();
        }
    }
}
