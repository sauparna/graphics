#include "draw_pch.h"
#include "devrsrc.h"

using namespace DirectX;
using namespace D2D1;
using namespace DX;
using Microsoft::WRL::ComPtr;

DeviceResources::DeviceResources(XMUINT2 back_buffer_size, UINT back_buffer_count, DXGI_FORMAT back_buffer_format, DXGI_FORMAT depth_buffer_format, D3D_FEATURE_LEVEL d3d_min_feature_level)
noexcept : 
	back_buffer_size_{ back_buffer_size },
	back_buffer_count_{ back_buffer_count },
	back_buffer_format_{ back_buffer_format },
	depth_buffer_format_{ depth_buffer_format },
	d3d_device_{ nullptr },
	d3d_context_{ nullptr },
	d2d_device_{ nullptr },
	d2d_context_{ nullptr },
	d2d_factory_{ nullptr },
	d2d_bitmap_{ nullptr },
	d3d_min_feature_level_{ d3d_min_feature_level },
	d3d_feature_level_{ D3D_FEATURE_LEVEL_9_1 },
	hwnd_{ nullptr },
	device_notify_{ nullptr }
{ }

DeviceResources::~DeviceResources()
{
	swap_chain_.Reset();
	d2d_bitmap_.Reset();
	d2d_context_.Reset();
	d2d_device_.Reset();
	d3d_context_.Reset();
	d3d_device_.Reset();
	d2d_factory_.Reset();
}

void DeviceResources::set_window(HWND hwnd)
{
	hwnd_ = hwnd;
}

void DeviceResources::cdir()
{
	D2D1_FACTORY_OPTIONS opts = {};
#if defined(_DEBUG)
	opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, opts, d2d_factory_.GetAddressOf()));
}

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
	d3d_creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	if (FAILED(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		d3d_creation_flags,
		d3d_feature_level_table,
		d3d_feature_level_count,
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(),
		&d3d_feature_level_,
		context.ReleaseAndGetAddressOf())
	))
	{
		ThrowIfFailed(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_WARP,
			nullptr,
			d3d_creation_flags,
			d3d_feature_level_table,
			d3d_feature_level_count,
			D3D11_SDK_VERSION,
			device.ReleaseAndGetAddressOf(),
			&d3d_feature_level_,
			context.ReleaseAndGetAddressOf())
		);
		OutputDebugStringA("Direct3D adapter: WARP.\n");
	}

	ThrowIfFailed(device.As(&d3d_device_));
	ThrowIfFailed(context.As(&d3d_context_));

	// use a DXGI dvice as a bridge to get to a D2D device from a D3D device

	ComPtr<IDXGIDevice4> dxgi_device;
	ThrowIfFailed(d3d_device_.As(&dxgi_device));
	ThrowIfFailed(d2d_factory_->CreateDevice(dxgi_device.Get(), d2d_device_.ReleaseAndGetAddressOf()));
	ThrowIfFailed(d2d_device_->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2d_context_.ReleaseAndGetAddressOf()));
}

void DeviceResources::cwsdr()
{
	// Set the render target to a D2D bitmap.

	ComPtr<IDXGISurface> dxgi_surface;
	ThrowIfFailed(swap_chain_->GetBuffer(0, IID_PPV_ARGS(dxgi_surface.GetAddressOf())));

	auto d2d_bitmap_properties = BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		PixelFormat(back_buffer_format_, D2D1_ALPHA_MODE_IGNORE)
	);

	ThrowIfFailed(d2d_context_->CreateBitmapFromDxgiSurface(dxgi_surface.Get(), d2d_bitmap_properties, d2d_bitmap_.GetAddressOf()));

	d2d_context_->SetTarget(d2d_bitmap_.Get());
}

void DeviceResources::create_swap_chain()
{
	ComPtr<IDXGIDevice> dxgi_device;
	ThrowIfFailed(d3d_device_.As(&dxgi_device));

	ComPtr<IDXGIAdapter> dxgi_adapter;
	ThrowIfFailed(dxgi_device->GetAdapter(dxgi_adapter.GetAddressOf()));

	ComPtr<IDXGIFactory> dxgi_factory;
	ThrowIfFailed(dxgi_adapter->GetParent(IID_PPV_ARGS(dxgi_factory.GetAddressOf())));

	ComPtr<IDXGIFactory2> dxgi_factory2;
	ThrowIfFailed(dxgi_factory.As(&dxgi_factory2));

	DXGI_SWAP_CHAIN_DESC1 dxgi_swap_chain_descriptor = {};
	dxgi_swap_chain_descriptor.Width = back_buffer_size_.x;
	dxgi_swap_chain_descriptor.Height = back_buffer_size_.y;
	dxgi_swap_chain_descriptor.Format = back_buffer_format_;
	dxgi_swap_chain_descriptor.BufferCount = back_buffer_count_;
	dxgi_swap_chain_descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgi_swap_chain_descriptor.Scaling = DXGI_SCALING_STRETCH;
	dxgi_swap_chain_descriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dxgi_swap_chain_descriptor.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	dxgi_swap_chain_descriptor.SampleDesc.Count = 1;
	dxgi_swap_chain_descriptor.SampleDesc.Quality = 0;

	ThrowIfFailed(dxgi_factory2->CreateSwapChainForHwnd(d3d_device_.Get(), hwnd_, &dxgi_swap_chain_descriptor, nullptr, nullptr, swap_chain_.ReleaseAndGetAddressOf()));
}

void DeviceResources::modify_swap_chain()
{
	d2d_bitmap_.Reset();
	d2d_context_->SetTarget(nullptr);

	HRESULT hr = swap_chain_->ResizeBuffers(back_buffer_count_, back_buffer_size_.x, back_buffer_size_.y, back_buffer_format_, 0);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buf[64] = {};
		sprintf_s(buf, "Device lost on ResizeBuffers(): Reason code 0x%08X\n",
			(hr == DXGI_ERROR_DEVICE_REMOVED) ? d3d_device_->GetDeviceRemovedReason() : hr);
		OutputDebugStringA(buf);
#endif // _DEBUG
		handle_loss();
	}
	else
		ThrowIfFailed(hr);
}

bool DeviceResources::handle_resize(size_t w, size_t h)
{
	if (!hwnd_)
		throw std::exception("Missing Win32 window handle.");

	// Do nothing if the size hasn't changed.
	if (back_buffer_size_.x == w && back_buffer_size_.y == h)
		return false;

	back_buffer_size_.x = std::max<int32_t>(w, 1);
	back_buffer_size_.y = std::max<int32_t>(h, 1);

	if (swap_chain_)
		modify_swap_chain();
	else
		create_swap_chain();

	cwsdr();

	return true;
}

void DeviceResources::handle_loss()
{
	swap_chain_.Reset();
	d2d_bitmap_.Reset();
	d2d_context_.Reset();
	d2d_device_.Reset();
	d3d_context_.Reset();

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
}

void DeviceResources::present()
{
	HRESULT hr = swap_chain_->Present(1, 0);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buf[64] = {};
		sprintf_s(buf, "Device lost on Present(): Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? d3d_device_->GetDeviceRemovedReason() : hr);
		OutputDebugStringA(buf);
#endif
		handle_loss();
	}
	else
		ThrowIfFailed(hr);
}
