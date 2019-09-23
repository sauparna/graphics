#pragma once

namespace DX
{
	// For an application, that owns DeviceResources, to be notified of loss and creation.
	interface IDeviceNotify
	{
		virtual void OnDeviceLoss() = 0;
		virtual void OnDeviceRestore() = 0;
	};

	class DeviceResources
	{
	public:
		DeviceResources(DirectX::XMUINT2 back_buffer_size = DirectX::XMUINT2{ 400, 300 }, DXGI_FORMAT back_buffer_format = DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT depth_buffer_format = DXGI_FORMAT_D24_UNORM_S8_UINT, UINT back_buffer_count = 2, D3D_FEATURE_LEVEL min_feature_level = D3D_FEATURE_LEVEL_9_1) noexcept;
		void RegisterDeviceNotify(IDeviceNotify* device_notify) { device_notify_ = device_notify; }
		void cdir();
		void cddr();
		void cwsdr();
		void create_swap_chain();
		void modify_swap_chain();
		bool handle_resize(size_t w, size_t h);
		void handle_loss();
		void set_window(HWND hWnd);
		void present();
		RECT output_rect() const { return output_rect_; }
		ID3D11Device* d3d_device() const { return d3d_device_.Get(); }
		ID3D11DeviceContext* d3d_context() const { return d3d_context_.Get(); }
		IDXGISwapChain* swap_chain() const { return swap_chain_.Get(); }
		D3D_FEATURE_LEVEL feature_level() const { return d3d_feature_level_; }
		ID3D11Texture2D* render_target() const { return render_target_.Get(); }
		ID3D11Texture2D* depth_stencil() const { return depth_stencil_.Get(); }
		ID3D11RenderTargetView* rendet_target_view() const { return render_target_view_.Get(); }
		ID3D11DepthStencilView* depth_stencil_view() const { return depth_stencil_view_.Get(); }
		DirectX::XMUINT2 back_buffer_size() const { return back_buffer_size_; }
		DXGI_FORMAT back_buffer_format() const { return back_buffer_format_; }
		DXGI_FORMAT depth_buffer_format() const { return depth_buffer_format_; }
		D3D11_VIEWPORT screen_viewport() const { return screen_viewport_; }
		UINT back_buffer_count() const { return back_buffer_count_; }
		void PIXBeginEvent(_In_z_ const wchar_t* name) { if (d3d_annotation_) d3d_annotation_->BeginEvent(name); }
		void PIXEndEvent() { if (d3d_annotation_) d3d_annotation_->EndEvent(); }
		void PIXSetMarket(_In_z_ const wchar_t* name) { if (d3d_annotation_) d3d_annotation_->SetMarker(name); }

	private:
		void GetHardwareAdapter(IDXGIAdapter1** ppAdapter);
		Microsoft::WRL::ComPtr<ID3D11Device5> d3d_device_;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext4> d3d_context_;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain_;
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> d3d_annotation_;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view_;
		DirectX::XMUINT2 back_buffer_size_;
		DXGI_FORMAT back_buffer_format_;
		DXGI_FORMAT depth_buffer_format_;
		UINT back_buffer_count_;
		D3D_FEATURE_LEVEL d3d_min_feature_level_;
		D3D_FEATURE_LEVEL d3d_feature_level_;
		D3D11_VIEWPORT screen_viewport_;
		HWND hwnd_;
		RECT output_rect_;
		IDeviceNotify* device_notify_;
	};
}

