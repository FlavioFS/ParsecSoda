#include "AppIcons.h"

AppIcons::AppIcons(ID3D11Device* pd3dDevice)
{
    Texture yesIcon, noIcon, playIcon, stopIcon, lockIcon, unlockIcon;

    _empty.loadFromFile(pd3dDevice, "./icons/empty.png");
    _lock.loadFromFile(pd3dDevice, "./icons/lock.png");
    _mic.loadFromFile(pd3dDevice, "./icons/microphone.png");
    _no.loadFromFile(pd3dDevice, "./icons/close-circle.png");
    _play.loadFromFile(pd3dDevice, "./icons/play.png");
    _refresh.loadFromFile(pd3dDevice, "./icons/refresh.png");
    _send.loadFromFile(pd3dDevice, "./icons/send.png");
    _send_off.loadFromFile(pd3dDevice, "./icons/send-off.png");
    _speakers.loadFromFile(pd3dDevice, "./icons/speakers.png");
    _stop.loadFromFile(pd3dDevice, "./icons/stop.png");
    _unlock.loadFromFile(pd3dDevice, "./icons/unlock.png");
    _yes.loadFromFile(pd3dDevice, "./icons/check-on.png");

    empty = _empty.texture;
    lock = _lock.texture;
    mic = _mic.texture;
    no = _no.texture;
    play = _play.texture;
    refresh = _refresh.texture;
    send = _send.texture;
    send_off = _send_off.texture;
    speakers = _speakers.texture;
    stop = _stop.texture;
    unlock = _unlock.texture;
    yes = _yes.texture;
}
