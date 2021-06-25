#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <iostream>
//#include <atlbase.h>
#include <atlcomcli.h>
#include "parsec-dso.h"


class DX11
{
public:
	void clear();
	bool recover();
	bool init();
	bool captureScreen(ParsecDSO *ps);

private:
	// Windows
	HWND hwnd;

	// D3D11
	D3D11_TEXTURE2D_DESC d3TexDesc, _desc;
};

