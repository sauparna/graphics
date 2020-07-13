// A short example demostrating how to get to the point of drawing a line in a window
// using Direct2D. We don't create any DirectX11 device, but make use of a factory,
// render target and a brush (ID2D1Factory, ID2D1HwndRenderTarget and ID2D1SolidColorBrush).

#include <windows.h>

#include <d2d1.h>
#include <d2d1helper.h>

#include <string>

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif // DEBUG || _DEBUG
#endif // Assert

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != nullptr)
	{
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = nullptr;
	}
}

static inline void PrintLastError()
{
	wchar_t buf[512];
	size_t size = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
	std::wstring msg(buf, size);
	OutputDebugString((LPCWSTR)msg.c_str());
}

class D2DDrawingSurface
{
public:
	D2DDrawingSurface();
	D2DDrawingSurface(float w, float h);
	~D2DDrawingSurface();
	HRESULT Initialize(HINSTANCE hInstance);
	void	RunMessageLoop();
	void	keypress(WPARAM wParam, LPARAM lParam);

private:
	HRESULT cdir();
	HRESULT cddr();
	void	dddr();
	HRESULT render();
	void	resize(UINT w, UINT h);

	// A custom wndproc and a WndProc for Win32 using an appropriate calling convention
	LRESULT wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND hWnd_;
	ID2D1Factory* d2d_factory_;
	ID2D1HwndRenderTarget* d2d_render_target_;
	ID2D1SolidColorBrush* d2d_gray_brush_;
	float w_, h_;
};

D2DDrawingSurface::D2DDrawingSurface() :
	hWnd_{ nullptr }, d2d_factory_{ nullptr }, d2d_render_target_{ nullptr }, d2d_gray_brush_{ nullptr },
	w_{ 640.f }, h_{ 480.f }
{}

D2DDrawingSurface::D2DDrawingSurface(float w, float h) :
	hWnd_{ nullptr }, d2d_factory_{ nullptr }, d2d_render_target_{ nullptr }, d2d_gray_brush_{ nullptr },
	w_{ w }, h_{ h }
{}

D2DDrawingSurface::~D2DDrawingSurface()
{
	SafeRelease(&d2d_factory_);
	SafeRelease(&d2d_render_target_);
	SafeRelease(&d2d_gray_brush_);
}

void D2DDrawingSurface::RunMessageLoop()
{
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT D2DDrawingSurface::Initialize(HINSTANCE hInstance)
{
	const wchar_t CLASS_NAME[] = L"D2DDrawingSurface";
	HRESULT hr;
	hr = cdir();
	if (!SUCCEEDED(hr)) return hr;

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= nullptr;
	wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= CLASS_NAME;
	wc.hIconSm			= nullptr;

	if (!RegisterClassEx(&wc)) hr = E_FAIL;
	if (!SUCCEEDED(hr)) return hr;

	FLOAT dpi_x, dpi_y;
	// 1D2D1Factory::GetDesktopDpi is deprecated. Recommendation is DisplayInformation::LogicalDpi
	// for Windows Store Apps ro GetDpiForWindow for desktop apps. However, in this program, we need
	// the dpi information to create the Window handle and the recommended function needs the Window
	// handle (type HWND) to get the DPI.
	#pragma warning(suppress: 4996)
	d2d_factory_->GetDesktopDpi(&dpi_x, &dpi_y);
	UINT w = static_cast<UINT>(ceil(w_ * dpi_x / 96.f));
	UINT h = static_cast<UINT>(ceil(h_ * dpi_y / 96.f));

	hWnd_ = CreateWindowEx(
		0,						// Optional window styles.
		CLASS_NAME,				// Window class name.
		L"Direct2D Surface",	// Window text.
		WS_OVERLAPPEDWINDOW,	// Window style.
		CW_USEDEFAULT, CW_USEDEFAULT, w, h, // Size and position.
		nullptr,				// Parent window.
		nullptr,				// Menu.
		hInstance,				// Instance handle.
		this					// Additional application data; pointer to calling object.
	);
	hr = hWnd_ ? S_OK : E_FAIL;
	if (!SUCCEEDED(hr)) return hr;
	ShowWindow(hWnd_, SW_SHOWNORMAL);
	UpdateWindow(hWnd_);
	return hr;
}

HRESULT D2DDrawingSurface::cdir()
{
	HRESULT hr = S_OK;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory_);
	return hr;
}

HRESULT D2DDrawingSurface::cddr()
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect(hWnd_, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	hr = d2d_factory_->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd_, size),
		&d2d_render_target_
	);
	if (!SUCCEEDED(hr)) return hr;
	hr = d2d_render_target_->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Gray),
		&d2d_gray_brush_
	);
	return hr;
}

void D2DDrawingSurface::dddr()
{
	SafeRelease(&d2d_render_target_);
	SafeRelease(&d2d_gray_brush_);
}

LRESULT D2DDrawingSurface::wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
	{
		UINT w = LOWORD(lParam);
		UINT h = HIWORD(lParam);
		this->resize(w, h);
	}
	break;
	case WM_DISPLAYCHANGE:
	{
		InvalidateRect(hWnd, nullptr, FALSE);
	}
	break;
	case WM_KEYDOWN:
	{
		keypress(wParam, lParam);
	}
	break;
	case WM_PAINT:
	{
		this->render();
		ValidateRect(hWnd, nullptr);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK D2DDrawingSurface::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	D2DDrawingSurface* d2d_surface = nullptr;
	if (uMsg == WM_CREATE)
	{
		// Pass the 'this' pointer by calling CreateWindowEx and put it in the instance data for the window.
		d2d_surface = reinterpret_cast<D2DDrawingSurface*>((reinterpret_cast<CREATESTRUCT*>(lParam))->lpCreateParams);
		::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(d2d_surface));
	}
	else
	{
		d2d_surface = reinterpret_cast<D2DDrawingSurface*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}
	if (d2d_surface)
		return d2d_surface->wndproc(hWnd, uMsg, wParam, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HRESULT D2DDrawingSurface::render()
{
	HRESULT hr = S_OK;
	hr = cddr();
	if (!SUCCEEDED(hr)) return hr;

	d2d_render_target_->BeginDraw();
	d2d_render_target_->SetTransform(D2D1::Matrix3x2F::Identity());
	d2d_render_target_->Clear(D2D1::ColorF(D2D1::ColorF::White));
	D2D1_SIZE_F rt_size = d2d_render_target_->GetSize();
	auto line_start_point = D2D1::Point2F(0.f, 0.f);
	auto line_end_point = D2D1::Point2F(static_cast<int>(rt_size.width), static_cast<int>(rt_size.height));
	d2d_render_target_->DrawLine(line_start_point, line_end_point, d2d_gray_brush_, 1.5f);
	hr = d2d_render_target_->EndDraw();

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		dddr();
	}
	return hr;
}

void D2DDrawingSurface::resize(UINT w, UINT h)
{
	if (!d2d_render_target_) return;
	d2d_render_target_->Resize(D2D1::SizeU(w, h));
}

void D2DDrawingSurface::keypress(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_ESCAPE)
		PostMessageA(hWnd_, WM_CLOSE, 0, 0);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
	if (SUCCEEDED(CoInitialize(nullptr)))
	{
		{
			D2DDrawingSurface d2d_surface{ 800.f, 600.f };
			if (SUCCEEDED(d2d_surface.Initialize(hInstance)))
			{
				d2d_surface.RunMessageLoop();
			}
		}
		CoUninitialize();
	}
	return 0;
}
