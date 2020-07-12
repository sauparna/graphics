#include "kwindow_pch.h"
#include "kwindow.h"

void KWindow::Initalize(HINSTANCE hInstance)
{
	RECT rc = { 0, 0, static_cast<LONG>(sz.cx), static_cast<LONG>(sz.cy) };
	AdjustWindowRect(&rc, WS_TILEDWINDOW, FALSE);
	wndsz = SIZE{ rc.right - rc.left, rc.bottom - rc.top };
	KCreateWindow(0, "KWindow", wndname.c_str(), WS_TILEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, wndsz.cx, wndsz.cy, nullptr, nullptr, hInstance);
}

LRESULT KWindow::KWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		OnDraw(ps.hdc);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK KWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	KWindow* pWindow;

	if (uMsg == WM_NCCREATE)
	{
		assert(!IsBadReadPtr((void*)lParam, sizeof(CREATESTRUCT)));
		MDICREATESTRUCT* pMDIC = reinterpret_cast<MDICREATESTRUCT*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		pWindow = reinterpret_cast<KWindow*>(pMDIC->lParam);
		assert(!IsBadReadPtr(pWindow, sizeof(KWindow)));
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
	}
	else
		pWindow = reinterpret_cast<KWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (pWindow)
		return pWindow->KWndProc(hWnd, uMsg, wParam, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool KWindow::KRegisterClass(HINSTANCE hInstance, LPCSTR lpszClassName)
{
	WNDCLASSEX wc;

	if (!GetClassInfoEx(hInstance, lpszClassName, &wc))
	{
		KSetClass(wc);
		wc.hInstance = hInstance;
		wc.lpszClassName = lpszClassName;
		if (!RegisterClassEx(&wc))
			return false;
	}

	return true;
}

bool KWindow::KCreateWindow(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
	int x, int y, int w, int h, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance)
{
	if (!KRegisterClass(hInstance, lpClassName))
		return false;

	// Use MDICREATESTRUCT to pass the 'this' pointer.
	MDICREATESTRUCT mdic{};
	mdic.lParam = reinterpret_cast<LPARAM>(this);

	hWnd = CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle,
		x, y, w, h, hWndParent, hMenu, hInstance, &mdic);

	return hWnd != nullptr;
}

void KWindow::KSetClass(WNDCLASSEX& wc)
{
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = nullptr;
	wc.hIcon		 = nullptr;
	wc.hCursor		 = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName	 = nullptr;
	wc.lpszClassName = nullptr;
	wc.hIconSm		 = nullptr;
}

WPARAM KWindow::KMessageLoop()
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
	return msg.wParam;
}
