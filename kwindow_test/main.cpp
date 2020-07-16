#include "kwindowtest_pch.h"
#include "ktestwindow.h"
#include "ktimer.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nCmdShow)
{
	auto win = std::make_unique<KTestWindow>(640, 480, L"Test");
	if (win->KCreateWindow(100, 100, WS_OVERLAPPEDWINDOW) == true)
	{
		win->KShowWindow(nCmdShow);
		win->KUpdateWindow();
		auto ret = win->KMessageLoop();
		win.reset();
		return static_cast<int>(ret);
	}
	else
	{
		MessageBox(NULL, L"K_ERROR: KCreateWindow failed.", L"K_ERROR", MB_OK);
	}
	return 0;
}
