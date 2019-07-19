#include "world_pch.h"
#include "devrsrc.h"

using namespace DirectX;
using namespace DX;

using Microsoft::WRL::ComPtr;

namespace
{
#if defined(_DEBUG)
	inline bool SDKLayersAvailable()
	{	// Check for SDK layer support.
		HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_NULL, 0, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, nullptr, nullptr, nullptr);
		return SUCCEEDED(hr);
	}
#endif // _DEBUG
};

DeviceResources::DeviceResources(XMUINT2 backBufSize, DXGI_FORMAT bbufFormat, DXGI_FORMAT depthBufferFormat, UINT bbufCount, D3D_FEATURE_LEVEL minFeatureLevel)
noexcept :
	m_backBufSize{ backBufSize }, 
	m_bbufFormat{ bbufFormat },
	m_depthBufferFormat{depthBufferFormat},
	m_bbufCount{bbufCount},
	m_d3dMinFeatureLevel(minFeatureLevel),
	m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	m_screenViewport{},
	m_hWnd(nullptr),
	m_outputRect{ 0, 0, static_cast<LONG>(backBufSize.x), static_cast<LONG>(backBufSize.y) },
	m_deviceNotify(nullptr)
{ }

void DeviceResources::cdir() {}

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
	if (SDKLayersAvailable())
		d3dCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	else
		OutputDebugStringA("WARNING: Direct3D Debug device is not available.\n");
#endif
	{
		bool warp = false;
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<IDXGIAdapter1> adapter;
		GetHardwareAdapter(adapter.GetAddressOf());
		if (adapter)
		{
			if (FAILED(D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0, d3dCreationFlags, d3dFeatureLevelTable, d3dFeatureLevelCount, D3D11_SDK_VERSION, device.ReleaseAndGetAddressOf(), &m_d3dFeatureLevel, context.ReleaseAndGetAddressOf())))
				warp = true; // the adapter exists but somehow the device creation fails
		}
		else
		{
			warp = true; // we don't have 3D hardware on this de
			OutputDebugStringA("No Direct3D hardware device found; falling back to a software renderer.\n");
#if defined(NDEBUG)
			throw std::exception("No Direct3D hardware device found.")
#endif
		}
		if (warp)
		{
			// if that fails we're at a point of no return; throw an exception
			ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, 0, d3dCreationFlags, d3dFeatureLevelTable, d3dFeatureLevelCount, D3D11_SDK_VERSION, device.ReleaseAndGetAddressOf(), &m_d3dFeatureLevel, context.ReleaseAndGetAddressOf()));
			OutputDebugStringA("Direct3D adapter: WARP.\n");
		}
		ThrowIfFailed(device.As(&m_d3dDevice));
		ThrowIfFailed(context.As(&m_d3dContext));
		ThrowIfFailed(m_d3dContext.As(&m_d3dAnnotation));
#ifndef NDEBUG
		ComPtr<ID3D11Debug> d3dDebug;
		if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
		{
			ComPtr<ID3D11InfoQueue> d3dInfoQueue;
			if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
			{
#ifdef _DEBUG
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif // _DEBUG
				D3D11_MESSAGE_ID hide[] = { D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS };
				D3D11_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.NumIDs = _countof(hide);
				filter.DenyList.pIDList = hide;
				d3dInfoQueue->AddStorageFilterEntries(&filter);
			}
		}
#endif // NDEBUG
	}
}

void DeviceResources::cwsdr()
{
	if (!m_hWnd)
		throw std::exception("Missing window handle; did you call SetWindow at the right place with a valid Win32 window handle?");
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_d3dRenderTargetView.Reset();
	m_d3dDepthStencilView.Reset();
	m_renderTarget.Reset();
	m_depthStencil.Reset();
	m_d3dContext->Flush();
	if (!m_swapChain)
		CreateSwapChain();
	else
		ModifySwapChain();
	ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(m_renderTarget.ReleaseAndGetAddressOf())));
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS, m_bbufFormat);
	ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(m_renderTarget.Get(), &renderTargetViewDesc, m_d3dRenderTargetView.ReleaseAndGetAddressOf()));
	if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
	{
		CD3D11_TEXTURE2D_DESC depthStencilDesc(m_depthBufferFormat, m_backBufSize.x, m_backBufSize.y, 1, 1, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0, 8, 0);
		ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, m_depthStencil.ReleaseAndGetAddressOf()));
		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);
		ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &depthStencilViewDesc, m_d3dDepthStencilView.ReleaseAndGetAddressOf()));
	}
	m_screenViewport = CD3D11_VIEWPORT(0.f, 0.f, static_cast<float>(m_backBufSize.x), static_cast<float>(m_backBufSize.y));
}

void DeviceResources::CreateSwapChain()
{
	assert(m_hWnd);
	assert(m_backBufSize.x > 0);
	assert(m_backBufSize.y > 0);
	ComPtr<IDXGIDevice> dxgiDevice; ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));
	ComPtr<IDXGIAdapter> dxgiAdapter; ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));
	ComPtr<IDXGIFactory> dxgiFactory; ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));
	ComPtr<IDXGIFactory2> dxgiFactory2; ThrowIfFailed(dxgiFactory.As(&dxgiFactory2));
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = m_backBufSize.x;
	swapChainDesc.Height = m_backBufSize.y;
	swapChainDesc.Format = m_bbufFormat;
	swapChainDesc.BufferCount = m_bbufCount;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.SampleDesc.Count = 8;
	swapChainDesc.SampleDesc.Quality = 0;
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
	fsSwapChainDesc.Windowed = TRUE;
	ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), m_hWnd, &swapChainDesc, &fsSwapChainDesc, nullptr, m_swapChain.ReleaseAndGetAddressOf()));
	ThrowIfFailed(dxgiFactory2->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));
}

void DeviceResources::ModifySwapChain()
{
	assert(m_hWnd);
	assert(m_backBufSize.x > 0);
	assert(m_backBufSize.y > 0);
	m_d3dRenderTargetView.Reset(); 	// release all outstanding buffer references before resizing the swap chain; the render target view
	m_renderTarget.Reset();			// release the render target
	auto hr = m_swapChain->ResizeBuffers(m_bbufCount, m_backBufSize.x, m_backBufSize.y, m_bbufFormat, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buf[64] = {};
		sprintf_s(buf, "Device lost on ModifySwapChain(): reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
		OutputDebugStringA(buf);
#endif
		HandleLoss();
	}
	else
		ThrowIfFailed(hr);
}

bool DeviceResources::HandleResize(size_t w, size_t h)
{
	if (m_backBufSize.x == w && m_backBufSize.y == h)
		return false; // don't do anything if the size hasn't changed
	if (!m_swapChain)
		CreateSwapChain();
	else
		ModifySwapChain();
	cwsdr();
	return true;
}

void DeviceResources::HandleLoss()
{
	if (m_deviceNotify)
		m_deviceNotify->OnDeviceLoss();
	m_d3dRenderTargetView.Reset();
	m_d3dDepthStencilView.Reset();
	m_swapChain.Reset();
	m_d3dContext.Reset();
	m_d3dAnnotation.Reset();
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
	if (m_deviceNotify)
		m_deviceNotify->OnDeviceRestore();
}

void DeviceResources::SetWindow(HWND hWnd)
{
	m_hWnd = hWnd;
}

void DeviceResources::Present()
{
	auto hr = m_swapChain->Present(1, 0);
	// NOTE: If dirty or scroll rects are used, don't discard the render target.
	if (m_d3dContext) m_d3dContext->DiscardView(m_d3dRenderTargetView.Get());
	if (m_d3dDepthStencilView) m_d3dContext->DiscardView(m_d3dDepthStencilView.Get());
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buf[64] = {};
		sprintf_s(buf, "Device lost on Present(): reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
		OutputDebugStringA(buf);
#endif
		HandleLoss();
	}
	else
		ThrowIfFailed(hr);
}

void DeviceResources::GetHardwareAdapter(IDXGIAdapter1** ppAdapter)
{
	*ppAdapter = nullptr;
	ComPtr<IDXGIFactory1> dxgiFactory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));
	ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0;
		DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(adapterIndex, adapter.ReleaseAndGetAddressOf());
		++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;
#ifdef _DEBUG
		wchar_t buf[256] = {};
		swprintf_s(buf, L"Direct3D adapter (%u): VID:%04X, PID:%04X - %ls.\n",
			adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
		OutputDebugStringW(buf);
#endif
		break;
	}

	*ppAdapter = adapter.Detach();
}
