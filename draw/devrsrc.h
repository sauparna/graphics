#pragma once

namespace DX
{
	interface IDeviceNotify
	{
		virtual void OnDeviceLoss() = 0;
		virtual void OnDeviceRestore() = 0;
	};

	class DeviceResources
	{
	public:

		DeviceResources(
			DirectX::XMUINT2 back_buffer_size = DirectX::XMUINT2{ 400, 300 },
			UINT back_buffer_count = 2,
			DXGI_FORMAT back_buffer_format = DXGI_FORMAT_B8G8R8A8_UNORM,
			DXGI_FORMAT depth_buffer_format = DXGI_FORMAT_D24_UNORM_S8_UINT,
			D3D_FEATURE_LEVEL d3d_min_feature_level = D3D_FEATURE_LEVEL_9_1
		) noexcept;
		~DeviceResources();
		void RegisterDeviceNotify(IDeviceNotify* device_notify) { device_notify_ = device_notify; }
		void cdir();
		void cddr();
		void cwsdr();
		void modify_swap_chain();
		void create_swap_chain();
		bool handle_resize(size_t w, size_t h);
		void handle_loss();
		void present();
		void set_window(HWND hWnd);
		ID2D1DeviceContext* d2d_context() const { return d2d_context_.Get(); }
		ID2D1Factory7* d2d_factory() const { return d2d_factory_.Get(); }
		DirectX::XMUINT2 back_buffer_size() const { return back_buffer_size_; }
		DXGI_FORMAT back_buffer_format() const { return back_buffer_format_; }
		UINT back_buffer_count() const { return back_buffer_count_; }

	private:
		Microsoft::WRL::ComPtr<ID3D11Device5> d3d_device_;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext4> d3d_context_;
		Microsoft::WRL::ComPtr<ID2D1Device6> d2d_device_;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext6> d2d_context_;
		Microsoft::WRL::ComPtr<IDXGISwapChain1>	swap_chain_;
		Microsoft::WRL::ComPtr<ID2D1Factory7> d2d_factory_;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_bitmap_;
		DXGI_FORMAT	back_buffer_format_, depth_buffer_format_;
		UINT back_buffer_count_;
		D3D_FEATURE_LEVEL d3d_min_feature_level_, d3d_feature_level_;
		HWND hwnd_;
		DirectX::XMUINT2 back_buffer_size_;
		IDeviceNotify* device_notify_;
	};
}