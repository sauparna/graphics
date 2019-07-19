#include "draw_pch.h"
#include "game.h"

using namespace std;
using namespace DirectX;
using namespace D2D1;
using namespace DX;
using Microsoft::WRL::ComPtr;

Game::Game(XMUINT2 backBufSize) noexcept(false) :
	m_devrsrc{ make_unique<DX::DeviceResources>(XMUINT2{backBufSize}) },
	m_sz{ 100, 100 },
	m_blurStddev{ 0.f },
	m_dropshadowOffset{ 10.f, 10.f }
{
	m_devrsrc->RegisterDeviceNotify(this);
}

Game::~Game() {}

void Game::Initialize(HWND hWnd)
{
	m_devrsrc->SetWindow(hWnd);
	m_devrsrc->cdir();
	cdir();
	m_devrsrc->cddr();
	cddr();
	m_devrsrc->CreateSwapChain();
	m_devrsrc->cwsdr();
	cwsdr();
}

void Game::cdir()
{
	ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), reinterpret_cast<IUnknown * *>(m_dwriteFactory.GetAddressOf())));
	ThrowIfFailed(m_dwriteFactory->CreateTextFormat(
		L"Segoe UI",
		nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		8.f,
		L"en-us",
		m_textFormat.GetAddressOf()));
	ThrowIfFailed(m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
	ThrowIfFailed(m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}

void Game::cddr()
{
	auto context = m_devrsrc->GetD2DContext();
	ThrowIfFailed(context->CreateSolidColorBrush(ColorF(ColorF::White), m_whiteBrush.ReleaseAndGetAddressOf()));
	ThrowIfFailed(context->CreateSolidColorBrush(ColorF(ColorF::Black), m_blackBrush.ReleaseAndGetAddressOf()));
	ThrowIfFailed(context->CreateSolidColorBrush(ColorF(ColorF::SkyBlue), m_blueBrush.ReleaseAndGetAddressOf()));
	CreateBitmaps();
	draw_primitives(m_bitmap1, m_sz);
	draw_grid(m_bitmap2, m_sz, m_whiteBrush);
	draw_text(m_bitmap3, m_sz);
}

void Game::cwsdr()
{
}

void Game::Tick()
{
	m_timer.Tick([&]() { Update(m_timer); });
	Render();
}

void Game::Update(DX::StepTimer const& timer)
{
}

void Game::CreateBitmaps()
{
	auto backBufFormat = m_devrsrc->GetBackBufFormat();
	auto context = m_devrsrc->GetD2DContext();
	auto props = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET,
		D2D1::PixelFormat(backBufFormat, D2D1_ALPHA_MODE_IGNORE)
	);

	D2D1_SIZE_U sz{ static_cast<UINT32>(m_sz.x), static_cast<UINT32>(m_sz.y) };

	DX::ThrowIfFailed(context->CreateBitmap(sz, nullptr, 0, props, m_bitmap1.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(context->CreateBitmap(sz, nullptr, 0, props, m_bitmap2.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(context->CreateBitmap(sz, nullptr, 0, props, m_bitmap3.ReleaseAndGetAddressOf()));
}

void Game::Render()
{
	if (m_timer.FrameCount() == 0)
		return;
	auto context = m_devrsrc->GetD2DContext();
	auto sz = m_devrsrc->GetBackBufSize();
	context->BeginDraw();
	context->Clear(ColorF(ColorF::LightCoral, 0.f)); // clear the back buffer
	context->DrawImage(m_bitmap1.Get(), D2D1_POINT_2F{ 20.f, 20.f });
	context->DrawImage(m_bitmap2.Get(), D2D1_POINT_2F{ 140.f, 20.f });
	context->DrawImage(m_bitmap3.Get(), D2D1_POINT_2F{ 260.f, 20.f });

	// draw a cross-hair at the center of the back buffer, on top of everything else
	XMFLOAT2 pos1{ static_cast<float>(sz.x / 2), static_cast<float>(sz.y / 2) };
	XMFLOAT2 sz1{ 10.f, 10.f };
	context->DrawLine(D2D1_POINT_2F{ pos1.x, pos1.y - sz1.y }, D2D1_POINT_2F{ pos1.x, pos1.y + sz1.y }, m_whiteBrush.Get());
	context->DrawLine(D2D1_POINT_2F{ pos1.x - sz1.x, pos1.y }, D2D1_POINT_2F{ pos1.x + sz1.y, pos1.y }, m_whiteBrush.Get());

	auto hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);
	m_devrsrc->Present();
}

void Game::draw_grid(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 bitmap_sz, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush)
{
	auto context = m_devrsrc->GetD2DContext();
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

void Game::draw_primitives(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 size)
{
	auto context = m_devrsrc->GetD2DContext();
	XMFLOAT2 pos{ static_cast<FLOAT>(size.x / 4), static_cast<FLOAT>(size.y / 4) };
	XMFLOAT2 sz{ 30.f, 30.f };
	ComPtr<ID2D1Image> prevTarget;
	context->GetTarget(prevTarget.GetAddressOf());
	context->SetTarget(bitmap.Get());
	context->BeginDraw();
	context->Clear(ColorF(ColorF::Teal, 0.0f));
	context->FillRectangle(RectF(pos.x, pos.y, pos.x + sz.x, pos.y + sz.y), m_whiteBrush.Get());
	auto hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);
	context->SetTarget(prevTarget.Get());
}

void Game::draw_text(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, DirectX::XMUINT2 size)
{
	auto context = m_devrsrc->GetD2DContext();

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

	context->DrawText(text, wcslen(text), m_textFormat.Get(),
		RectF(pos.x, pos.y, pos.x + sz.x, pos.y + sz.y),
		m_blackBrush.Get());

	auto hr = context->EndDraw();

	// Check for error but avoid handling a device loss because that situation is handled by
	// the next call to Present().
	if (hr != D2DERR_RECREATE_TARGET)
		ThrowIfFailed(hr);

	context->SetTarget(prevTarget.Get());
}

void Game::clear_bitmap(Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap, ColorF color)
{
	auto context = m_devrsrc->GetD2DContext();

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

void Game::Clear()
{
	auto context = m_devrsrc->GetD2DContext();
	context->Clear(ColorF(ColorF::Blue));
}

void Game::OnDeviceLoss()
{
	m_whiteBrush.Reset();
	m_blackBrush.Reset();
	m_blueBrush.Reset();

	m_builtInEffect.Reset();
	m_customBlurEffect.Reset();
	m_customDropShadowEffect.Reset();
	m_psEffect.Reset();

	m_bitmap1.Reset();
	m_bitmap2.Reset();
	m_bitmap3.Reset();
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
	auto sz = m_devrsrc->GetBackBufSize();
	m_devrsrc->HandleResize(sz.x, sz.y);
}

void Game::OnWindowResize(size_t w, size_t h)
{
	m_devrsrc->HandleResize(w, h);
	this->cwsdr();
}
