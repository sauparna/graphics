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
			DirectX::XMUINT2 backBufSize = DirectX::XMUINT2{ 400, 300 },
			UINT backBufCount = 2,
			DXGI_FORMAT backBufFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
			DXGI_FORMAT depthBufFormat = DXGI_FORMAT_D24_UNORM_S8_UINT,
			D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_9_1
		) noexcept;

		~DeviceResources();

		void RegisterDeviceNotify(IDeviceNotify* deviceNotify) { m_deviceNotify = deviceNotify; }

		void cdir();
		void cddr();
		void cwsdr();

		void SetWindow(HWND hWnd);
		bool HandleResize(size_t w, size_t h);
		void HandleLoss();
		void ModifySwapChain();
		void CreateSwapChain();
		void Present();

		ID2D1DeviceContext* GetD2DContext() const { return m_d2dContext.Get(); }
		ID2D1Factory7* GetD2DFactory() const { return m_d2dFactory.Get(); }
		DirectX::XMUINT2 GetBackBufSize() const { return m_backBufSize; }
		DXGI_FORMAT GetBackBufFormat() const { return m_backBufFormat; }
		UINT GetBackBufCount() const { return m_backBufCount; }

	private:
		Microsoft::WRL::ComPtr<ID3D11Device5>			m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext4>	m_d3dContext;
		Microsoft::WRL::ComPtr<ID2D1Device6>			m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext6>		m_d2dContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain1>			m_swapChain;
		Microsoft::WRL::ComPtr<ID2D1Factory7>			m_d2dFactory;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1>			m_d2dBitmap;

		DXGI_FORMAT			m_backBufFormat, m_depthBufFormat;
		UINT				m_backBufCount;
		D3D_FEATURE_LEVEL	m_d3dMinFeatureLevel, m_d3dFeatureLevel;
		HWND				m_hWnd;
		DirectX::XMUINT2	m_backBufSize;
		IDeviceNotify* m_deviceNotify;
	};
}