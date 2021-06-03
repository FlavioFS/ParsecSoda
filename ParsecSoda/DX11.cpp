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

bool DX11::captureScreen(Parsec *ps)
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