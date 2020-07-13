#include "win.h"

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
		DrawText(hDC, (LPCWSTR)pState->s.c_str(), pState->s.length(), (LPRECT) & (pState->box), DT_CENTER);
		EndPaint(hWnd, &ps);
	}
	return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
