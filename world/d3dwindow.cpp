#include "world_pch.h"
#include "kwindow.h"
#include "d3dwindow.h"
#include "devrsrc.h"
#include "game.h"

using namespace DirectX;
using namespace DX;
using Microsoft::WRL::ComPtr;

KD3DWindow::KD3DWindow(SIZE sz, std::string wndname) noexcept :
	KWindow{ sz, wndname },
	game{ std::make_unique<Game>(DirectX::XMUINT2{static_cast<uint32_t>(sz.cx), static_cast<uint32_t>(sz.cy)}) }
{}

KD3DWindow::~KD3DWindow()
{
	game.reset();
}

void KD3DWindow::Initialize(HINSTANCE hInstance)
{
	KWindow::Initalize(hInstance);
	game->Initialize(hWnd);
}

void KD3DWindow::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_ESCAPE)
		PostMessageA(hWnd, WM_CLOSE, 0, 0);
}

LRESULT KD3DWindow::KWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps{};
	HDC hdc;

	static bool s_in_sizemove = false;
	static bool s_in_suspend = false;
	static bool s_minimized = false;
	static bool s_fullscreen = false;

	switch (message)
	{
#ifdef DXTK_AUDIO
	case WM_CREATE:
		if (!g_hNewAudio)
		{
			// Ask for notification of new audio devices.
			DEV_BROADCAST_DEVICEINTERFACE filter = {};
			filter.dbcc_size = sizeof(filter);
			filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			filter.dbcc_classguid = KSCATEGORY_AUDIO;
			g_hNewAudio = RegisterDeviceNotification(hWnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
		}
		break;

	case WM_CLOSE:
		if (g_hNewAudio)
		{
			UnregisterDeviceNotification(g_hNewAudio);
			g_hNewAudio = nullptr;
		}
		DestroyWindow(hWnd);
		break;

	case WM_DEVICECHANGE:
		switch (wParam)
		{
		case DBT_DEVICEARRIVAL:
			auto pDev = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
			if (pDev)
			{
				if (pDev->dbch_devicetype = DBT_DEVTYP_DEVICEINTERFACE)
				{
					auto pInter = reinterpret_cast<const PDEV_BROADCAST_DEVICEINTERFACE>(pDev);
					if (pInter->dbcc_classguid == KSCATEGORY_AUDIO)
					{
						if (g_game)
							g_game->NewAudioDevice();
					}
				}
			}
		}
		break;

	case DBT_DEVICEREMOVECOMPLETE:
	{
		auto pDev = reinterpret_cast<PDEV_BROADCAST>(lParam);
		if (pDev)
		{
			if (pDev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
			{
				auto pInter = reinterpret_cast<const PDEV_BROADCAST_DEVICEINTERFACE>(pDev);
				if (pInter->dbcc_classguid == KSCATEGORY_AUDIO)
				{
					if (g_game)
						g_game->NewAudioDevice();
				}
			}
		}
	}
	break;
	}
	return 0;
#endif
	case WM_PAINT:
		if (s_in_sizemove && game)
			game->Tick();
		else
		{
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_MOVE:
		if (game)
			game->OnWindowMove();
		break;
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			if (!s_minimized)
			{
				s_minimized = true;
				if (!s_in_suspend && game)
					game->OnSuspend();
				s_in_suspend = true;
			}
		}
		else if (s_minimized)
		{
			s_minimized = false;
			if (s_in_suspend && game)
				game->OnResume();
			s_in_suspend = false;
		}
		else if (!s_in_sizemove && game)
			game->OnWindowResize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ENTERSIZEMOVE:
		s_in_sizemove = true;
		break;
	case WM_EXITSIZEMOVE:
		s_in_sizemove = false;
		if (game)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			game->OnWindowResize(rc.right - rc.left, rc.bottom - rc.top);
		}
		break;
	case WM_GETMINMAXINFO:
		{
			auto info = reinterpret_cast<MINMAXINFO*>(lParam);
			info->ptMinTrackSize.x = 320;
			info->ptMinTrackSize.y = 200;
		}
		break;
	case WM_ACTIVATEAPP:
		if (game)
		{
			if (wParam)
				game->OnActivate();
			else
				game->OnDeactivate();
		}
		break;
	case WM_POWERBROADCAST:
		switch (wParam)
		{
		case PBT_APMQUERYSUSPEND:
			if (!s_in_suspend && game)
				game->OnSuspend();
			s_in_suspend = true;
			return TRUE;
		case PBT_APMRESUMESUSPEND:
			if (!s_minimized)
			{
				if (s_in_suspend && game)
					game->OnResume();
				s_in_suspend = false;
			}
			return TRUE;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// ALT+ENTER fullscreen toggle.
			if (s_fullscreen)
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);
				int w = 800;
				int h = 600;
				if (game)
				{
					XMUINT2 back_buffer_size = game->device_back_buffer_size();
					w = back_buffer_size.x;
					h = back_buffer_size.y;
				}
				ShowWindow(hWnd, SW_SHOWNORMAL);
				SetWindowPos(hWnd, HWND_TOP, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			else
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, 0);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);
				SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
				ShowWindow(hWnd, SW_SHOWMAXIMIZED);
			}
			s_fullscreen = !s_fullscreen;
		}
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	case WM_MENUCHAR:
		// A menu is active and a subsequent keypress doesn't correspond to any mnemonic or accelerator
		// key, so ignore to avoid producing a beep.
		return MAKELRESULT(0, MNC_CLOSE);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

WPARAM KD3DWindow::KMessageLoop()
{
	MSG msg{};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			game->Tick();
	}
	return msg.wParam;
}
