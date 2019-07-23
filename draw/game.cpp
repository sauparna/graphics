#include "draw_pch.h"
#include "game.h"

using namespace std;
using namespace DirectX;
using namespace D2D1;
using namespace DX;
using Microsoft::WRL::ComPtr;

Game::Game(XMUINT2 backBufSize) noexcept(false) :
	devrsrc_{ make_unique<DX::DeviceResources>(XMUINT2{backBufSize}) },
	tile_size_{ 100, 100 },
	blur_stddev_{ 0.f },
	dropshadow_offset_{ 10.f, 10.f }
{
	devrsrc_->RegisterDeviceNotify(this);
}

Game::~Game() {}

void Game::Initialize(HWND hWnd)
{
	devrsrc_->set_window(hWnd);
	devrsrc_->cdir();
	cdir();
	devrsrc_->cddr();
	cddr();
	devrsrc_->create_swap_chain();
	devrsrc_->cwsdr();
	cwsdr();
}

void Game::cdir()
{
	ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), reinterpret_cast<IUnknown * *>(dwrite_factory_.GetAddressOf())));
	ThrowIfFailed(dwrite_factory_->CreateTextFormat(
		L"Segoe UI",
		nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		8.f,
		L"en-us",
		text_format_.GetAddressOf()));
	ThrowIfFailed(text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
	ThrowIfFailed(text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}

void Game::cddr()
{
	auto context = devrsrc_->d2d_context();
	auto backBufFormat = devrsrc_->back_buffer_format();
	auto props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(backBufFormat, D2D1_ALPHA_MODE_IGNORE));

	for (size_t i = 0; i < DX::kBitmapCount; i++)
		ThrowIfFailed(context->CreateBitmap(D2D1_SIZE_U{ tile_size_.x, tile_size_.y }, nullptr, 0, props, bitmaps_[i].ReleaseAndGetAddressOf()));

	ThrowIfFailed(context->CreateSolidColorBrush(ColorF(ColorF::White), brushes_[BRUSH_WHITE].ReleaseAndGetAddressOf()));
	ThrowIfFailed(context->CreateSolidColorBrush(ColorF(ColorF::Black), brushes_[BRUSH_BLACK].ReleaseAndGetAddressOf()));
	ThrowIfFailed(context->CreateSolidColorBrush(ColorF(ColorF::SkyBlue), brushes_[BRUSH_SKYBLUE].ReleaseAndGetAddressOf()));

	DrawPrimitives(bitmaps_[BITMAP_PRIMITIVES], tile_size_);
	DrawGrid(bitmaps_[BITMAP_PRIMITIVES_GRID], tile_size_, brushes_[BRUSH_WHITE]);
	DrawText(bitmaps_[BITMAP_TEXT], tile_size_);
	LoadBitmapFromFile(bitmaps_[BITMAP_FROM_FILE], L"..\\assets\\tintin.jpg");
	LoadBitmapFromResource(bitmaps_[BITMAP_FROM_RSRC], L"TintinAndSnowy", L"Image");
	CopyBitmapFromMemory(bitmaps_[BITMAP_FROM_MEM], tile_size_);
}

void Game::cwsdr()
{
}

void Game::Tick()
{
	step_timer_.Tick([&]() { update(step_timer_); });
	render();
}

void Game::update(DX::StepTimer const& timer)
{
}

void Game::render()
{
	if (step_timer_.FrameCount() == 0)
		return;
	auto context = devrsrc_->d2d_context();
	auto sz = devrsrc_->back_buffer_size();
	context->BeginDraw();
	context->Clear(ColorF(ColorF::LightCoral, 0.f)); // clear the back buffer

	context->DrawImage(bitmaps_[BITMAP_PRIMITIVES].Get(), D2D1_POINT_2F{ 20.f, 20.f });
	context->DrawImage(bitmaps_[BITMAP_PRIMITIVES_GRID].Get(), D2D1_POINT_2F{ 140.f, 20.f });
	context->DrawImage(bitmaps_[BITMAP_TEXT].Get(), D2D1_POINT_2F{ 260.f, 20.f });
	context->DrawImage(bitmaps_[BITMAP_FROM_FILE].Get(), D2D1_POINT_2F{ 380.f, 20.f });
	context->DrawImage(bitmaps_[BITMAP_FROM_RSRC].Get(), D2D1_POINT_2F{ 500.f, 20.f });
	context->DrawImage(bitmaps_[BITMAP_FROM_MEM].Get(), D2D1_POINT_2F{ 620.f, 20.f });

	// draw a cross-hair at the center of the back buffer, on top of everything else
	XMFLOAT2 pos1{ static_cast<float>(sz.x / 2), static_cast<float>(sz.y / 2) };
	XMFLOAT2 sz1{ 10.f, 10.f };
	context->DrawLine(D2D1_POINT_2F{ pos1.x, pos1.y - sz1.y }, D2D1_POINT_2F{ pos1.x, pos1.y + sz1.y }, brushes_[BRUSH_WHITE].Get());
	context->DrawLine(D2D1_POINT_2F{ pos1.x - sz1.x, pos1.y }, D2D1_POINT_2F{ pos1.x + sz1.y, pos1.y }, brushes_[BRUSH_WHITE].Get());

	auto hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);
	devrsrc_->present();
}

void Game::DrawGrid(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 bitmap_sz, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush)
{
	auto context = devrsrc_->d2d_context();
	ComPtr<ID2D1Image> prevTarget;
	context->GetTarget(prevTarget.GetAddressOf());
	context->SetTarget(bitmap.Get());
	context->BeginDraw();
	context->Clear(ColorF(ColorF::Black, 0.f));
	float y1 = 0.f, y2 = static_cast<float>(bitmap_sz.y);
	for (float x = 10.f; x < static_cast<float>(bitmap_sz.x); x += 10.f) // draw the vertical lines
		context->DrawLine(D2D1_POINT_2F{ x, y1 }, D2D1_POINT_2F{ x, y2 }, brush.Get());
	float x1 = 0.f, x2 = static_cast<float>(bitmap_sz.x);
	for (float y = 10.f; y < static_cast<float>(bitmap_sz.y); y += 10.f) // draw the horizontal lines
		context->DrawLine(D2D1_POINT_2F{ x1, y }, D2D1_POINT_2F{ x2, y }, brush.Get());
	auto hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);
	context->SetTarget(prevTarget.Get());
}

void Game::DrawPrimitives(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 size)
{
	auto context = devrsrc_->d2d_context();
	XMFLOAT2 pos{ static_cast<FLOAT>(size.x / 4), static_cast<FLOAT>(size.y / 4) };
	XMFLOAT2 sz{ 30.f, 30.f };
	ComPtr<ID2D1Image> prevTarget;
	context->GetTarget(prevTarget.GetAddressOf());
	context->SetTarget(bitmap.Get());
	context->BeginDraw();
	context->Clear(ColorF(ColorF::Teal, 0.0f));
	context->FillRectangle(RectF(pos.x, pos.y, pos.x + sz.x, pos.y + sz.y), brushes_[BRUSH_WHITE].Get());
	auto hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);
	context->SetTarget(prevTarget.Get());
}

void Game::DrawText(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 size)
{
	auto context = devrsrc_->d2d_context();

	XMFLOAT2 pos{ 0.f, 0.f }; // text-area rectangle's top-left
	XMFLOAT2 sz{ static_cast<FLOAT>(size.x), static_cast<FLOAT>(size.y) }; // the rectangle's dimension

	ComPtr<ID2D1Image> prevTarget;
	context->GetTarget(prevTarget.GetAddressOf());
	context->SetTarget(bitmap.Get());

	auto text = L"\"If I had a world of my own, everything would be nonsense. \
Nothing would be what it is, because everything would be what it isn't. \
And contrary wise, what is, it wouldn't be. And what it wouldn't be, it would. You see?\"";

	context->BeginDraw();

	context->Clear(ColorF(ColorF::White, 0.0f));

	context->DrawText(text, wcslen(text), text_format_.Get(),
		RectF(pos.x, pos.y, pos.x + sz.x, pos.y + sz.y),
		brushes_[BRUSH_BLACK].Get());

	auto hr = context->EndDraw();

	// Check for error but avoid handling a device loss because that situation is handled by
	// the next call to Present().
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);

	context->SetTarget(prevTarget.Get());
}

void Game::ClearBitmap(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, ColorF color)
{
	auto context = devrsrc_->d2d_context();

	ComPtr<ID2D1Image> prevTarget;
	context->GetTarget(prevTarget.GetAddressOf());
	context->SetTarget(bitmap.Get());

	context->BeginDraw();
	context->Clear(color);
	auto hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);

	context->SetTarget(prevTarget.Get());
}

void Game::clear()
{
	auto context = devrsrc_->d2d_context();
	context->Clear(ColorF(ColorF::Blue));
}

void Game::OnDeviceLoss()
{
	for (size_t i = 0; i < kBrushCount; i++)
		brushes_[i].Reset();

	builtin_effect_.Reset();
	custom_blur_effect_.Reset();
	custom_dropshadow_effect_.Reset();
	ps_effect_.Reset();

	for (size_t i = 0; i < kBitmapCount; i++)
		bitmaps_[i].Reset();
}

void Game::OnDeviceRestore()
{
	cddr();
	cwsdr();
}

void Game::OnActivate() {}

void Game::OnDeactivate() {}

void Game::OnSuspend() {}

void Game::OnResume() {}

void Game::OnWindowMove()
{
	auto sz = devrsrc_->back_buffer_size();
	devrsrc_->handle_resize(sz.x, sz.y);
}

void Game::OnWindowResize(size_t w, size_t h)
{
	devrsrc_->handle_resize(w, h);
	this->cwsdr();
}

void Game::LoadBitmapFromResource(Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap, wstring resource_name, wstring resource_type)
{
	HRSRC imageResourceHandle = nullptr;
	imageResourceHandle = FindResourceW(HINST_THISCOMPONENT, resource_name.c_str(), resource_type.c_str());
	HRESULT hr = imageResourceHandle ? S_OK : E_FAIL;
	ThrowIfFailed(hr);

	HGLOBAL imageResourceDataHandle = nullptr;
	imageResourceDataHandle = LoadResource(HINST_THISCOMPONENT, imageResourceHandle);
	hr = imageResourceDataHandle ? S_OK : E_FAIL;
	ThrowIfFailed(hr);

	void* imageFile = nullptr;
	imageFile = LockResource(imageResourceDataHandle);
	hr = imageFile ? S_OK : E_FAIL;
	ThrowIfFailed(hr);

	DWORD imageFileSize = 0;
	imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResourceHandle);
	hr = imageFileSize ? S_OK : E_FAIL;
	ThrowIfFailed(hr);

	ComPtr<IWICImagingFactory> iwicFactory;
	ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(iwicFactory.GetAddressOf())));

	ComPtr<IWICStream> iwicStream;
	ThrowIfFailed(iwicFactory->CreateStream(iwicStream.GetAddressOf()));
	iwicStream->InitializeFromMemory(reinterpret_cast<BYTE*>(imageFile), imageFileSize);

	ComPtr<IWICBitmapDecoder> iwicDecoder;
	ThrowIfFailed(iwicFactory->CreateDecoderFromStream(iwicStream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, iwicDecoder.GetAddressOf()));

	ComPtr<IWICBitmapFrameDecode> iwicDecoderFrame;
	ThrowIfFailed(iwicDecoder->GetFrame(0, iwicDecoderFrame.GetAddressOf()));

	ComPtr<IWICFormatConverter> iwicFormatConverter;
	ThrowIfFailed(iwicFactory->CreateFormatConverter(iwicFormatConverter.GetAddressOf()));
	ThrowIfFailed(iwicFormatConverter->Initialize(iwicDecoderFrame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeCustom));

	auto context = devrsrc_->d2d_context();

	// TODO: this create-bitmap and preceding parapharnalia should move to cddr
	ComPtr<ID2D1Bitmap1> bitmap;
	ThrowIfFailed(context->CreateBitmapFromWicBitmap(iwicFormatConverter.Get(), bitmap.GetAddressOf()));

	ComPtr<ID2D1Image> prevTarget;
	context->GetTarget(prevTarget.GetAddressOf());
	context->SetTarget(target_bitmap.Get());

	context->BeginDraw();
	context->DrawBitmap(bitmap.Get());
	hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);

	context->SetTarget(prevTarget.Get());
}

void Game::LoadBitmapFromFile(Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap, wstring bitmap_file)
{
	ComPtr<IWICImagingFactory> iwicFactory;
	ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(iwicFactory.GetAddressOf())));
	ComPtr<IWICBitmapDecoder> iwicDecoder;
	ThrowIfFailed(iwicFactory->CreateDecoderFromFilename(bitmap_file.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, iwicDecoder.GetAddressOf()));
	ComPtr<IWICBitmapFrameDecode> iwicDecoderFrame;
	ThrowIfFailed(iwicDecoder->GetFrame(0, iwicDecoderFrame.GetAddressOf()));

	// FIXIT: writing to bitmap memory has no effect
	//ComPtr<IWICBitmap> iwicBitmap;
	//ThrowIfFailed(iwicFactory->CreateBitmapFromSource(iwicDecoderFrame.Get(), WICBitmapCacheOnDemand, iwicBitmap.GetAddressOf()));
	//ComPtr<IWICBitmapLock> iwicLock;
	//WICRect lock_rect = { 0, 0, 10, 10 };
	//ThrowIfFailed(iwicBitmap->Lock(&lock_rect, WICBitmapLockWrite, iwicLock.GetAddressOf()));
	//unsigned buf_sz = 0;
	//BYTE* pv = nullptr;
	//ThrowIfFailed(iwicLock->GetDataPointer(&buf_sz, &pv));
	//for (unsigned y = 0; y < 10; y++)
	//	for (unsigned x = 0; x < 10 * 24; x++)
	//		* (pv + 10 * y + x) = 0x00;
	//iwicLock.Reset();
	//iwicBitmap.Reset();

	ComPtr<IWICFormatConverter> iwicFormatConverter;
	ThrowIfFailed(iwicFactory->CreateFormatConverter(iwicFormatConverter.GetAddressOf()));
	ThrowIfFailed(iwicFormatConverter->Initialize(iwicDecoderFrame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeCustom));

	auto context = devrsrc_->d2d_context();

	ComPtr<ID2D1Bitmap1> bitmap;
	ThrowIfFailed(context->CreateBitmapFromWicBitmap(iwicFormatConverter.Get(), bitmap.GetAddressOf()));

	ComPtr<ID2D1Image> prevTarget;
	context->GetTarget(prevTarget.GetAddressOf());
	context->SetTarget(target_bitmap.Get());

	context->BeginDraw();
	context->DrawBitmap(bitmap.Get());
	auto hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);

	context->SetTarget(prevTarget.Get());
}

void Game::CopyBitmapFromMemory(Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap, XMUINT2 bitmap_size)
{
	array<int, 100 * 100 * 4> mem;
	mem.fill(0);

	for (unsigned y = 20; y < 80; y++)
		for (unsigned x = 20; x < 80; x++)
			mem[y * 100 + x] = 0x00ffff00;

	ComPtr<ID2D1Bitmap1> bitmap;
	auto context = devrsrc_->d2d_context();
	auto backBufFormat = devrsrc_->back_buffer_format();
	auto props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(backBufFormat, D2D1_ALPHA_MODE_IGNORE));
	ThrowIfFailed(context->CreateBitmap(D2D1_SIZE_U{ bitmap_size.x, bitmap_size.y }, nullptr, 0, props, bitmap.ReleaseAndGetAddressOf()));

	D2D1_RECT_U dstRect{ 0, 0, bitmap_size.x, bitmap_size.y };
	ThrowIfFailed(bitmap->CopyFromMemory(&dstRect, mem.data(), 100 * 4));

	ComPtr<ID2D1Image> prevTarget;
	context->GetTarget(prevTarget.GetAddressOf());
	context->SetTarget(target_bitmap.Get());

	context->BeginDraw();
	context->DrawBitmap(bitmap.Get());
	auto hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);

	context->SetTarget(prevTarget.Get());
}