#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <exception>

static inline void PrintLastError()
{
    PWSTR wbuf = nullptr;
    size_t size = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<PWSTR>(&wbuf),
        512,
        NULL
    );
    OutputDebugString(wbuf);
}

#if defined( _DEBUG ) || defined ( DEBUG )
    #define PRINT_LAST_ERROR() PrintLastError()
#else
    #define PRINT_LAST_ERROR() ((void)0)
#endif // _DEBUG


template <class DERIVED_TYPE>
class BaseWindow
{
public:
    BaseWindow() : hwnd_{ NULL }, hInstance_{ NULL } {}
    HWND hwnd() const { return hwnd_; }
    BOOL create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        int x           = CW_USEDEFAULT,
        int y           = CW_USEDEFAULT,
        int w           = CW_USEDEFAULT,
        int h           = CW_USEDEFAULT,
        DWORD dwExStyle = 0,
        HWND hWndParent = NULL,
        HMENU hMenu     = NULL
    );
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual WPARAM msg_loop();

protected:
    virtual PCWSTR class_name() const = 0;
    virtual LRESULT wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    HWND hwnd_{ NULL };
    HINSTANCE hInstance_{ NULL };

    struct StateInfo
    {
        std::wstring s{ L"Hello world!" };
        RECT box{ 10, 10, 100, 100 };
    };

    StateInfo state_info;
};

template <class DERIVED_TYPE>
LRESULT CALLBACK BaseWindow<DERIVED_TYPE>::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DERIVED_TYPE* pThis = nullptr;
    if (uMsg == WM_NCCREATE)
    {
        pThis = reinterpret_cast<DERIVED_TYPE*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->hwnd_ = hWnd;
    }
    else
    {
        pThis = reinterpret_cast<DERIVED_TYPE*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->wndproc(hWnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

template <class DERIVED_TYPE>
WPARAM BaseWindow<DERIVED_TYPE>::msg_loop()
{
    MSG msg{};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

template <class DERIVED_TYPE>
BOOL BaseWindow<DERIVED_TYPE>::create(
    PCWSTR lpWindowName,
    DWORD dwStyle,
    int x,
    int y,
    int w,
    int h,
    DWORD dwExStyle,
    HWND hWndParent,
    HMENU hMenu
)
{
    if ((hInstance_ = GetModuleHandle(NULL)) == NULL)
    {
        PRINT_LAST_ERROR();
        return FALSE;
    }

    WNDCLASS wc{};
    wc.lpfnWndProc      = DERIVED_TYPE::WndProc;
    wc.hInstance        = hInstance_;
    wc.lpszClassName    = class_name();

    if (!RegisterClass(&wc))
    {
        PRINT_LAST_ERROR();
        return FALSE;
    }

    if ((hwnd_ = CreateWindowEx(
        dwExStyle,
        class_name(),
        lpWindowName,
        dwStyle,
        x, y, w, h,
        hWndParent,
        hMenu,
        hInstance_,
        this)) == NULL)
    {
        PRINT_LAST_ERROR();
        return FALSE;
    }

    return TRUE;
}
