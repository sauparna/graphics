#include "kwindowtest_pch.h"
#include "ktestwindow.h"

void KTestWindow::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_ESCAPE)
		PostMessageA(hWnd, WM_CLOSE, 0, 0);
}
