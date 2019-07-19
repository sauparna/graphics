#pragma once

#include "devrsrc.h"
#include "steptimer.h"

class Game final : public DX::IDeviceNotify
{
public:
	Game(DirectX::XMUINT2 backBufSize = DirectX::XMUINT2{ 400, 300 }) noexcept(false);
	~Game();
	void Initialize(HWND hWnd);
	virtual void OnDeviceLoss() override;
	virtual void OnDeviceRestore() override;
	void OnActivate();
	void OnDeactivate();
	void OnSuspend();
	void OnResume();
	void OnWindowMove();
	void OnWindowResize(size_t w, size_t h);
	void Tick();

private:
	void Update(DX::StepTimer const& timer);
	void Render();
	void Clear();
	void cdir();
	void cddr();
	void cwsdr();
	void CreateBitmaps();
	void draw_primitives(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 sz);
	void draw_text(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 sz);
	void draw_grid(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 sz, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush);
	void clear_bitmap(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, D2D1::ColorF color);

	std::unique_ptr<DX::DeviceResources> m_devrsrc;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_whiteBrush, m_blackBrush, m_blueBrush;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_bitmap1, m_bitmap2, m_bitmap3;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
	Microsoft::WRL::ComPtr<IDWriteFactory5> m_dwriteFactory;
	Microsoft::WRL::ComPtr<ID2D1Effect> m_builtInEffect, m_customBlurEffect, m_customDropShadowEffect, m_psEffect;

	DirectX::XMUINT2 m_sz;
	DX::StepTimer m_timer;
	FLOAT m_blurStddev;
	D2D_VECTOR_2F m_dropshadowOffset;
};