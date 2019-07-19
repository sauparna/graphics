#pragma once

#include "devrsrc.h"
#include "game.h"

class KD2DWindow : public KWindow
{
private:
	std::unique_ptr<Game> game;
	LRESULT KWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	KD2DWindow(SIZE sz, std::string wndname) noexcept :
		KWindow{ sz, wndname },
		game{ std::make_unique<Game>(DirectX::XMUINT2{static_cast<uint32_t>(sz.cx), static_cast<uint32_t>(sz.cy)}) }
	{}
	~KD2DWindow();
	void Initialize(HINSTANCE hInstance);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	WPARAM KMessageLoop();
};
