#include "DX11.h"

using namespace Microsoft::WRL;

D3D_FEATURE_LEVEL gFeatureLevels[] = {
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1,
};
UINT gNumFeatureLevels = 6;
ComPtr<ID3D11Device> _lDevice;
ID3D11DeviceContext *_lImmediateContext;
ComPtr<IDXGIOutputDuplication> _lDeskDupl;
DXGI_OUTPUT_DESC _lOutputDesc;
DXGI_OUTDUPL_DESC _lOutputDuplDesc;
ComPtr<ID3D11Texture2D> _lAcquiredDesktopImage;
D3D11_MAPPED_SUBRESOURCE _resource;


ID3D11Device * DX11::getDevice()
{
	return _lDevice.Get();
}

ID3D11DeviceContext * DX11::getDeviceContext()
{
	return _lImmediateContext;
}

void DX11::clear()
{
	if (_lDevice) _lDevice.ReleaseAndGetAddressOf();
	if (d3Device) d3Device->Release();
	if (d3DeviceContext) d3DeviceContext->Release();
	if (_lDeskDupl) _lDeskDupl.ReleaseAndGetAddressOf();
	if (_lAcquiredDesktopImage) _lAcquiredDesktopImage.ReleaseAndGetAddressOf();
}

bool DX11::recover()
{
	HRESULT hr;

	// Get DXGI device
	ComPtr<IDXGIDevice> lDxgiDevice;
	hr = _lDevice.As(&lDxgiDevice);

	if (FAILED(hr))
		return false;

	// Get DXGI adapter
	ComPtr<IDXGIAdapter> lDxgiAdapter;
	hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter), &lDxgiAdapter);

	if (FAILED(hr))
		return false;

	lDxgiDevice.Reset();

	UINT Output = 0;

	// Get output
	ComPtr<IDXGIOutput> lDxgiOutput;
	hr = lDxgiAdapter->EnumOutputs(Output, &lDxgiOutput);

	if (FAILED(hr))
		return false;

	lDxgiAdapter.Reset();

	hr = lDxgiOutput->GetDesc(&_lOutputDesc);

	if (FAILED(hr))
		return false;

	// QI for Output 1
	ComPtr<IDXGIOutput1> lDxgiOutput1;
	hr = lDxgiOutput.As(&lDxgiOutput1);

	if (FAILED(hr))
		return false;

	lDxgiOutput.Reset();

	// Create desktop duplication
	hr = lDxgiOutput1->DuplicateOutput(_lDevice.Get(), &_lDeskDupl);

	if (FAILED(hr))
		return false;

	lDxgiOutput1.Reset();

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
	ComPtr<IDXGIDevice> lDxgiDevice;
	hr = _lDevice.As(&lDxgiDevice);

	if (FAILED(hr))
		return false;

	// Get DXGI adapter
	ComPtr<IDXGIAdapter> lDxgiAdapter;
	hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter), &lDxgiAdapter);

	if (FAILED(hr))
		return false;

	lDxgiDevice.Reset();

	UINT Output = 0;

	// Get output
	ComPtr<IDXGIOutput> lDxgiOutput;
	hr = lDxgiAdapter->EnumOutputs(Output, &lDxgiOutput);

	if (FAILED(hr))
		return false;

	lDxgiAdapter.Reset();

	hr = lDxgiOutput->GetDesc(&_lOutputDesc);

	if (FAILED(hr))
		return false;

	// QI for Output 1
	ComPtr<IDXGIOutput1> lDxgiOutput1;
	hr = lDxgiOutput.As(&lDxgiOutput1);

	if (FAILED(hr))
		return false;

	lDxgiOutput.Reset();

	// Create desktop duplication
	hr = lDxgiOutput1->DuplicateOutput(_lDevice.Get(), &_lDeskDupl);

	if (FAILED(hr))
		return false;

	lDxgiOutput1.Reset();

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

bool DX11::captureScreen(ParsecDSO *ps)
{
	if (!_lDeskDupl)
	{
		if(!recover()) return false;
	}

	HRESULT hr(E_FAIL), hr0(E_FAIL);
	ComPtr<IDXGIResource> lDesktopResource = nullptr;
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
	hr = lDesktopResource.As(&_lAcquiredDesktopImage);

	//////////////////////////////////////// 
	// LEL
	//////////////////////////////////////// 
	ParsecHostD3D11SubmitFrame(ps, 0, getDevice(), getDeviceContext(), _lAcquiredDesktopImage.Get());
	
	
	_lDeskDupl->ReleaseFrame();
	

	return true;
}












////////////////////////////////////////////////////////////////
//
// OLD
//
////////////////////////////////////////////////////////////////

//HRESULT DX11::init()
//{
//	HRESULT hr;
//	
//	// Windows
//	hwnd = GetDesktopWindow();
//	
//	// D3D11
//	dxSwapDesc = { 0 };
//	dxSwapDesc.BufferDesc.RefreshRate.Numerator = 60;
//	dxSwapDesc.BufferDesc.RefreshRate.Denominator = 1;
//	dxSwapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//	dxSwapDesc.SampleDesc.Count = 1;
//	dxSwapDesc.SampleDesc.Quality = 0;
//	dxSwapDesc.BufferUsage = DXGI_USAGE_READ_ONLY;
//	dxSwapDesc.BufferCount = 1;
//	dxSwapDesc.OutputWindow = hwnd;
//	dxSwapDesc.Windowed = true;
//
//	D3D_FEATURE_LEVEL featureLevel;
//	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG;
//
//	hr = D3D11CreateDeviceAndSwapChain(
//		NULL,
//		D3D_DRIVER_TYPE_HARDWARE,
//		NULL,
//		flags,
//		NULL,
//		0,
//		D3D11_SDK_VERSION,
//		&dxSwapDesc,
//		&dxSwapChain,
//		&d3Device,
//		&featureLevel,
//		&d3DeviceContext
//	);
//	if (FAILED(hr)) { return hr; }
//
//	// DXGI
//	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxFactory);
//	if (FAILED(hr)) { return hr; }
//
//	hr = dxFactory->EnumAdapters(0, &dxAdapter);
//	if (FAILED(hr)) { return hr; }
//
//	hr = dxAdapter->EnumOutputs(0, &dxOutput);
//	if (FAILED(hr)) { return hr; }
//
//	dxOutput->GetDesc(&dxOutDesc);
//
//	memset(&d3TexDesc, 0, sizeof(d3TexDesc));
//	d3TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
//	d3TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	d3TexDesc.BindFlags = 0;
//	d3TexDesc.Width = dxOutDesc.DesktopCoordinates.right - dxOutDesc.DesktopCoordinates.left;
//	d3TexDesc.Height = dxOutDesc.DesktopCoordinates.bottom - dxOutDesc.DesktopCoordinates.top;
//	d3TexDesc.MipLevels = 1;
//	d3TexDesc.SampleDesc = { 1, 0 };
//	d3TexDesc.Usage = D3D11_USAGE_STAGING;
//	d3TexDesc.ArraySize = 1;
//
//	return hr;
//}
//
//HRESULT DX11::screenCapture()
//{
//	if (d3Frame != NULL)
//	{
//		d3Frame->Release();
//	}
//
//	HRESULT hr;
//
//	hr = d3Device->CreateTexture2D(&d3TexDesc, 0, &d3Frame);
//	if (FAILED(hr)) { return hr; }
//
//	// Get IDXGIResource from texture.
//	IDXGIResource *destResource = 0;
//	hr = d3Frame->QueryInterface(IID_PPV_ARGS(&destResource));
//	if (FAILED(hr)) { return hr; }
//
//	// Get data.
//	IDXGIOutput1 *output1;
//	hr = dxOutput->QueryInterface(IID_PPV_ARGS(&output1));
//	if (FAILED(hr)) { return hr; }
//
//	output1->TakeOwnership(d3Device, TRUE);
//	hr = output1->GetDisplaySurfaceData1(destResource);
//	output1->ReleaseOwnership();
//
//	const bool fail = FAILED(hr);
//
//	return hr;
//}


/*

bool initDx11()
{
	HWND hwnd = GetDesktopWindow();
	//HWND hwnd = CreateWindowA("STATIC", "dummy", WS_VISIBLE, 0, 0, 800, 600, NULL, NULL, NULL, NULL);
	//::SetWindowTextA(hwnd, "DXD11Window");
	//ShowWindow(hwnd, SW_SHOW);

	DXGI_SWAP_CHAIN_DESC descr = { 0 };
	descr.BufferDesc.RefreshRate.Numerator = 0;
	descr.BufferDesc.RefreshRate.Denominator = 1;
	descr.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	descr.SampleDesc.Count = 1;
	descr.SampleDesc.Quality = 0;
	descr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descr.BufferCount = 1;
	descr.OutputWindow = hwnd;
	descr.Windowed = true;

	D3D_FEATURE_LEVEL featureLevel;
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&descr,
		&g_SwapChain,
		&g_d3d11Device,
		&featureLevel,
		&g_d3d11DevCon
	);

	return SUCCEEDED(hr);
}


void screenCapture()
{
	if (g_frame != NULL)
	{
		g_frame->Release();
	}

	HRESULT hr;

	IDXGIFactory *factory;
	IDXGIAdapter *adapter;
	IDXGIOutput *output;
	//DXGI_MODE_DESC *displayModeList;
	//DXGI_ADAPTER_DESC adapterDesc;
	//D3D11_TEXTURE2D_DESC depthBufferDesc;

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr)) { return ; }

	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr)) { return; }

	hr = adapter->EnumOutputs(0, &output);
	if (FAILED(hr)) { return; }

	DXGI_OUTPUT_DESC outDesc;
	output->GetDesc(&outDesc);

	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.BindFlags = 0;
	texDesc.Width = outDesc.DesktopCoordinates.right - outDesc.DesktopCoordinates.left;
	texDesc.Height = outDesc.DesktopCoordinates.bottom - outDesc.DesktopCoordinates.top;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc = { 1, 0 };
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.ArraySize = 1;

	hr = g_d3d11Device->CreateTexture2D(&texDesc, 0, &g_frame);
	if (FAILED(hr)) { return; }

	IDXGIResource *destResource = 0;
	hr = g_frame->QueryInterface(IID_PPV_ARGS(&destResource));
	if (FAILED(hr)) { return; }

	IDXGIOutput1 *output1;
	hr = output->QueryInterface(IID_PPV_ARGS(&output1));
	if (FAILED(hr)) { return; }

	output1->TakeOwnership(g_d3d11Device, TRUE);
	hr = output1->GetDisplaySurfaceData1(destResource);

	const bool fail = FAILED(hr);
	if (FAILED(hr)) {
		//return;
	}

	output1->ReleaseOwnership();


	////ID3D11Texture2D *frame = NULL;
	//ID3D11Texture2D *pBackBuffer = NULL;
	//hr = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	//D3D11_TEXTURE2D_DESC resolveDesc;
	//pBackBuffer->GetDesc(&resolveDesc);
	//resolveDesc.Usage = D3D11_USAGE_STAGING;
	//resolveDesc.BindFlags = 0;
	//resolveDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	//resolveDesc.SampleDesc.Count = 1;    //resolved buffer is not multisampled
	//resolveDesc.SampleDesc.Quality = 0;

	//hr = g_d3d11Device->CreateTexture2D(&resolveDesc, NULL, &g_frame);
	//g_d3d11DevCon->ResolveSubresource(g_frame, D3D11CalcSubresource(0, 0, 1), pBackBuffer, D3D11CalcSubresource(0, 0, 1), resolveDesc.Format);



	////hr = D3DX11SaveTextureToFileA(g_d3d11DevCon, pResolveBuffer, D3DX11_IFF_PNG, filename);

	//pBackBuffer->Release();

}
*/