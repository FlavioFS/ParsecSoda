#include "SDLGamepadReader.h"

vector<SDLGamepad>& SDLGamepadReader::getGamepads()
{
    return _gamepads;
}

void SDLGamepadReader::fetchGamepads()
{
    SDL_JoystickUpdate();

    eraseDetachedGamepads();

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        SDL_Joystick* joy = SDL_JoystickOpen(i);
        if (SDL_JoystickGetAttached(joy))
        {
            if (contains(joy))
            {
                continue;
            }

            _gamepads.push_back(SDLGamepad(joy));
        }
    }
}

void SDLGamepadReader::eraseDetachedGamepads()
{
    vector<SDLGamepad>::iterator it;
    for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
    {
        if (_gamepads.empty())
        {
            break;
        }

        if (!SDL_JoystickGetAttached((*it).joystick))
        {
            SDL_JoystickClose((*it).joystick);
            _gamepads.erase(it);

            if (_gamepads.empty())
            {
                break;
            }
            else
            {
                it = _gamepads.begin();
            }
        }
    }
}

bool SDLGamepadReader::contains(SDL_Joystick* joystick)
{
    vector<SDLGamepad>::iterator it;
    for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
    {
        if ((*it).joystick == joystick)
        {
            return true;
        }
    }

    return false;
}