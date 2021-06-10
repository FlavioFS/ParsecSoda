#include "AppIcons.h"

AppIcons::AppIcons(ID3D11Device* pd3dDevice)
{
    Texture yesIcon, noIcon, playIcon, stopIcon, lockIcon, unlockIcon;

    _iconYes.loadFromFile(pd3dDevice, "./icons/check-on.png");
    _iconNo.loadFromFile(pd3dDevice, "./icons/close-circle.png");
    _iconPlay.loadFromFile(pd3dDevice, "./icons/play.png");
    _iconStop.loadFromFile(pd3dDevice, "./icons/stop.png");
    _iconUnlock.loadFromFile(pd3dDevice, "./icons/unlock.png");
    _iconLock.loadFromFile(pd3dDevice, "./icons/lock.png");
    _iconSend.loadFromFile(pd3dDevice, "./icons/send.png");
    _iconRefresh.loadFromFile(pd3dDevice, "./icons/refresh.png");
    _iconMic.loadFromFile(pd3dDevice, "./icons/microphone.png");
    _iconSpeakers.loadFromFile(pd3dDevice, "./icons/speakers.png");
    _iconEmpty.loadFromFile(pd3dDevice, "./icons/empty.png");
}
