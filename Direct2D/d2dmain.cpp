// A short example demostrating how to get to the point of drawing a line in a window
// using Direct2D. We don't create any DirectX11 device, but make use of a factory,
// render target and a brush (ID2D1Factory, ID2D1HwndRenderTarget and ID2D1SolidColorBrush).

#pragma comment(lib, "d2d1")

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <string>

static inline void PrintLastError()
{
	PWSTR wbuf = nullptr;
	size_t size = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<PWSTR>(&wbuf),
		512,
		NULL
	);
	OutputDebugString(wbuf);
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#define PRINT_LAST_ERROR() PrintLastError()
#else
#define Assert(b)
#define PRINT_LAST_ERROR() ((void)0)
#endif // DEBUG || _DEBUG
#endif // Assert

template<class T>
inline void SafeRelease(T** ppT)
{
	if (*ppT != nullptr)
	{
		(*ppT)->Release();
		(*ppT) = nullptr;
	}
}

class D2DDrawingSurface
{
public:
	D2DDrawingSurface(float w, float h);
	~D2DDrawingSurface();
	bool	Initialize();
	void	RunMessageLoop();
	void	keypress(WPARAM wParam, LPARAM lParam);

private:
	bool	cdir();
	bool	cddr();
	void	dddr();
	bool	render();
	void	resize(UINT w, UINT h);
	LRESULT wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND hWnd_{ NULL };
	HINSTANCE hInstance_{ NULL };
	ID2D1Factory* d2d_factory_{};
	ID2D1HwndRenderTarget* d2d_render_target_{};
	ID2D1SolidColorBrush* d2d_gray_brush_{};
	float w_{ 640.f }, h_{ 480.f };
};

D2DDrawingSurface::D2DDrawingSurface(float w, float h) : w_{ w }, h_{ h } {}

D2DDrawingSurface::~D2DDrawingSurface()
{
	SafeRelease(&d2d_factory_);
	dddr();
}

void D2DDrawingSurface::RunMessageLoop()
{
	MSG msg{};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

bool D2DDrawingSurface::Initialize()
{
	if (!cdir()) return false;

	if ((hInstance_ = GetModuleHandle(NULL)) == NULL)
	{
		PRINT_LAST_ERROR();
		return false;
	}

	const wchar_t CLASS_NAME[] = L"D2DDrawingSurface";
	WNDCLASSEX wc{};
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= D2DDrawingSurface::WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance_;
	wc.hIcon			= nullptr;
	wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= CLASS_NAME;
	wc.hIconSm			= nullptr;

	if (!RegisterClassEx(&wc))
	{
		PRINT_LAST_ERROR();
		return false;
	}

	if ((hWnd_ = CreateWindowEx(
		0,						// Optional window styles.
		CLASS_NAME,				// Window class name.
		L"Direct2D Surface",	// Window text.
		WS_OVERLAPPEDWINDOW,	// Window style.
		CW_USEDEFAULT,			// Position (x, y)
		CW_USEDEFAULT,
		static_cast<int>(w_),	// Size (width, height)
		static_cast<int>(h_),
		nullptr,				// Parent window.
		nullptr,				// Menu.
		hInstance_,				// Instance handle.
		this					// Additional application data; pointer to calling object.
	)) == NULL)
	{
		PRINT_LAST_ERROR();
		return false;
	}

	if (!cddr()) return false;

	ShowWindow(hWnd_, SW_SHOWNORMAL);

	if (!UpdateWindow(hWnd_))
	{
		PRINT_LAST_ERROR();
	}

	return true;
}

bool D2DDrawingSurface::cdir()
{
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory_);
	if (FAILED(hr))
		return false;
	return true;
}

bool D2DDrawingSurface::cddr()
{
	RECT rc;
	GetClientRect(hWnd_, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	if (d2d_render_target_) return true;

	HRESULT hr = S_OK;

	hr = d2d_factory_->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd_, size),
		&d2d_render_target_
	);
	if (FAILED(hr)) return false;

	hr = d2d_render_target_->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Gray),
		&d2d_gray_brush_
	);
	if (FAILED(hr)) return false;

	return true;
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

bool D2DDrawingSurface::render()
{
	if(!cddr()) return false;
	if (!d2d_render_target_)
		return false;
	d2d_render_target_->BeginDraw();
	d2d_render_target_->SetTransform(D2D1::Matrix3x2F::Identity());
	d2d_render_target_->Clear(D2D1::ColorF(D2D1::ColorF::White));
	D2D1_SIZE_F rt_size		= d2d_render_target_->GetSize();
	auto line_start_point	= D2D1::Point2F(0.f, 0.f);
	auto line_end_point		= D2D1::Point2F(static_cast<int>(rt_size.width), static_cast<int>(rt_size.height));
	d2d_render_target_->DrawLine(line_start_point, line_end_point, d2d_gray_brush_, 1.5f);
	HRESULT hr = d2d_render_target_->EndDraw();

	if (hr == D2DERR_RECREATE_TARGET)
	{
		dddr();
		if (!cddr()) return false;
	}
	return true;
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

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
	if (SUCCEEDED(CoInitialize(nullptr)))
	{
		{
			D2DDrawingSurface d2d_surface{ 800.f, 600.f };
			if (d2d_surface.Initialize())
			{
				d2d_surface.RunMessageLoop();
			}
		}
		CoUninitialize();
	}
	return 0;
}
