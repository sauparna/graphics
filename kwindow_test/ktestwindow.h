#pragma once

#include "kwindow.h"

class KTestWindow : public KWindow<KTestWindow>
{
public:
	KTestWindow(LONG w, LONG h, PCWSTR wndname) : KWindow{ w, h, wndname } {}

private:
	PCWSTR classname() { return L"KTestWindow"; }
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
};
