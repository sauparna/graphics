//----------------------------------------------------------------------------------------------------
// This is a vanilla Win32 program that demonstrates the basics of creating a window for your program.
// Most of it was picked from Microsoft Docs, here:
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/learn-to-program-for-windows
// The C++ version was written from the "Managing Application State" section of the same document.
//----------------------------------------------------------------------------------------------------

#include <windows.h>
#include "win.h"
#include "winnew.h"

const int WND_X = 400;
const int WND_Y = 300;
const int WND_W = 300;
const int WND_H = 200;

// Use the macros to switch on/off two different entry points: one calls plain C code, the other call the code that
// uses C++ classes to wrap Win32 API in order to keep track of program state.

#define WINOO // object oriented; wrapped a in C++ class
//#define WIN // vanilla; C functions

#ifdef WINOO

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	NewWindow winnew;
	if (!winnew.create(L"New Window", WS_OVERLAPPEDWINDOW, WND_X, WND_Y, WND_W, WND_H))
		return 0;
	ShowWindow(winnew.hwnd(), nCmdShow);
	winnew.msg_loop();
	return 0;
}

#endif // WINOO

#ifdef WIN

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	//----------------------------------------------------------------------------------------------------
	// Register the window class
	//----------------------------------------------------------------------------------------------------

	const wchar_t CLASS_NAME[] = L"Reference Win32 Class";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	//----------------------------------------------------------------------------------------------------
	// Initialize new state information object.
	//----------------------------------------------------------------------------------------------------
	
	StateInfo* pState = new (std::nothrow) StateInfo;
	if (pState == nullptr) return 0;

	//----------------------------------------------------------------------------------------------------
	// Create the window
	//----------------------------------------------------------------------------------------------------

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

	//----------------------------------------------------------------------------------------------------
	// Run the message loop
	// DispatchMessage indirectly causes Windows to invoke WndProc, once for each message.
	//----------------------------------------------------------------------------------------------------

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

#endif // WIN
