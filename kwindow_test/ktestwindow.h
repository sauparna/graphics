#pragma once

#include "kwindow.h"

class KTestWindow : public KWindow
{
public:
	KTestWindow(SIZE sz, std::string wndname) : KWindow{ sz, wndname } {}

private:
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
};
