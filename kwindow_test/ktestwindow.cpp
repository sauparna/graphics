#include "kwindowtest_pch.h"
#include "ktestwindow.h"

void KTestWindow::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_ESCAPE)
		PostMessage(hwnd_, WM_DESTROY, 0, 0);
}
