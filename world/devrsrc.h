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
		DeviceResources(DirectX::XMUINT2 backBufSize = DirectX::XMUINT2{ 400, 300 }, DXGI_FORMAT bbufFormat = DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT, UINT bbufCount = 2, D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_9_1) noexcept;
		void RegisterDeviceNotify(IDeviceNotify* deviceNotify) { m_deviceNotify = deviceNotify; }

		void cdir();
		void cddr();
		void cwsdr();
		void CreateSwapChain();
		void ModifySwapChain();
		bool HandleResize(size_t w, size_t h);
		void HandleLoss();
		void SetWindow(HWND hWnd);
		void Present();
		RECT GetOutputRect() const { return m_outputRect; }
		ID3D11Device* GetD3DDevice() const { return m_d3dDevice.Get(); }
		ID3D11DeviceContext* GetD3DContext() const { return m_d3dContext.Get(); }
		IDXGISwapChain* GetSwapChain() const { return m_swapChain.Get(); }
		D3D_FEATURE_LEVEL GetFeatureLevel() const { return m_d3dFeatureLevel; }
		ID3D11Texture2D* GetRenderTarget() const { return m_renderTarget.Get(); }
		ID3D11Texture2D* GetDepthStencil() const { return m_depthStencil.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_d3dRenderTargetView.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_d3dDepthStencilView.Get(); }
		DirectX::XMUINT2 GetBackBufSize() const { return m_backBufSize; }
		DXGI_FORMAT GetBackBufferFormat() const { return m_bbufFormat; }
		DXGI_FORMAT GetDepthBufferFormat() const { return m_depthBufferFormat; }
		D3D11_VIEWPORT GetScreenViewPort() const { return m_screenViewport; }
		UINT GetBackBufferCount() const { return m_bbufCount; }
		void PIXBeginEvent(_In_z_ const wchar_t* name) { if (m_d3dAnnotation) m_d3dAnnotation->BeginEvent(name); }
		void PIXEndEvent() { if (m_d3dAnnotation) m_d3dAnnotation->EndEvent(); }
		void PIXSetMarket(_In_z_ const wchar_t* name) { if (m_d3dAnnotation) m_d3dAnnotation->SetMarker(name); }

	private:
		void GetHardwareAdapter(IDXGIAdapter1** ppAdapter);
		Microsoft::WRL::ComPtr<ID3D11Device5>				m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext4>		m_d3dContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain1>				m_swapChain;
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_renderTarget;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_depthStencil;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_d3dRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_d3dDepthStencilView;
		DirectX::XMUINT2	m_backBufSize;
		DXGI_FORMAT			m_bbufFormat;
		DXGI_FORMAT			m_depthBufferFormat;
		UINT				m_bbufCount;
		D3D_FEATURE_LEVEL	m_d3dMinFeatureLevel;
		D3D_FEATURE_LEVEL	m_d3dFeatureLevel;
		D3D11_VIEWPORT		m_screenViewport;
		HWND				m_hWnd;
		RECT				m_outputRect;
		IDeviceNotify*		m_deviceNotify;
	};
}

