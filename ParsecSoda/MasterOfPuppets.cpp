#include "MasterOfPuppets.h"

MasterOfPuppets::MasterOfPuppets()
    : _gamepadClient(nullptr), masterIndex(-1), isSDLEngine(true)
{
    stopwatch = Stopwatch(5000);
}

void MasterOfPuppets::init(GamepadClient& gamepadClient)
{
    _gamepadClient = &gamepadClient;
    _xinputReader.init();
}

void MasterOfPuppets::start()
{
    if (isRunning) return;

    isRunning = true;
    _thread = thread([&]() {
        stopwatch.start();

        while (isRunning)
        {
            inputMutex.lock();
            if (isSDLEngine) {
                if (stopwatch.isFinished())
                {
                    internalFetchSDLGamepads();
                    stopwatch.reset();
                }

                SDL_JoystickUpdate();
                mapFromSDL();
            }
            else
            {
                mapFromXInput();
            }

            if (_isVirtualPad)
            {
                mapFromVirtualMaster();
            }

            inputMutex.unlock();
            Sleep(8);
        }

        stopwatch.stop();
        _thread.detach();
        });
}

void MasterOfPuppets::stop()
{
    isRunning = false;
}

void MasterOfPuppets::fetchSDLGamepads()
{
    if (isSDLEngine)
    {
        inputMutex.lock();
        internalFetchSDLGamepads();
        inputMutex.unlock();
    }
}

void MasterOfPuppets::internalFetchSDLGamepads()
{
    if (isSDLEngine) _sdlReader.fetchGamepads();
}

void MasterOfPuppets::mapInputs()
{
    if (isSDLEngine) mapFromSDL();
    else mapFromXInput();
}

void MasterOfPuppets::setMasterIndex(int index)
{
    masterIndex = index;
    _gamepadClient->isPuppetMaster = (index >= 0) || _isVirtualPad;
}

int MasterOfPuppets::getMasterIndex()
{
    return masterIndex;
}

void MasterOfPuppets::setVirtualMaster(bool enabled)
{
    _isVirtualPad = enabled;
    _gamepadClient->isPuppetMaster = (masterIndex >= 0) || _isVirtualPad;
}

bool MasterOfPuppets::getVirtualMaster()
{
    return _isVirtualPad;
}

void MasterOfPuppets::setVirtualMasterState(XINPUT_GAMEPAD vpad)
{
    ZeroMemory(&_vMasterState, sizeof(XINPUT_STATE));
    _vMasterState.Gamepad = vpad;
}

vector<SDLGamepad>& MasterOfPuppets::getSDLGamepads()
{
    return _sdlReader.getGamepads();
}

vector<GamepadState>& MasterOfPuppets::getXInputGamepads()
{
    return _xinputReader.readInputs();
}

void MasterOfPuppets::mapFromVirtualMaster()
{
    if (_gamepadClient == nullptr) return;

    vector<AGamepad*>& puppets = _gamepadClient->gamepads;

    for (size_t i = 0; i < puppets.size(); ++i)
    {
        if (puppets[i]->isPuppet)
        {
            puppets[i]->setState(_vMasterState);
        }
    }
}

void MasterOfPuppets::mapFromSDL()
{
    if (_gamepadClient == nullptr) return;

    vector<AGamepad*>& puppets = _gamepadClient->gamepads;
    vector<SDLGamepad>& gamepads = _sdlReader.getGamepads();

    if (masterIndex >= 0 && masterIndex < gamepads.size())
    {
        for (size_t i = 0; i < puppets.size(); ++i)
        {
            if (puppets[i]->isPuppet)
            {
                puppets[i]->setState(gamepads[masterIndex].getGamepadState().state);
            }
        }
    }
}

void MasterOfPuppets::mapFromXInput()
{
    if (_gamepadClient == nullptr) return;

    vector<AGamepad*>& puppets = _gamepadClient->gamepads;
    vector<GamepadState>& gamepads = _xinputReader.readInputs();

    if (masterIndex >= 0 && masterIndex < gamepads.size())
    {
        for (size_t i = 0; i < puppets.size(); ++i)
        {
            if (puppets[i]->isPuppet)
            {
                puppets[i]->setState(gamepads[masterIndex].state);
            }
        }
    }
}