#include "world_pch.h"
#include "devrsrc.h"

using namespace DirectX;
using namespace DX;

using Microsoft::WRL::ComPtr;

namespace
{
#if defined(_DEBUG)
	inline bool SDKLayersAvailable() // Check for SDK layer support.
	{	
		HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_NULL, 0, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, nullptr, nullptr, nullptr);
		return SUCCEEDED(hr);
	}
#endif // _DEBUG
};

DeviceResources::DeviceResources(XMUINT2 back_buffer_size, DXGI_FORMAT back_buffer_format, DXGI_FORMAT depth_buffer_format, UINT back_buffer_count, D3D_FEATURE_LEVEL min_feature_level)
noexcept :
	back_buffer_size_{ back_buffer_size }, 
	back_buffer_format_{ back_buffer_format },
	depth_buffer_format_{ depth_buffer_format },
	back_buffer_count_{ back_buffer_count },
	d3d_min_feature_level_( min_feature_level ),
	d3d_feature_level_(D3D_FEATURE_LEVEL_9_1),
	screen_viewport_{},
	hwnd_(nullptr),
	output_rect_{ 0, 0, static_cast<LONG>(back_buffer_size.x), static_cast<LONG>(back_buffer_size.y) },
	device_notify_(nullptr)
{ }

void DeviceResources::cdir() {}

void DeviceResources::cddr()
{
	static const D3D_FEATURE_LEVEL d3d_feature_level_table[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT d3d_feature_level_count = ARRAYSIZE(d3d_feature_level_table);
	for (int i = 0; i < _countof(d3d_feature_level_table); ++i)
	{
		if (d3d_feature_level_table[i] < d3d_min_feature_level_)
		{
			if (i == 0)
				throw std::out_of_range("Minimum Feature Level too high.");
			d3d_feature_level_count = i;
			break;
		}
	}
	UINT d3d_creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	if (SDKLayersAvailable())
		d3d_creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
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
			if (FAILED(D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0, d3d_creation_flags, d3d_feature_level_table, d3d_feature_level_count, D3D11_SDK_VERSION, device.ReleaseAndGetAddressOf(), &d3d_feature_level_, context.ReleaseAndGetAddressOf())))
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
			ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, 0, d3d_creation_flags, d3d_feature_level_table, d3d_feature_level_count, D3D11_SDK_VERSION, device.ReleaseAndGetAddressOf(), &d3d_feature_level_, context.ReleaseAndGetAddressOf()));
			OutputDebugStringA("Direct3D adapter: WARP.\n");
		}
		ThrowIfFailed(device.As(&d3d_device_));
		ThrowIfFailed(context.As(&d3d_context_));
		ThrowIfFailed(d3d_context_.As(&d3d_annotation_));
#ifndef NDEBUG
		ComPtr<ID3D11Debug> d3d_debug;
		if (SUCCEEDED(d3d_device_.As(&d3d_debug)))
		{
			ComPtr<ID3D11InfoQueue> d3dInfoQueue;
			if (SUCCEEDED(d3d_debug.As(&d3dInfoQueue)))
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
	if (!hwnd_)
		throw std::exception("Missing window handle; did you call set_window at the right place with a valid Win32 window handle?");
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	d3d_context_->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	render_target_view_.Reset();
	depth_stencil_view_.Reset();
	render_target_.Reset();
	depth_stencil_.Reset();
	d3d_context_->Flush();
	if (!swap_chain_)
		create_swap_chain();
	else
		modify_swap_chain();
	ThrowIfFailed(swap_chain_->GetBuffer(0, IID_PPV_ARGS(render_target_.ReleaseAndGetAddressOf())));
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS, back_buffer_format_);
	ThrowIfFailed(d3d_device_->CreateRenderTargetView(render_target_.Get(), &renderTargetViewDesc, render_target_view_.ReleaseAndGetAddressOf()));
	if (depth_buffer_format_ != DXGI_FORMAT_UNKNOWN)
	{
		CD3D11_TEXTURE2D_DESC texture_descriptor(depth_buffer_format_, back_buffer_size_.x, back_buffer_size_.y, 1, 1, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0, 8, 0);
		ThrowIfFailed(d3d_device_->CreateTexture2D(&texture_descriptor, nullptr, depth_stencil_.ReleaseAndGetAddressOf()));
		CD3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_descriptor(D3D11_DSV_DIMENSION_TEXTURE2DMS);
		ThrowIfFailed(d3d_device_->CreateDepthStencilView(depth_stencil_.Get(), &depth_stencil_view_descriptor, depth_stencil_view_.ReleaseAndGetAddressOf()));
	}
	screen_viewport_ = CD3D11_VIEWPORT(0.f, 0.f, static_cast<float>(back_buffer_size_.x), static_cast<float>(back_buffer_size_.y));
}

void DeviceResources::create_swap_chain()
{
	assert(hwnd_);
	assert(back_buffer_size_.x > 0);
	assert(back_buffer_size_.y > 0);
	ComPtr<IDXGIDevice> dxgi_device; ThrowIfFailed(d3d_device_.As(&dxgi_device));
	ComPtr<IDXGIAdapter> dxgi_adapter; ThrowIfFailed(dxgi_device->GetAdapter(dxgi_adapter.GetAddressOf()));
	ComPtr<IDXGIFactory> dxgi_factory; ThrowIfFailed(dxgi_adapter->GetParent(IID_PPV_ARGS(dxgi_factory.GetAddressOf())));
	ComPtr<IDXGIFactory2> dxgi_factory2; ThrowIfFailed(dxgi_factory.As(&dxgi_factory2));
	DXGI_SWAP_CHAIN_DESC1 swap_chain_descriptor = {};
	swap_chain_descriptor.Width = back_buffer_size_.x;
	swap_chain_descriptor.Height = back_buffer_size_.y;
	swap_chain_descriptor.Format = back_buffer_format_;
	swap_chain_descriptor.BufferCount = back_buffer_count_;
	swap_chain_descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_descriptor.Scaling = DXGI_SCALING_STRETCH;
	swap_chain_descriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_descriptor.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swap_chain_descriptor.SampleDesc.Count = 8;
	swap_chain_descriptor.SampleDesc.Quality = 0;
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_descriptor = {};
	swap_chain_fullscreen_descriptor.Windowed = TRUE;
	ThrowIfFailed(dxgi_factory2->CreateSwapChainForHwnd(d3d_device_.Get(), hwnd_, &swap_chain_descriptor, &swap_chain_fullscreen_descriptor, nullptr, swap_chain_.ReleaseAndGetAddressOf()));
	ThrowIfFailed(dxgi_factory2->MakeWindowAssociation(hwnd_, DXGI_MWA_NO_ALT_ENTER));
}

void DeviceResources::modify_swap_chain()
{
	assert(hwnd_);
	assert(back_buffer_size_.x > 0);
	assert(back_buffer_size_.y > 0);
	render_target_view_.Reset(); 	// release all outstanding buffer references before resizing the swap chain; the render target view
	render_target_.Reset();			// release the render target
	auto hr = swap_chain_->ResizeBuffers(back_buffer_count_, back_buffer_size_.x, back_buffer_size_.y, back_buffer_format_, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buf[64] = {};
		sprintf_s(buf, "Device lost on modify_swap_chain(): reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? d3d_device_->GetDeviceRemovedReason() : hr);
		OutputDebugStringA(buf);
#endif
		handle_loss();
	}
	else
		ThrowIfFailed(hr);
}

bool DeviceResources::handle_resize(size_t w, size_t h)
{
	if (back_buffer_size_.x == w && back_buffer_size_.y == h)
		return false; // don't do anything if the size hasn't changed
	if (!swap_chain_)
		create_swap_chain();
	else
		modify_swap_chain();
	cwsdr();
	return true;
}

void DeviceResources::handle_loss()
{
	if (device_notify_)
		device_notify_->OnDeviceLoss();
	render_target_view_.Reset();
	depth_stencil_view_.Reset();
	swap_chain_.Reset();
	d3d_context_.Reset();
	d3d_annotation_.Reset();
#ifdef _DEBUG
	{
		ComPtr<ID3D11Debug> d3d_debug;
		if (SUCCEEDED(d3d_device_.As(&d3d_debug)))
			d3d_debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
	}
#endif
	d3d_device_.Reset();
	cddr();
	create_swap_chain();
	cwsdr();
	if (device_notify_)
		device_notify_->OnDeviceRestore();
}

void DeviceResources::set_window(HWND hwnd)
{
	hwnd_ = hwnd;
}

void DeviceResources::present()
{
	auto hr = swap_chain_->Present(1, 0);
	// NOTE: If dirty or scroll rects are used, don't discard the render target.
	if (d3d_context_) d3d_context_->DiscardView(render_target_view_.Get());
	if (depth_stencil_view_) d3d_context_->DiscardView(depth_stencil_view_.Get());
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buf[64] = {};
		sprintf_s(buf, "Device lost on Present(): reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? d3d_device_->GetDeviceRemovedReason() : hr);
		OutputDebugStringA(buf);
#endif
		handle_loss();
	}
	else
		ThrowIfFailed(hr);
}

void DeviceResources::GetHardwareAdapter(IDXGIAdapter1** dxgi_adapter)
{
	*dxgi_adapter = nullptr;
	ComPtr<IDXGIFactory1> dxgi_factory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(dxgi_factory.GetAddressOf())));
	ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0;
		DXGI_ERROR_NOT_FOUND != dxgi_factory->EnumAdapters1(adapterIndex, adapter.ReleaseAndGetAddressOf());
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

	*dxgi_adapter = adapter.Detach();
}
