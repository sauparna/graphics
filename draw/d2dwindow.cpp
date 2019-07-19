#include "draw_pch.h"
#include "kwindow.h"
#include "d2dwindow.h"
#include "devrsrc.h"
#include "game.h"

using namespace DirectX;
using namespace DX;
using Microsoft::WRL::ComPtr;
using namespace D2D1;

KD2DWindow::~KD2DWindow()
{
	game.reset();
}

void KD2DWindow::Initialize(HINSTANCE hInstance)
{
	KWindow::Initalize(hInstance);
	game->Initialize(hWnd);
}

void KD2DWindow::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_ESCAPE)
		PostMessageA(hWnd, WM_CLOSE, 0, 0);
}

LRESULT KD2DWindow::KWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;

	switch (uMsg)
	{
	case WM_PAINT:
		if (game)
		{
			game->Tick();
		}
		else
		{
			hDC = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_MOVE:
		if (game)
		{
			game->OnWindowMove();
		}
		break;
	case WM_SIZE:
		if (game)
		{
			game->OnWindowResize(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

WPARAM KD2DWindow::KMessageLoop()
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
		{
			game->Tick();
		}
	}

	return msg.wParam;
}
