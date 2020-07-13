#include "winnew.h"

LRESULT NewWindow::wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hwnd_, &ps);
        FillRect(hDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        DrawText(hDC, (LPCWSTR)this->state_info.s.c_str(), this->state_info.s.length(), (LPRECT) & (this->state_info.box), DT_CENTER);
        EndPaint(hwnd_, &ps);
    }
    return 0;
    }
    return DefWindowProc(hwnd_, uMsg, wParam, lParam);
}
