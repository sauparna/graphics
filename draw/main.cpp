#include "draw_pch.h"
#include "kwindow.h"
#include "d2dwindow.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	if(FAILED(CoInitialize(nullptr)))
		return 1;
	auto win = std::make_unique<KD2DWindow>(SIZE{ 800, 600 }, "Direct2D");
	win->Initialize(hInstance);
	win->KShowWindow(nCmdShow);
	win->KUpdateWindow();
	auto ret = win->KMessageLoop();
	win.reset();
	CoUninitialize();
	return static_cast<int>(ret);
}
