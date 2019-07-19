#pragma once

#include "devrsrc.h"
#include "game.h"

class KD3DWindow : public KWindow
{
public:
	KD3DWindow(SIZE sz, std::string wndname) noexcept;
	~KD3DWindow();
	void Initialize(HINSTANCE hInstance);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	WPARAM KMessageLoop();

private:
	std::unique_ptr<Game> game;
	LRESULT KWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
