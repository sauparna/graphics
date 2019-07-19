#pragma once

class KWindow
{
public:

	KWindow(SIZE sz, std::string wndname)
		: hWnd{ nullptr }, sz{ sz }, wndsz{ sz }, wndname{ wndname } {}

	virtual ~KWindow() {}
	virtual void Initalize(HINSTANCE hInstance);
	virtual bool KCreateWindow(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
		int x, int y, int w, int h, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance);
	virtual WPARAM KMessageLoop();

	bool KRegisterClass(HINSTANCE hInstance, LPCSTR lpszClassName);

	BOOL KShowWindow(int nCmdShow) const { return ::ShowWindow(hWnd, nCmdShow); }
	BOOL KUpdateWindow() const { return ::UpdateWindow(hWnd); }

	SIZE KGetSize() { return sz; }
	SIZE KGetWndSize() { return sz; }
	HWND KGetWnd() { return hWnd; }

	HWND hWnd;
	SIZE sz{ 400, 300 };

private:

	virtual void	OnDraw(HDC hDC) {}
	virtual void	OnKeyDown(WPARAM wParam, LPARAM lParam) {}
	virtual void	KSetClass(WNDCLASSEX& wc);

	virtual LRESULT			KWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	SIZE wndsz;
	std::string wndname{ "unnamed" };
};
