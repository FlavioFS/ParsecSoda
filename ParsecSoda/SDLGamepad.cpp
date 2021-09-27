#include "SDLGamepad.h"

GamepadState SDLGamepad::getGamepadState()
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
    pad.sThumbLX = SDL_JoystickGetAxis(joystick, 0);
    pad.sThumbLY = SDL_JoystickGetAxis(joystick, 1);
    pad.sThumbRX = SDL_JoystickGetAxis(joystick, 2);
    pad.sThumbRY = SDL_JoystickGetAxis(joystick, 3);

    if (pad.sThumbLY == SDL_JOYSTICK_AXIS_MIN) pad.sThumbLY = SDL_JOYSTICK_AXIS_MAX;
    else if (pad.sThumbLY == SDL_JOYSTICK_AXIS_MAX) pad.sThumbLY = SDL_JOYSTICK_AXIS_MIN;
    else     pad.sThumbLY = -pad.sThumbLY;

    if (pad.sThumbRY == SDL_JOYSTICK_AXIS_MIN) pad.sThumbRY = SDL_JOYSTICK_AXIS_MAX;
    else if (pad.sThumbRY == SDL_JOYSTICK_AXIS_MAX) pad.sThumbRY = SDL_JOYSTICK_AXIS_MIN;
    else     pad.sThumbRY = -pad.sThumbRY;


    // =====================================================
    // XInput
    // =====================================================
    if (type == Type::XBox)
    {
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_UP, (SDL_JoystickGetHat(joystick, 0) & SDL_HAT_UP) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, (SDL_JoystickGetHat(joystick, 0) & SDL_HAT_DOWN) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, (SDL_JoystickGetHat(joystick, 0) & SDL_HAT_LEFT) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, (SDL_JoystickGetHat(joystick, 0) & SDL_HAT_RIGHT) != 0);

        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_A, SDL_JoystickGetButton(joystick, (Uint8)XInput::A) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_B, SDL_JoystickGetButton(joystick, (Uint8)XInput::B) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_X, SDL_JoystickGetButton(joystick, (Uint8)XInput::X) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_Y, SDL_JoystickGetButton(joystick, (Uint8)XInput::Y) != 0);

        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_LEFT_SHOULDER, SDL_JoystickGetButton(joystick, (Uint8)XInput::LB) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_RIGHT_SHOULDER, SDL_JoystickGetButton(joystick, (Uint8)XInput::RB) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_BACK, SDL_JoystickGetButton(joystick, (Uint8)XInput::BACK) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_START, SDL_JoystickGetButton(joystick, (Uint8)XInput::START) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_LEFT_THUMB, SDL_JoystickGetButton(joystick, (Uint8)XInput::L3) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_RIGHT_THUMB, SDL_JoystickGetButton(joystick, (Uint8)XInput::R3) != 0);

        pad.bLeftTrigger = axisToByte(SDL_JoystickGetAxis(joystick, 4));
        pad.bRightTrigger = axisToByte(SDL_JoystickGetAxis(joystick, 5));
    }

    // =====================================================
    // DInput
    // =====================================================
    else if (type == Type::DS)
    {
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_UP, (SDL_JoystickGetHat(joystick, 0) & SDL_HAT_UP) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, (SDL_JoystickGetHat(joystick, 0) & SDL_HAT_DOWN) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, (SDL_JoystickGetHat(joystick, 0) & SDL_HAT_LEFT) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, (SDL_JoystickGetHat(joystick, 0) & SDL_HAT_RIGHT) != 0);

        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_A, SDL_JoystickGetButton(joystick, (Uint8)DInput::X) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_B, SDL_JoystickGetButton(joystick, (Uint8)DInput::Circle) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_X, SDL_JoystickGetButton(joystick, (Uint8)DInput::Square) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_Y, SDL_JoystickGetButton(joystick, (Uint8)DInput::Triangle) != 0);

        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_LEFT_SHOULDER, SDL_JoystickGetButton(joystick, (Uint8)DInput::L1) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_RIGHT_SHOULDER, SDL_JoystickGetButton(joystick, (Uint8)DInput::R1) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_BACK, SDL_JoystickGetButton(joystick, (Uint8)DInput::SELECT) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_START, SDL_JoystickGetButton(joystick, (Uint8)DInput::START) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_LEFT_THUMB, SDL_JoystickGetButton(joystick, (Uint8)DInput::L3) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_RIGHT_THUMB, SDL_JoystickGetButton(joystick, (Uint8)DInput::R3) != 0);

        pad.bLeftTrigger = (SDL_JoystickGetButton(joystick, (Uint8)DInput::L2) != 0) ? 255 : 0;
        pad.bRightTrigger = (SDL_JoystickGetButton(joystick, (Uint8)DInput::R2) != 0) ? 255 : 0;
    }
    
    // =====================================================
    // DS4
    // =====================================================
    else
    {
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_UP, SDL_JoystickGetButton(joystick, (int)DS4::DUp) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, SDL_JoystickGetButton(joystick, (int)DS4::DDown) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, SDL_JoystickGetButton(joystick, (int)DS4::DLeft) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, SDL_JoystickGetButton(joystick, (int)DS4::DRight) != 0);

        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_A, SDL_JoystickGetButton(joystick, (int)DS4::X) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_B, SDL_JoystickGetButton(joystick, (int)DS4::Circle) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_X, SDL_JoystickGetButton(joystick, (int)DS4::Square) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_Y, SDL_JoystickGetButton(joystick, (int)DS4::Triangle) != 0);

        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_LEFT_SHOULDER, SDL_JoystickGetButton(joystick, (int)DS4::L1) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_RIGHT_SHOULDER, SDL_JoystickGetButton(joystick, (int)DS4::R1) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_BACK, SDL_JoystickGetButton(joystick, (int)DS4::SELECT) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_START, SDL_JoystickGetButton(joystick, (int)DS4::START) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_LEFT_THUMB, SDL_JoystickGetButton(joystick, (int)DS4::L3) != 0);
        Bitwise::setValue(&pad.wButtons, XUSB_GAMEPAD_RIGHT_THUMB, SDL_JoystickGetButton(joystick, (int)DS4::R3) != 0);

        pad.bLeftTrigger = axisToByte(SDL_JoystickGetAxis(joystick, 4));
        pad.bRightTrigger = axisToByte(SDL_JoystickGetAxis(joystick, 5));
    }


    GamepadState result;
    result.state.Gamepad = pad;
    return result;
}

void SDLGamepad::cycleType()
{
    type = (Type)(((int)type + 1) % (int)Type::COUNT);
}


BYTE SDLGamepad::axisToByte(Sint16 axis)
{
    return (BYTE)(255 * ((float)(axis - SDL_JOYSTICK_AXIS_MIN) / SDL_AXIS_RANGE));
}