#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <iostream>
//#include <atlbase.h>
#include <atlcomcli.h>
#include "parsec-dso.h"
#include <vector>
#include <string>
#include <mutex>
#include "MetadataCache.h"

using namespace std;

class DX11
{
public:
	void clear();
	bool recover();
	bool clearAndRecover();
	bool init();
	bool captureScreen(ParsecDSO *ps);
	vector<string> listScreens();
	void setScreen(UINT index);
	UINT getScreen();

private:
	void fetchScreenList();

	mutex _mutex;

	// Windows
	HWND hwnd;

	// D3D11
	D3D11_TEXTURE2D_DESC d3TexDesc, _desc;
	vector<string> _screens;

	UINT _currentScreen = 0;
};