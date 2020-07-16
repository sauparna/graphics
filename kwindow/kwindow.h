#pragma once

// INVARIANT: Call to GetLastError() is the first line after a Win32 API function call.
static inline void PrintLastError(std::wstring wstr_func_name)
{
	DWORD dwMessageId = GetLastError();
	PWSTR wbuf = nullptr;
	std::wstring err_str{};
	size_t sz = 0;
	if ((sz = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwMessageId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<PWSTR>(&wbuf),
		512,
		NULL)) == 0)
	{
		DWORD msg_id = GetLastError();
		err_str = L"(K_ERROR: Missing error string because error string construction function 'FormatMessage' failed with Win32 API error code "
			+ std::to_wstring(msg_id)
			+ L")\n";
	}
	else
	{
		err_str = std::wstring(wbuf, sz);
	}
	std::wstring ws = L"Failed function: "
		+ wstr_func_name
		+ L"\nError "
		+ std::to_wstring(dwMessageId)
		+ L": "
		+ err_str;
	OutputDebugString(ws.c_str());
};

#if defined( _DEBUG ) || defined ( DEBUG )
	#define PRINT_LAST_ERROR(str) PrintLastError(str)
#else
	#define PRINT_LAST_ERROR(str) ((void)0)
#endif // _DEBUG

template <class DERIVED_TYPE>
class KWindow
{
public:
	KWindow(LONG w, LONG h, PCWSTR wndname) : size_{ w, h }, wndname_{ wndname } {}
	virtual			~KWindow() {}
	virtual bool	KCreateWindow(
			int		x			= CW_USEDEFAULT,
			int		y			= CW_USEDEFAULT,
			DWORD	dwStyle		= WS_OVERLAPPEDWINDOW,
			DWORD	dwExStyle	= 0,
			HWND	hWndParent	= NULL,
			HMENU	hMenu		= NULL
	);
	virtual WPARAM	KMessageLoop();
			BOOL	KShowWindow(int nCmdShow) const { return ::ShowWindow(hwnd_, nCmdShow); }
			BOOL	KUpdateWindow() const { return ::UpdateWindow(hwnd_); }
			HWND	hwnd() { return hwnd_; }
			SIZE	size() { return size_; }

protected:
			PCWSTR	name() { return wndname_; }
	virtual PCWSTR	classname() { return L"KWindow"; }
	virtual void	OnDraw(HDC hDC) {}
	virtual void	OnKeyDown(WPARAM wParam, LPARAM lParam) {}
	virtual LRESULT	KWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

			SIZE		size_		{ CW_USEDEFAULT, CW_USEDEFAULT };
			PCWSTR		wndname_	{ L"unnamed" };
			PCWSTR		classname_	{ L"unnamed" };
			HINSTANCE	hInstance_	{ NULL };
			HWND		hwnd_		{ NULL };
};

// INVARIANT: Can not specify WS_OVERLAPPED style because AdjustWindowRect, that is called
// subsequently, does not take that as a parameter (See API documentation for AdjustWindowRect). 
template <class DERIVED_TYPE>
bool KWindow<DERIVED_TYPE>::KCreateWindow(
	int x,
	int y,
	DWORD dwStyle,
	DWORD dwExStyle,
	HWND hWndParent,
	HMENU hMenu
)
{
	RECT wnd_rect_ { 0, 0, size_.cx, size_.cy };
	if (AdjustWindowRect(&wnd_rect_, dwStyle, FALSE) == FALSE)
	{
		PRINT_LAST_ERROR(L"AdjustWindowRect");
		OutputDebugString(L"K_WARN: Continuing with smaller client area, entire window size set to client area requested by user.\n");
	}

	classname_ = classname();

	if ((hInstance_ = GetModuleHandle(NULL)) == NULL)
	{
		PRINT_LAST_ERROR(L"GetModuleHandle");
		return false;
	}

	WNDCLASS wc{};
	wc.lpfnWndProc	 = DERIVED_TYPE::WndProc;
	wc.hInstance	 = hInstance_;
	wc.lpszClassName = classname();

	if (!RegisterClass(&wc))
	{
		PRINT_LAST_ERROR(L"RegisterClass");
		return false;
	}

	if ((hwnd_ = CreateWindowEx(
		dwExStyle,
		classname_,
		wndname_,
		dwStyle,
		x, y,
		wnd_rect_.right - wnd_rect_.left,
		wnd_rect_.bottom - wnd_rect_.top,
		hWndParent,
		hMenu,
		hInstance_,
		reinterpret_cast<void*>(this))) == NULL)
	{
		PRINT_LAST_ERROR(L"CreateWindowEx");
		return false;
	}
	return true;
}

template <class DERIVED_TYPE>
LRESULT KWindow<DERIVED_TYPE>::KWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		OnDraw(ps.hdc);
		EndPaint(hWnd, &ps);
	}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

template <class DERIVED_TYPE>
LRESULT CALLBACK KWindow<DERIVED_TYPE>::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DERIVED_TYPE* pThis;

	if (uMsg == WM_NCCREATE)
	{
		pThis = reinterpret_cast<DERIVED_TYPE*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->hwnd_ = hWnd;
	}
	else
		pThis = reinterpret_cast<DERIVED_TYPE*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (pThis)
	{
		return pThis->KWndProc(hWnd, uMsg, wParam, lParam);
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

template <class DERIVED_TYPE>
WPARAM KWindow<DERIVED_TYPE>::KMessageLoop()
{
	MSG msg{};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}
