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
	ID3D11Device *d3Device = NULL;
	ID3D11DeviceContext *d3DeviceContext = NULL;
	ID3D11RenderTargetView *d3Render = NULL;
	ID3D11Texture2D *d3Frame = NULL;
	D3D11_TEXTURE2D_DESC d3TexDesc, _desc;

	// IDGX
	IDXGISwapChain *dxSwapChain = NULL;
	IDXGIFactory *dxFactory = NULL;
	IDXGIAdapter *dxAdapter = NULL;
	IDXGIOutput *dxOutput = NULL;
	DXGI_OUTPUT_DESC dxOutDesc;
	DXGI_SWAP_CHAIN_DESC dxSwapDesc;
};

