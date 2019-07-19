#include "kwindowtest_pch.h"
#include "ktestwindow.h"
#include "ktimer.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	auto win = std::make_unique<KTestWindow>(SIZE{ 640, 480 }, "test");
	KTimer t;
	t.get_time();
	win->Initalize(hInstance);
	win->KShowWindow(nCmdShow);
	win->KUpdateWindow();
	t.get_time();

	std::string msg = "KWindow initialization took " + std::to_string(t.elapsed(1E3)) + " ms\n";
	msg += "Timer overhead was " + std::to_string(t.get_overhead(1E3)) + " ms\n";
	MessageBox(NULL, msg.c_str(), "Timing info", MB_OK);

	auto ret = win->KMessageLoop();

	win.reset();

	return static_cast<int>(ret);
}
