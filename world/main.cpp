#include "world_pch.h"
#include "kwindow.h"
#include "d3dwindow.h"
#include "game.h"

#ifdef DXTK_AUDIO
#include <Dbt.h>
#endif

using namespace DirectX;

namespace
{
	//std::unique_ptr<Game> g_game;
#ifdef DXTK_AUDIO
	HDEVNOTIFY g_hNewAudio = nullptr;
#endif
};

// Indicates to hybrid graphics systems to prefer the discrete part by default.
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!XMVerifyCPUSupport())
		return 1;

	auto hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return 1;

	auto win = std::make_unique<KD3DWindow>(SIZE{ 800, 600 }, "Direct3D");

	win->Initialize(hInstance);
	win->KShowWindow(nCmdShow);
	win->KUpdateWindow();

	auto ret = win->KMessageLoop();

	win.reset();
	CoUninitialize();

	return static_cast<int>(ret);
}

void ExitGame()
{
	PostQuitMessage(0);
}
