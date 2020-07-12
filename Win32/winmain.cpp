//----------------------------------------------------------------------------------------------------
// This is a vanilla Win32 program that demonstrates the basics of creating a window for your program.
// Most of it was picked from Microsoft Docs, here:
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/learn-to-program-for-windows
//----------------------------------------------------------------------------------------------------

#include <windows.h>
#include <string>
#include <exception>

const int WND_X = 400;
const int WND_Y = 300;
const int WND_W = 300;
const int WND_H = 200;

struct StateInfo
{
	std::wstring s = L"Hello world!";
	RECT box{10, 10, 100, 100};
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class ------------------------------------------------

	const wchar_t CLASS_NAME[] = L"Reference Win32 Class";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	// Initialize new state information object.
	StateInfo* pState = new (std::nothrow) StateInfo;
	if (pState == nullptr) return 0;

	// Create the window --------------------------------------------------------

	HWND hWnd = CreateWindowEx(
		0,						// Optional window styles
		CLASS_NAME,				// Window class
		L"Win32 Reference",		// Window text
		WS_OVERLAPPEDWINDOW,	// Window style
		WND_X, WND_Y,			// Position.
		WND_W, WND_H,			// Size.
		NULL,					// Parent window
		NULL,					// Menu
		hInstance,				// Instance handle
		pState					// Additional application data; a pointer to a StateInfo object
	);
	if (hWnd == NULL) return 0;
	ShowWindow(hWnd, nCmdShow);

	// Run the message loop -----------------------------------------------------
	// DispatchMessage indirectly causes Windows to invoke WndProc, once for each message.

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	StateInfo* pState = nullptr;
	if (uMsg == WM_CREATE)
	{
		// Get the state object and put it in the instance data for the window.
		pState = reinterpret_cast<StateInfo*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pState);
	}
	else
		pState = reinterpret_cast<StateInfo*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		if (MessageBox(hWnd, L"Really quit?", L"Win32 Reference", MB_OKCANCEL) == IDOK)
			DestroyWindow(hWnd);
	}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hWnd, &ps);
		FillRect(hDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		DrawText(hDC, (LPCWSTR)pState->s.c_str(), pState->s.length(), (LPRECT)&(pState->box), DT_CENTER);
		EndPaint(hWnd, &ps);
	}
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
