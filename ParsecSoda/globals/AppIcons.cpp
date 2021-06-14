#include "AppIcons.h"


ID3D11ShaderResourceView* AppIcons::play;
ID3D11ShaderResourceView* AppIcons::stop;

ID3D11ShaderResourceView* AppIcons::yes;
ID3D11ShaderResourceView* AppIcons::no;

ID3D11ShaderResourceView* AppIcons::send;
ID3D11ShaderResourceView* AppIcons::back;

ID3D11ShaderResourceView* AppIcons::lock;
ID3D11ShaderResourceView* AppIcons::unlock;

ID3D11ShaderResourceView* AppIcons::padon;
ID3D11ShaderResourceView* AppIcons::padoff;
ID3D11ShaderResourceView* AppIcons::plug;

ID3D11ShaderResourceView* AppIcons::mic;
ID3D11ShaderResourceView* AppIcons::speakers;

ID3D11ShaderResourceView* AppIcons::refresh;

Texture AppIcons::_play;
Texture AppIcons::_stop;

Texture AppIcons::_yes;
Texture AppIcons::_no;

Texture AppIcons::_send;
Texture AppIcons::_back;

Texture AppIcons::_padon;
Texture AppIcons::_padoff;
Texture AppIcons::_plug;

Texture AppIcons::_lock;
Texture AppIcons::_unlock;

Texture AppIcons::_mic;
Texture AppIcons::_speakers;

Texture AppIcons::_refresh;

void AppIcons::init(ID3D11Device* pd3dDevice)
{
    _play.loadFromFile(pd3dDevice, "./icons/play.png");
    _stop.loadFromFile(pd3dDevice, "./icons/stop.png");

    _yes.loadFromFile(pd3dDevice, "./icons/check-on.png");
    _no.loadFromFile(pd3dDevice, "./icons/close-circle.png");

    _lock.loadFromFile(pd3dDevice, "./icons/lock.png");
    _unlock.loadFromFile(pd3dDevice, "./icons/unlock.png");

    _padon.loadFromFile(pd3dDevice, "./icons/pad-on.png");
    _padoff.loadFromFile(pd3dDevice, "./icons/pad-off.png");
    _plug.loadFromFile(pd3dDevice, "./icons/plug.png");

    _send.loadFromFile(pd3dDevice, "./icons/send.png");
    _back.loadFromFile(pd3dDevice, "./icons/back.png");

    _mic.loadFromFile(pd3dDevice, "./icons/microphone.png");
    _speakers.loadFromFile(pd3dDevice, "./icons/speakers.png");

    _refresh.loadFromFile(pd3dDevice, "./icons/refresh.png");

    play = _play.texture;
    stop = _stop.texture;
    
    yes = _yes.texture;
    no = _no.texture;
    
    lock = _lock.texture;
    unlock = _unlock.texture;

    padon = _padon.texture;
    padoff = _padoff.texture;
    plug = _plug.texture;
    
    send = _send.texture;
    back = _back.texture;
    
    mic = _mic.texture;
    speakers = _speakers.texture;
    
    refresh = _refresh.texture;
}
