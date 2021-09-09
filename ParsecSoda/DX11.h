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
	class GPU
	{
	public:
		GPU(IDXGIAdapter1* adapter, DXGI_ADAPTER_DESC1 desc) : adapter(adapter), desc(desc) {}
		IDXGIAdapter1* adapter;
		DXGI_ADAPTER_DESC1 desc;
	};


public:
	void clear();
	bool recover();
	bool recover2();
	bool clearAndRecover();
	void enumGPUS();
	bool init();
	bool captureScreen(ParsecDSO *ps);
	
	vector<string> listScreens();
	void setScreen(UINT index);
	UINT getScreen();
	
	vector<string> listGPUs();
	void setGPU(size_t index);
	size_t getGPU();


private:
	void fetchScreenList();
	mutex _mutex;

	// Windows
	HWND hwnd;

	// D3D11
	D3D11_TEXTURE2D_DESC d3TexDesc, _desc;
	vector<string> _screens;

	UINT _currentScreen = 0;
	size_t _currentGPU = 0;

	vector<GPU> _gpus;
	vector<string> _gpuNames;
};