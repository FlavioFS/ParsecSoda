#pragma once

#include "Texture.h"

class AppIcons
{
public:
	AppIcons(ID3D11Device* pd3dDevice);

	ID3D11ShaderResourceView* empty;
	ID3D11ShaderResourceView* lock;
	ID3D11ShaderResourceView* mic;
	ID3D11ShaderResourceView* no;
	ID3D11ShaderResourceView* play;
	ID3D11ShaderResourceView* refresh;
	ID3D11ShaderResourceView* send;
	ID3D11ShaderResourceView* send_off;
	ID3D11ShaderResourceView* speakers;
	ID3D11ShaderResourceView* stop;
	ID3D11ShaderResourceView* unlock;
	ID3D11ShaderResourceView* yes;

private:
	Texture _empty;
	Texture _lock;
	Texture _mic;
	Texture _no;
	Texture _play;
	Texture _refresh;
	Texture _send;
	Texture _send_off;
	Texture _speakers;
	Texture _stop;
	Texture _unlock;
	Texture _yes;
};

