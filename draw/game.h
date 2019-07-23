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
	void update(DX::StepTimer const& timer);
	void render();
	void clear();
	void cdir();
	void cddr();
	void cwsdr();
	void DrawPrimitives(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 sz);
	void DrawText(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 sz);
	void DrawGrid(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 sz, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush);
	void ClearBitmap(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, D2D1::ColorF color);
	void LoadBitmapFromFile(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, std::wstring bitmap_file);
	void LoadBitmapFromResource(Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap, std::wstring resource_name, std::wstring resource_type);
	void CopyBitmapFromMemory(Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap, DirectX::XMUINT2 sz);

	std::unique_ptr<DX::DeviceResources> devrsrc_;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushes_[DX::kBrushCount];
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmaps_[DX::kBitmapCount];
	Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
	Microsoft::WRL::ComPtr<IDWriteFactory5> dwrite_factory_;
	Microsoft::WRL::ComPtr<ID2D1Effect> builtin_effect_, custom_blur_effect_, custom_dropshadow_effect_, ps_effect_;

	DirectX::XMUINT2 tile_size_;
	DX::StepTimer step_timer_;
	FLOAT blur_stddev_;
	D2D_VECTOR_2F dropshadow_offset_;
};