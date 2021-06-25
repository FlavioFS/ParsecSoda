#include "DX11.h"


D3D_FEATURE_LEVEL gFeatureLevels[] = {
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1,
};
UINT gNumFeatureLevels = 6;
ID3D11Device *_lDevice;
ID3D11DeviceContext *_lImmediateContext;
IDXGIOutputDuplication *_lDeskDupl;
DXGI_OUTPUT_DESC _lOutputDesc;
DXGI_OUTDUPL_DESC _lOutputDuplDesc;
ID3D11Texture2D *_lAcquiredDesktopImage;
D3D11_MAPPED_SUBRESOURCE _resource;


void DX11::clear()
{
	if (_lDevice != nullptr) _lDevice->Release();
	if (_lDeskDupl != nullptr) _lDeskDupl->Release();
	if (_lAcquiredDesktopImage != nullptr) _lAcquiredDesktopImage->Release();
}

bool DX11::recover()
{
	HRESULT hr;

	// Get DXGI device
	IDXGIDevice *lDxgiDevice = 0;
	hr = _lDevice->QueryInterface(__uuidof(IDXGIDevice), (void**) & lDxgiDevice);

	if (FAILED(hr))
	{
		if (lDxgiDevice != nullptr) lDxgiDevice->Release();
		return false;
	}

	// Get DXGI adapter
	IDXGIAdapter * lDxgiAdapter;
	hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&lDxgiAdapter);

	if (FAILED(hr))
	{
		if (lDxgiAdapter != nullptr) lDxgiAdapter->Release();
		return false;
	}

	lDxgiDevice->Release();

	UINT Output = 0;

	// Get output
	IDXGIOutput *lDxgiOutput;
	hr = lDxgiAdapter->EnumOutputs(Output, &lDxgiOutput);

	if (FAILED(hr))
	{
		if (lDxgiOutput != nullptr) lDxgiOutput->Release();
		return false;
	}

	lDxgiAdapter->Release();

	hr = lDxgiOutput->GetDesc(&_lOutputDesc);

	if (FAILED(hr))
	{
		return false;
	}

	// QI for Output 1
	IDXGIOutput1 *lDxgiOutput1;
	hr = lDxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&lDxgiOutput1);

	if (FAILED(hr))
	{
		if (lDxgiOutput1 != nullptr) lDxgiOutput1->Release();
		return false;
	}

	lDxgiOutput->Release();

	// Create desktop duplication
	hr = lDxgiOutput1->DuplicateOutput(_lDevice, &_lDeskDupl);

	if (FAILED(hr))
	{
		if (_lDeskDupl != nullptr) _lDeskDupl->Release();
		return false;
	}

	lDxgiOutput1->Release();

	// Create GUI drawing texture
	_lDeskDupl->GetDesc(&_lOutputDuplDesc);
	// Create CPU access texture
	_desc.Width = _lOutputDuplDesc.ModeDesc.Width;
	_desc.Height = _lOutputDuplDesc.ModeDesc.Height;
	_desc.Format = _lOutputDuplDesc.ModeDesc.Format;
	std::cout << _desc.Width << "x" << _desc.Height << "\n\n\n";

	_desc.ArraySize = 1;
	_desc.BindFlags = 0;
	_desc.MiscFlags = 0;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.MipLevels = 1;
	_desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
	_desc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;

	return true;
}

bool DX11::init()
{
	int lresult(-1);

	D3D_FEATURE_LEVEL lFeatureLevel;

	HRESULT hr(E_FAIL);
	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		gFeatureLevels,
		gNumFeatureLevels,
		D3D11_SDK_VERSION,
		&_lDevice,
		&lFeatureLevel,
		&_lImmediateContext);

	if (FAILED(hr))
		return false;

	if (!_lDevice)
		return false;

	// Get DXGI device
	IDXGIDevice *lDxgiDevice;
	hr = _lDevice->QueryInterface(__uuidof(IDXGIAdapter), (void**)&lDxgiDevice);

	if (FAILED(hr))
	{
		if (lDxgiDevice != nullptr) lDxgiDevice->Release();
		return false;
	}

	// Get DXGI adapter
	IDXGIAdapter *lDxgiAdapter;
	hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&lDxgiAdapter);

	if (FAILED(hr))
	{
		if (lDxgiAdapter != nullptr) lDxgiAdapter->Release();
		return false;
	}

	lDxgiDevice->Release();

	UINT Output = 0;

	// Get output
	IDXGIOutput *lDxgiOutput;
	hr = lDxgiAdapter->EnumOutputs(Output, &lDxgiOutput);

	if (FAILED(hr))
	{
		if (lDxgiOutput != nullptr) lDxgiOutput->Release();
		return false;
	}

	lDxgiAdapter->Release();

	hr = lDxgiOutput->GetDesc(&_lOutputDesc);

	if (FAILED(hr))
		return false;

	// QI for Output 1
	IDXGIOutput1 *lDxgiOutput1;
	hr = lDxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&lDxgiOutput1);

	if (FAILED(hr))
	{
		if (lDxgiOutput1 != nullptr) lDxgiOutput1->Release();
		return false;
	}

	lDxgiOutput->Release();

	// Create desktop duplication
	hr = lDxgiOutput1->DuplicateOutput(_lDevice, &_lDeskDupl);

	if (FAILED(hr))
	{
		if (_lDeskDupl != nullptr) _lDeskDupl->Release();
		return false;
	}

	lDxgiOutput1->Release();

	// Create CPU access texture
	_lDeskDupl->GetDesc(&_lOutputDuplDesc);
	_desc.Width = _lOutputDuplDesc.ModeDesc.Width;
	_desc.Height = _lOutputDuplDesc.ModeDesc.Height;
	_desc.Format = _lOutputDuplDesc.ModeDesc.Format;
	_desc.ArraySize = 1;
	_desc.BindFlags = 0;
	_desc.MiscFlags = 0;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.MipLevels = 1;
	_desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
	_desc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;
	
	return true;
}


bool DX11::captureScreen(ParsecDSO *ps)
{
	if (!_lDeskDupl)
	{
		if (!recover())
		{
			return false;
		}
	}
	
	HRESULT hr(E_FAIL), hr0(E_FAIL);
	IDXGIResource *lDesktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO lFrameInfo;
	ID3D11Texture2D* currTexture = NULL;

	hr = _lDeskDupl->AcquireNextFrame(1, &lFrameInfo, &lDesktopResource);
	if (FAILED(hr)) {
		_lDeskDupl->ReleaseFrame();

		if (hr != DXGI_ERROR_WAIT_TIMEOUT)
		{
			recover();
		}

		return false;
	}

	// QI for ID3D11Texture2D
	hr = lDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&_lAcquiredDesktopImage);

	//////////////////////////////////////// 
	// LEL
	//////////////////////////////////////// 
	ParsecHostD3D11SubmitFrame(ps, 0, _lDevice, _lImmediateContext, _lAcquiredDesktopImage);
	
	
	_lDeskDupl->ReleaseFrame();
	

	return true;
}