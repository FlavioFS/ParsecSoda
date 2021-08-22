#include "AppIcons.h"


ID3D11ShaderResourceView* AppIcons::play;
ID3D11ShaderResourceView* AppIcons::stop;

ID3D11ShaderResourceView* AppIcons::yes;
ID3D11ShaderResourceView* AppIcons::no;

ID3D11ShaderResourceView* AppIcons::send;
ID3D11ShaderResourceView* AppIcons::back;

ID3D11ShaderResourceView* AppIcons::lock;
ID3D11ShaderResourceView* AppIcons::unlock;

ID3D11ShaderResourceView* AppIcons::padOn;
ID3D11ShaderResourceView* AppIcons::padOff;
ID3D11ShaderResourceView* AppIcons::plug;

ID3D11ShaderResourceView* AppIcons::micOn;
ID3D11ShaderResourceView* AppIcons::micOff;
ID3D11ShaderResourceView* AppIcons::speakersOn;
ID3D11ShaderResourceView* AppIcons::speakersOff;

ID3D11ShaderResourceView* AppIcons::video;

ID3D11ShaderResourceView* AppIcons::refresh;
ID3D11ShaderResourceView* AppIcons::sort;
ID3D11ShaderResourceView* AppIcons::submit;

ID3D11ShaderResourceView* AppIcons::chat;
ID3D11ShaderResourceView* AppIcons::users;
ID3D11ShaderResourceView* AppIcons::log;
ID3D11ShaderResourceView* AppIcons::logoff;

ID3D11ShaderResourceView* AppIcons::userOn;
ID3D11ShaderResourceView* AppIcons::userOff;
ID3D11ShaderResourceView* AppIcons::block;

ID3D11ShaderResourceView* AppIcons::info;
ID3D11ShaderResourceView* AppIcons::logo;

Texture AppIcons::_play;
Texture AppIcons::_stop;

Texture AppIcons::_yes;
Texture AppIcons::_no;

Texture AppIcons::_send;
Texture AppIcons::_back;

Texture AppIcons::_padOn;
Texture AppIcons::_padOff;
Texture AppIcons::_plug;

Texture AppIcons::_lock;
Texture AppIcons::_unlock;

Texture AppIcons::_micOn;
Texture AppIcons::_micOff;
Texture AppIcons::_speakersOn;
Texture AppIcons::_speakersOff;

Texture AppIcons::_video;

Texture AppIcons::_refresh;
Texture AppIcons::_sort;
Texture AppIcons::_submit;

Texture AppIcons::_chat;
Texture AppIcons::_users;
Texture AppIcons::_log;
Texture AppIcons::_logoff;

Texture AppIcons::_userOn;
Texture AppIcons::_userOff;
Texture AppIcons::_block;

Texture AppIcons::_info;
Texture AppIcons::_logo;


void AppIcons::init(ID3D11Device* pd3dDevice)
{
    _play.loadFromFile(pd3dDevice, "./icons/play.png");
    _stop.loadFromFile(pd3dDevice, "./icons/stop.png");

    _yes.loadFromFile(pd3dDevice, "./icons/check-on.png");
    _no.loadFromFile(pd3dDevice, "./icons/close-circle.png");

    _lock.loadFromFile(pd3dDevice, "./icons/lock.png");
    _unlock.loadFromFile(pd3dDevice, "./icons/unlock.png");

    _padOn.loadFromFile(pd3dDevice, "./icons/pad-on.png");
    _padOff.loadFromFile(pd3dDevice, "./icons/pad-off.png");
    _plug.loadFromFile(pd3dDevice, "./icons/plug.png");

    _send.loadFromFile(pd3dDevice, "./icons/send.png");
    _back.loadFromFile(pd3dDevice, "./icons/back.png");

    _micOn.loadFromFile(pd3dDevice, "./icons/microphone-on.png");
    _micOff.loadFromFile(pd3dDevice, "./icons/microphone-off.png");
    _speakersOn.loadFromFile(pd3dDevice, "./icons/speakers-on.png");
    _speakersOff.loadFromFile(pd3dDevice, "./icons/speakers-off.png");

    _video.loadFromFile(pd3dDevice, "./icons/video.png");

    _refresh.loadFromFile(pd3dDevice, "./icons/refresh.png");
    _sort.loadFromFile(pd3dDevice, "./icons/sort.png");
    _submit.loadFromFile(pd3dDevice, "./icons/submit.png");

    _chat.loadFromFile(pd3dDevice, "./icons/chat.png");
    _users.loadFromFile(pd3dDevice, "./icons/users.png");
    _log.loadFromFile(pd3dDevice, "./icons/log.png");
    _logoff.loadFromFile(pd3dDevice, "./icons/logoff.png");

    _userOn.loadFromFile(pd3dDevice, "./icons/user-on.png");
    _userOff.loadFromFile(pd3dDevice, "./icons/user-off.png");
    _block.loadFromFile(pd3dDevice, "./icons/block.png");

    _info.loadFromFile(pd3dDevice, "./icons/info.png");
    _logo.loadFromFile(pd3dDevice, "./icons/logo.png");

    play = _play.texture;
    stop = _stop.texture;
    
    yes = _yes.texture;
    no = _no.texture;
    
    lock = _lock.texture;
    unlock = _unlock.texture;

    padOn = _padOn.texture;
    padOff = _padOff.texture;
    plug = _plug.texture;
    
    send = _send.texture;
    back = _back.texture;
    
    micOn = _micOn.texture;
    micOff = _micOff.texture;
    speakersOn = _speakersOn.texture;
    speakersOff = _speakersOff.texture;
    
    video = _video.texture;

    refresh = _refresh.texture;
    sort = _sort.texture;
    submit = _submit.texture;

    chat = _chat.texture;
    users = _users.texture;
    log = _log.texture;
    logoff = _logoff.texture;
    block = _block.texture;

    userOn = _userOn.texture;
    userOff = _userOff.texture;

    info = _info.texture;
    logo = _logo.texture;
}
