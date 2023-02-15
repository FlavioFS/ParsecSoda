#pragma once

#include "../Texture.h"

typedef ID3D11ShaderResourceView* Icon;

class AppIcons
{
public:
	static void init(ID3D11Device* pd3dDevice);

	static ID3D11ShaderResourceView* switchOn;
	static ID3D11ShaderResourceView* switchOff;
	static ID3D11ShaderResourceView* wifiOn;
	static ID3D11ShaderResourceView* wifiOff;

	static ID3D11ShaderResourceView* play;
	static ID3D11ShaderResourceView* stop;
	static ID3D11ShaderResourceView* skip;
	static ID3D11ShaderResourceView* remove;
	static ID3D11ShaderResourceView* cog;
	static ID3D11ShaderResourceView* copy;

	static ID3D11ShaderResourceView* yes;
	static ID3D11ShaderResourceView* no;

	static ID3D11ShaderResourceView* send;
	static ID3D11ShaderResourceView* back;
	static ID3D11ShaderResourceView* sendTop;

	static ID3D11ShaderResourceView* lock;
	static ID3D11ShaderResourceView* unlock;

	static ID3D11ShaderResourceView* padOn;
	static ID3D11ShaderResourceView* padOff;
	static ID3D11ShaderResourceView* plug;

	static ID3D11ShaderResourceView* micOn;
	static ID3D11ShaderResourceView* micOff;
	static ID3D11ShaderResourceView* speakersOn;
	static ID3D11ShaderResourceView* speakersOff;

	static ID3D11ShaderResourceView* video;

	static ID3D11ShaderResourceView* refresh;
	static ID3D11ShaderResourceView* timerRefresh;
	static ID3D11ShaderResourceView* sort;
	static ID3D11ShaderResourceView* submit;
	static ID3D11ShaderResourceView* saveOn;
	static ID3D11ShaderResourceView* saveOff;
	static ID3D11ShaderResourceView* edit;
	static ID3D11ShaderResourceView* image;
	static ID3D11ShaderResourceView* trash;
	static ID3D11ShaderResourceView* move;
	static ID3D11ShaderResourceView* moveOff;

	static ID3D11ShaderResourceView* chat;
	static ID3D11ShaderResourceView* users;
	static ID3D11ShaderResourceView* log;
	static ID3D11ShaderResourceView* logoff;

	static ID3D11ShaderResourceView* userOn;
	static ID3D11ShaderResourceView* userOff;
	static ID3D11ShaderResourceView* block;
	static ID3D11ShaderResourceView* kick;

	static ID3D11ShaderResourceView* info;
	static ID3D11ShaderResourceView* logo;

	static ID3D11ShaderResourceView* puppet;
	static ID3D11ShaderResourceView* puppetAdd;
	static ID3D11ShaderResourceView* xinput;
	static ID3D11ShaderResourceView* dinput;
	static ID3D11ShaderResourceView* ds4;
	static ID3D11ShaderResourceView* windows;
	static ID3D11ShaderResourceView* sdl;
	static ID3D11ShaderResourceView* seat;

	static ID3D11ShaderResourceView* xbox;
	static ID3D11ShaderResourceView* xbox1;
	static ID3D11ShaderResourceView* xbox2;
	static ID3D11ShaderResourceView* xbox3;
	static ID3D11ShaderResourceView* xbox4;

	static ID3D11ShaderResourceView* vpad;
	static ID3D11ShaderResourceView* multitap;
	static ID3D11ShaderResourceView* mirror;

	static ID3D11ShaderResourceView* btc;
	static ID3D11ShaderResourceView* ltc;
	static ID3D11ShaderResourceView* eth;

private:
	static Texture _switchOn;
	static Texture _switchOff;
	static Texture _wifiOn;
	static Texture _wifiOff;

	static Texture _play;
	static Texture _stop;
	static Texture _skip;
	static Texture _remove;
	static Texture _cog;
	static Texture _copy;

	static Texture _yes;
	static Texture _no;
	
	static Texture _send;
	static Texture _back;
	static Texture _sendTop;

	static Texture _padOn;
	static Texture _padOff;
	static Texture _plug;

	static Texture _lock;
	static Texture _unlock;
	
	static Texture _micOn;
	static Texture _micOff;
	static Texture _speakersOn;
	static Texture _speakersOff;

	static Texture _video;
	
	static Texture _refresh;
	static Texture _timerRefresh;
	static Texture _sort;
	static Texture _submit;
	static Texture _saveOn;
	static Texture _saveOff;
	static Texture _edit;
	static Texture _image;
	static Texture _trash;
	static Texture _move;
	static Texture _moveOff;

	static Texture _chat;
	static Texture _users;
	static Texture _log;
	static Texture _logoff;

	static Texture _userOn;
	static Texture _userOff;
	static Texture _block;
	static Texture _kick;

	static Texture _info;
	static Texture _logo;

	static Texture _puppet;
	static Texture _puppetAdd;
	static Texture _xinput;
	static Texture _dinput;
	static Texture _ds4;
	static Texture _windows;
	static Texture _sdl;
	static Texture _seat;

	static Texture _xbox;
	static Texture _xbox1;
	static Texture _xbox2;
	static Texture _xbox3;
	static Texture _xbox4;

	static Texture _vpad;
	static Texture _multitap;
	static Texture _mirror;

	static Texture _btc;
	static Texture _ltc;
	static Texture _eth;
};

