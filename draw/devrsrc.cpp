#include "draw_pch.h"
#include "devrsrc.h"

using namespace DirectX;
using namespace D2D1;
using namespace DX;
using Microsoft::WRL::ComPtr;

DeviceResources::DeviceResources(XMUINT2 backBufSize, UINT backBufCount, DXGI_FORMAT backBufFormat, DXGI_FORMAT depthBufFormat, D3D_FEATURE_LEVEL d3dMinFeatureLevel)
noexcept : 
	m_backBufSize{ backBufSize },
	m_backBufCount{ backBufCount },
	m_backBufFormat{ backBufFormat },
	m_depthBufFormat{ depthBufFormat },
	m_d3dDevice{ nullptr },
	m_d3dContext{ nullptr },
	m_d2dDevice{ nullptr },
	m_d2dContext{ nullptr },
	m_d2dFactory{ nullptr },
	m_d2dBitmap{ nullptr },
	m_d3dMinFeatureLevel{ d3dMinFeatureLevel },
	m_d3dFeatureLevel{ D3D_FEATURE_LEVEL_9_1 },
	m_hWnd{ nullptr },
	m_deviceNotify{ nullptr }
{ }

DeviceResources::~DeviceResources()
{
	m_swapChain.Reset();
	m_d2dBitmap.Reset();
	m_d2dContext.Reset();
	m_d2dDevice.Reset();
	m_d3dContext.Reset();
	m_d3dDevice.Reset();
	m_d2dFactory.Reset();
}

void DeviceResources::SetWindow(HWND hWnd)
{
	m_hWnd = hWnd;
}

void DeviceResources::cdir()
{
	D2D1_FACTORY_OPTIONS opts = {};

#if defined(_DEBUG)
	opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, opts, m_d2dFactory.GetAddressOf()));
}

void DeviceResources::cddr()
{
	static const D3D_FEATURE_LEVEL d3dFeatureLevelTable[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	UINT d3dFeatureLevelCount = ARRAYSIZE(d3dFeatureLevelTable);
	for (int i = 0; i < _countof(d3dFeatureLevelTable); ++i)
	{
		if (d3dFeatureLevelTable[i] < m_d3dMinFeatureLevel)
		{
			if (i == 0)
				throw std::out_of_range("Minimum Feature Level too high.");
			d3dFeatureLevelCount = i;
			break;
		}
	}

	UINT d3dCreationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	d3dCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	if (FAILED(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		d3dCreationFlags,
		d3dFeatureLevelTable,
		d3dFeatureLevelCount,
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(),
		&m_d3dFeatureLevel,
		context.ReleaseAndGetAddressOf())
	))
	{
		ThrowIfFailed(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_WARP,
			nullptr,
			d3dCreationFlags,
			d3dFeatureLevelTable,
			d3dFeatureLevelCount,
			D3D11_SDK_VERSION,
			device.ReleaseAndGetAddressOf(),
			&m_d3dFeatureLevel,
			context.ReleaseAndGetAddressOf())
		);
		OutputDebugStringA("Direct3D adapter: WARP.\n");
	}

	ThrowIfFailed(device.As(&m_d3dDevice));
	ThrowIfFailed(context.As(&m_d3dContext));

	// use a DXGI dvice as a bridge to get to a D2D device from a D3D device

	ComPtr<IDXGIDevice4> dxgiDevice;
	ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));
	ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), m_d2dDevice.ReleaseAndGetAddressOf()));
	ThrowIfFailed(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_d2dContext.ReleaseAndGetAddressOf()));
}

void DeviceResources::cwsdr()
{
	// Set the render target to a D2D bitmap.

	ComPtr<IDXGISurface> dxgiSurface;
	ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(dxgiSurface.GetAddressOf())));

	auto bitmapProperties = BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		PixelFormat(m_backBufFormat, D2D1_ALPHA_MODE_IGNORE));

	ThrowIfFailed(m_d2dContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), bitmapProperties, m_d2dBitmap.GetAddressOf()));

	m_d2dContext->SetTarget(m_d2dBitmap.Get());
}

void DeviceResources::CreateSwapChain()
{
	ComPtr<IDXGIDevice> dxgiDevice;
	ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

	ComPtr<IDXGIAdapter> dxgiAdapter;
	ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

	ComPtr<IDXGIFactory> dxgiFactory;
	ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

	ComPtr<IDXGIFactory2> dxgiFactory2;
	ThrowIfFailed(dxgiFactory.As(&dxgiFactory2));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = m_backBufSize.x;
	swapChainDesc.Height = m_backBufSize.y;
	swapChainDesc.Format = m_backBufFormat;
	swapChainDesc.BufferCount = m_backBufCount;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), m_hWnd, &swapChainDesc, nullptr, nullptr, m_swapChain.ReleaseAndGetAddressOf()));
}

void DeviceResources::ModifySwapChain()
{
	m_d2dBitmap.Reset();
	m_d2dContext->SetTarget(nullptr);

	HRESULT hr = m_swapChain->ResizeBuffers(m_backBufCount, m_backBufSize.x, m_backBufSize.y, m_backBufFormat, 0);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buf[64] = {};
		sprintf_s(buf, "Device lost on ResizeBuffers(): Reason code 0x%08X\n",
			(hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
		OutputDebugStringA(buf);
#endif // _DEBUG
		HandleLoss();
	}
	else
		ThrowIfFailed(hr);
}

bool DeviceResources::HandleResize(size_t w, size_t h)
{
	if (!m_hWnd)
		throw std::exception("Missing Win32 window handle.");

	// Do nothing if the size hasn't changed.
	if (m_backBufSize.x == w && m_backBufSize.y == h)
		return false;

	m_backBufSize.x = std::max<int32_t>(w, 1);
	m_backBufSize.y = std::max<int32_t>(h, 1);

	if (m_swapChain)
		ModifySwapChain();
	else
		CreateSwapChain();

	cwsdr();

	return true;
}

void DeviceResources::HandleLoss()
{
	m_swapChain.Reset();
	m_d2dBitmap.Reset();
	m_d2dContext.Reset();
	m_d2dDevice.Reset();
	m_d3dContext.Reset();

#ifdef _DEBUG
	{
		ComPtr<ID3D11Debug> d3dDebug;
		if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
			d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
	}
#endif

	m_d3dDevice.Reset();

	cddr();
	CreateSwapChain();
	cwsdr();
}

void DeviceResources::Present()
{
	HRESULT hr = m_swapChain->Present(1, 0);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buf[64] = {};
		sprintf_s(buf, "Device lost on Present(): Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
		OutputDebugStringA(buf);
#endif
		HandleLoss();
	}
	else
		ThrowIfFailed(hr);
}
