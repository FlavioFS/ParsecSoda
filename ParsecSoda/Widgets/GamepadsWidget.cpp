#include "GamepadsWidget.h"

GamepadsWidget::GamepadsWidget(Hosting& hosting)
    : _hosting(hosting), _gamepads(hosting.getGamepads())
{
}

bool GamepadsWidget::render()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 5));

    static ImVec2 cursor;
    static bool isWindowLocked = false;
    static bool isConnectionButtonPressed = false;
    static ImVec2 dummySize = ImVec2(0.0f, 5.0f);
    static string name, id, tooltipTitle, tooltipDescription;

    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(250, 280), ImVec2(800, 1100));
    ImGui::Begin("Virtual Gamepads", (bool*)0, isWindowLocked ? (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize) : 0);
    AppStyle::pushInput();
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();

    static float indentDistance;

    renderTopBar(isWindowLocked, size);


    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));

    AnimatedGamepadWidget::tryResizeAnimations(_gamepadAnimations, _gamepads.size());

    for (size_t i = 0; i < _gamepads.size(); ++i)
    {
        AGamepad* gi = _gamepads[i];
        static uint32_t userID;
        userID = gi->owner.guest.userID;

        ImGui::BeginGroup();
        renderPadInputTypeIcon(gi);

        id = "###Connect gamepad" + to_string(i);
        if (ToggleIconButtonWidget::render(AppIcons::padOn, AppIcons::padOff, gi->isConnected(), id.c_str()))
        {
            if (gi->isConnected()) gi->disconnect();
            else gi->connect();

            isConnectionButtonPressed = true; // Refresh index UI - XInput workaround
        }
        if (gi->isConnected())  TitleTooltipWidget::render("Connected gamepad", "Press to \"physically\" disconnect\nthis gamepad (at O.S. level).");
        else                    TitleTooltipWidget::render("Disconnected gamepad", "Press to \"physically\" connect\nthis gamepad (at O.S. level).");
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(5, 1));
        ImGui::SameLine();

        ImGui::BeginGroup();

        // ==================
        // Username and ID
        // ==================
        static float gamepadLabelWidth;
        gamepadLabelWidth = size.x - 50.0f;

        ImGui::BeginGroup();
        cursor = ImGui::GetCursorPos();

        ImGui::AlignTextToFramePadding();

        if (gi->owner.guest.isValid())
        {
            id = string() + "# " + to_string(gi->owner.guest.userID);
            name = gi->owner.guest.name;
        }
        else if (_hosting.getGamepadClient().isPuppetMaster && gi->isPuppet)
        {
            id = string() + "# " + to_string(_hosting.getHost().userID);
            name = _hosting.getHost().name;
        }
        else {
            id = "    ";
            name = gi->owner.guest.name;
        }

        AppStyle::pushLabel();
        ImGui::TextWrapped(id.c_str());
        AppStyle::pop();

        AppFonts::pushInput();
        AppColors::pushPrimary();
        ImGui::SetNextItemWidth(gamepadLabelWidth);
        ImGui::Text(name.c_str());
        AppColors::pop();
        AppFonts::pop();

        static ImVec2 backupCursor;
        backupCursor = ImGui::GetCursorPos();

        ImGui::SetCursorPos(cursor);
        ImGui::Button(
            (string("##gamepad button") + to_string(i + 1)).c_str(),
            ImVec2(gamepadLabelWidth, 40.0f)
        );

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("Gamepad", &i, sizeof(int));

            AppFonts::pushInput();
            AppColors::pushPrimary();
            ImGui::Text("%s", (gi->owner.guest.isValid() ? gi->owner.guest.name.c_str() : "Empty gamepad"));
            AppColors::pop();
            AppFonts::pop();

            AppStyle::pushLabel();
            ImGui::Text("Drop into another Gamepad to swap.");
            AppStyle::pop();

            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Guest"))
            {
                if (payload->DataSize == sizeof(int))
                {
                    int guestIndex = *(const int*)payload->Data;

                    _hosting.getGuestList().findByIndexSafe(guestIndex, [&](Guest* guest) {
                        gi->owner.guest.copy(*guest);
                        });
                }
            }
            else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Gamepad"))
            {
                if (payload->DataSize == sizeof(int))
                {
                    int sourceIndex = *(const int*)payload->Data;
                    if (sourceIndex >= 0 && sourceIndex < _gamepads.size())
                    {
                        static GuestDevice backupOwner;
                        backupOwner.copy(_gamepads[i]->owner);

                        _gamepads[i]->copyOwner(_gamepads[sourceIndex]);
                        _gamepads[sourceIndex]->owner.copy(backupOwner);
                        _gamepads[i]->clearState();
                        _gamepads[sourceIndex]->clearState();
                    }
                }
            }

            ImGui::EndDragDropTarget();
        }

        ImGui::SetCursorPos(backupCursor);

        ImGui::EndGroup();
        
        

        // ==================
        // Badge Buttons
        // ==================
        tooltipTitle = "Strip gamepad";
        tooltipDescription = "Unlink current user from this gamepad.";
        id = "###Strip gamepad " + to_string(i);
        if (BadgeButtonWidget::render(AppIcons::back, tooltipTitle, tooltipDescription, id))
        {
            gi->clearOwner();
        }

        ImGui::SameLine();

        id = "###Lock gamepad" + to_string(i);
        if (ToggleIconButtonWidget::render(AppIcons::lock, AppIcons::unlock, gi->isLocked(), id.c_str(), AppColors::red, AppColors::negative, ImVec2(25, 25)))
        {
            gi->toggleLocked();
        }
        if(gi->isLocked())  TitleTooltipWidget::render("Locked gamepad", "Lock this specific gamepad, preventing inputs or picking.");
        else                TitleTooltipWidget::render("Unlocked gamepad", "Unlock this specific gamepad, allowing inputs and picking.");

        ImGui::SameLine();
        
        if (_hosting.getGamepadClient().isPuppetMaster && gi->isPuppet)
        {
            BadgeIconWidget::render(AppIcons::puppet, "Puppet", "This gamepad is under control of Master of Puppets.");
        }
        else
        {
            static bool isMultitap, isMirror, isButtonLock;
            static ImVec4 iconColor;
            tooltipTitle = "Multitap (disabled)";
            tooltipDescription = "DeviceID ignored.\nA guest account may only use 1 gamepad.\nMultiple gamepads will overlap.";
            iconColor = AppColors::negative;

            isMultitap = MetadataCache::preferences.defaultMultitapValue;
            isMirror = MetadataCache::preferences.defaultMirrorValue;
            isButtonLock = MetadataCache::preferences.buttonLock.isEnabled;
            if (gi->owner.guest.isValid())
            {
                _hosting.getGamepadClient().findPreferences(gi->owner.guest.userID, [&](GamepadClient::GuestPreferences& prefs) {
                    isMultitap = prefs.isMultitap();
                    isMirror = prefs.isMirror();
                    isButtonLock = prefs.isButtonLock();
                });
            }

            if (isButtonLock)
            {
                tooltipTitle = "Button lock enabled";
                tooltipDescription = "Guest can only press allowed buttons.\n\n* Setup at Button Lock window.";
                iconColor = AppColors::positive;
            }
            else
            {
                tooltipTitle = "Button lock disabled";
                tooltipDescription = "Guest can press any button.\n\n* Setup at Button Lock window.";
                iconColor = AppColors::negative;
            }
            id = "###Button lock " + to_string(i);
            if (BadgeButtonWidget::render(AppIcons::buttonLock, tooltipTitle, tooltipDescription, id, ImVec2(25, 25), iconColor))
            {
                if (gi->owner.guest.isValid()) _hosting.getGamepadClient().toggleButtonLock(gi->owner.guest.userID);
            }

            ImGui::SameLine();

            if (isMirror)
            {
                tooltipTitle = "Mirror enabled";
                tooltipDescription = "DPad and Left Stick mirror each other.";
                iconColor = AppColors::positive;
            }
            else
            {
                tooltipTitle = "Mirror disabled";
                tooltipDescription = "DPad and Left Stick are independent.";
                iconColor = AppColors::negative;
            }
            id = "###Mirror dpad " + to_string(i);
            if (BadgeButtonWidget::render(AppIcons::mirror, tooltipTitle, tooltipDescription, id, ImVec2(25, 25), iconColor))
            {
                if (gi->owner.guest.isValid()) _hosting.getGamepadClient().toggleMirror(gi->owner.guest.userID);
            }

            ImGui::SameLine();


            if (isMultitap)
            {
                tooltipTitle = "Multitap enabled";
                tooltipDescription = "DeviceID is active.\nA guest may have multiple gamepads\nin the same account.";
                iconColor = AppColors::positive;
            }
            else
            {
                tooltipTitle = "Multitap disabled";
                tooltipDescription = "DeviceID ignored.\nA guest account may only use 1 gamepad.\nMultiple gamepads will overlap.";
                iconColor = AppColors::negative;
            }
            id = "###Guest multitap " + to_string(i);
            if (BadgeButtonWidget::render(AppIcons::multitap, tooltipTitle, tooltipDescription, id, ImVec2(25,25), iconColor))
            {
                if (gi->owner.guest.isValid()) _hosting.getGamepadClient().toggleMultitap(gi->owner.guest.userID);
            }

            ImGui::SameLine();

            static int deviceIndices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
            ImGui::BeginGroup();
            ImGui::Dummy(ImVec2(0.0f, 3.0f));
            ImGui::SetNextItemWidth(40);
            deviceIndices[i] = gi->owner.deviceID;

            AppFonts::pushTitle();
            if (ImGui::DragInt(
                (string("##DeviceIndex") + to_string(i)).c_str(),
                &deviceIndices[i], 0.1f, -1, 65536
            ))
            {
                gi->owner.deviceID = deviceIndices[i];
            }
            if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            AppFonts::pop();
            ImGui::EndGroup();
        }

        _gamepadAnimations[i].render(gi->getState().Gamepad, 25.0f);

        ImGui::EndGroup();

        ImGui::Dummy(ImVec2(0, 5));
    }
    ImGui::PopStyleVar(4);

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();


    if (isConnectionButtonPressed)
    {
        isConnectionButtonPressed = false;
        for (size_t i = 0; i < _gamepads.size(); ++i)
        {
            _gamepads[i]->refreshIndex();
        }
    }

    return true;
}

void GamepadsWidget::renderTopBar(bool& isWindowLocked, const ImVec2& windowSize)
{
    static ImVec2 buttonSize = ImVec2(30, 30);

    if (IconButton::render(AppIcons::refresh, AppColors::primary, buttonSize))
    {
        _hosting.getGamepadClient().resetAll();
    }
    TitleTooltipWidget::render("Reset gamepad engine", "If all else fails, try this button.\nPress in dire situations.");

    ImGui::SameLine();

    if (IconButton::render(AppIcons::padOff, AppColors::primary, buttonSize))
    {
        _hosting.getGamepadClient().disconnectAllGamepads();
    }
    TitleTooltipWidget::render("Disconnect all gamepads", "Disconnects all of the gamepads currently enabled.");

    ImGui::SameLine();

    if (IconButton::render(AppIcons::sort, AppColors::primary, buttonSize))
    {
        _hosting.getGamepadClient().sortGamepads();
    }
    TitleTooltipWidget::render("Sort gamepads", "Re-sort all gamepads by index.");

    ImGui::SameLine();

    if (ToggleIconButtonWidget::render(
        AppIcons::lock, AppIcons::unlock, _hosting.isGamepadLock(),
        AppColors::red, AppColors::negative, buttonSize
    ))
    {
        _hosting.toggleGamepadLock();
    }
    if (_hosting.isGamepadLock())   TitleTooltipWidget::render("Unlock guest inputs", "Guests will be able to control gamepads again.");
    else                            TitleTooltipWidget::render("Lock guest inputs", "Guest inputs will be locked out of gamepads.");

    ImGui::SameLine();

    ImGui::BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonRight);

    ImGui::SameLine();


    IconButton::render(AppIcons::cog, AppColors::primary, buttonSize, "###Gamepad settings button");
    TitleTooltipWidget::render("Gamepad Settings", "Press to open Gamepad Settings menu.");
    renderOptionsMenu();


    ImGui::SameLine();

    ImGui::SetCursorPosX(windowSize.x - 25);
    if (ToggleIconButtonWidget::render(AppIcons::moveOff, AppIcons::move, isWindowLocked, AppColors::negative, AppColors::positive, buttonSize))
    {
        isWindowLocked = !isWindowLocked;
    }
    if (isWindowLocked) TitleTooltipWidget::render("Window Locked", "This window cannot move or resize.");
    else TitleTooltipWidget::render("Window Unlocked", "This window can move and resize.");

    ImGui::Dummy(ImVec2(0, 5));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 10));

    static const function<void(void)> releaseDragCallback = [&]() {
        _hosting.getGamepadClient().resize(MetadataCache::preferences.xboxPuppetCount, MetadataCache::preferences.ds4PuppetCount);
        MetadataCache::savePreferences();
    };

    static ImVec2 intRangeChildSize = ImVec2(85, 50);
    ImGui::Image(AppIcons::xinput, ImVec2(35, 35), ImVec2(0, 0), ImVec2(1, 1), AppColors::backgroundIcon);
    ImGui::SameLine();
    ImGui::BeginChild("Child: XBox Puppet Counter", intRangeChildSize); // Avoids imgui render bug (arrow button spacing in second IntRange)
    if (IntRangeWidget::render<uint32_t>("xboxCounter", MetadataCache::preferences.xboxPuppetCount, 0, 100, releaseDragCallback))
    {
        TitleTooltipWidget::render("XBox Puppet Counter", "Set the amount of XBox controllers.\n\n* Warning: disconnect all gamepads before changing this.");
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(5, 0));
    ImGui::SameLine();

    ImGui::Image(AppIcons::dinput, ImVec2(35, 35), ImVec2(0, 0), ImVec2(1, 1), AppColors::backgroundIcon);
    ImGui::SameLine();
    ImGui::BeginChild("Child: Dualshock Puppet Counter", intRangeChildSize);
    if (IntRangeWidget::render<uint32_t>("ds4Counter", MetadataCache::preferences.ds4PuppetCount, 0, 100, releaseDragCallback))
    {
        TitleTooltipWidget::render("Dualshock Puppet Counter", "Set the amount of DS4 controllers.\n\n* Warning: disconnect all gamepads before changing this.");
    }
    ImGui::EndChild();
}

void GamepadsWidget::renderOptionsMenu()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

    if (ImGui::BeginPopupContextItem("Gamepad Options", ImGuiPopupFlags_MouseButtonLeft))
    {
        if (SwitchWidget::render(
            MetadataCache::preferences.defaultMirrorValue,
            "###Mirror default switch", "Default mirror",
            "Mirror [ON]", "Default behaviour: enabled.\n\nLeft Stick and Dpad mirror each other.\nUse this if you need to choose between\nDPad and Analog.",
            "Mirror [OFF]", "Default behaviour: disabled.\n\nLeft Stick and Dpad are independent.\nUse this if you need to choose between\nDPad and Analog."
        ))
        {
            MetadataCache::savePreferences();
        }

        if (SwitchWidget::render(
            MetadataCache::preferences.defaultMultitapValue,
            "###Multitap default switch", "Default multitap",
            "Multitap [ON]", "Default behaviour: enabled.\n\nDeviceID active.\nA guest may have multiple gamepads\nin the same account.",
            "Multitap [OFF]", "Default behaviour: disabled.\n\nDeviceID ignored.\nA guest account may only use 1 gamepad.\nMultiple gamepads will overlap."
        ))
        {
            MetadataCache::savePreferences();
        }

        ImGui::Indent(8);
        AppStyle::pushLabel();
        ImGui::Text("Default Multitap Limit");
        AppStyle::pop();
        ImGui::Dummy(ImVec2(0, 4));
        if (IntRangeWidget::render<uint32_t>("###Default Multitap limit", MetadataCache::preferences.defaultMultitapPadLimit, 0, 100, [&]() {
            MetadataCache::savePreferences();
        }))
        {
            TitleTooltipWidget::render(
                "Default Gamepad Limit for Multitap",
                "Set the ammount of puppets a single Guest account can request when multitap is enabled."
            );
        }
        ImGui::Unindent();

        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
}

void GamepadsWidget::renderPadInputTypeIcon(AGamepad* pad)
{
    static ImVec2 cursor;
    static const ImVec2 ICON_SIZE = ImVec2(40, 40);
    const static vector<Icon> xboxIcons{ AppIcons::xbox1, AppIcons::xbox2, AppIcons::xbox3, AppIcons::xbox4 };

    cursor = ImGui::GetCursorPos();
    static int xboxIndex = 0, padIndex = 0;
    xboxIndex = (int)pad->getIndex();
    padIndex = xboxIndex + 1;
    static bool isIndexSuccess = false;
    isIndexSuccess = pad->isConnected() && padIndex > 0 && padIndex <= 4;
    
    ImGui::BeginGroup();

    switch (pad->type())
    {
    case AGamepad::Type::XBOX:
        if (isIndexSuccess)
        {
            ImGui::Image(
                AppIcons::xbox,
                ICON_SIZE, ImVec2(0, 0), ImVec2(1, 1), AppColors::white
            );

            ImGui::SetCursorPos(cursor);
            ImGui::Image(
                xboxIcons[xboxIndex],
                ICON_SIZE, ImVec2(0, 0), ImVec2(1, 1), AppColors::primary
            );
            TitleTooltipWidget::render(
                (string() + "XInput " + to_string(padIndex)).c_str(),
                (
                    string("This controller is using XInput slot ") + to_string(padIndex) + ".\n\n" +
                    "* Remember:\nYour physical controllers may also occupy XInput slots."
                    ).c_str()
            );

        }
        else
        {
            ImGui::Image(
                AppIcons::xinput,
                ICON_SIZE, ImVec2(0, 0), ImVec2(1, 1), AppColors::backgroundIcon
            );
            TitleTooltipWidget::render(
                (string() + "XBox controller").c_str(),
                (
                    string("This is an XBox controller out of XInput range.") + "\n\n" +
                    "* It still works, but as a generic controller without XInput features."
                    ).c_str()
            );
        }
        break;
    case AGamepad::Type::DUALSHOCK:
        ImGui::Image(
            AppIcons::dinput,
            ICON_SIZE, ImVec2(0, 0), ImVec2(1, 1), AppColors::backgroundIcon
        );
        TitleTooltipWidget::render(
            (string() + "Dualshock controller").c_str(),
            (
                string("This is a Dualshock 4 controller.")).c_str()
        );
        break;
    default:
        break;
    }
    ImGui::EndGroup();
}
