#pragma once

#include <Windows.h>
#include <string>
#include <exception>

template <class DERIVED_TYPE>
class BaseWindow
{
public:
    BaseWindow() : hwnd_{ NULL }, hInstance_{ NULL } {}
    HWND hwnd() const { return hwnd_; }
    BOOL create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int w = CW_USEDEFAULT,
        int h = CW_USEDEFAULT,
        DWORD dwExStyle = 0,
        HWND hWndParent = NULL,
        HMENU hMenu = NULL
    );
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual WPARAM msg_loop();

protected:
    virtual PCWSTR class_name() const = 0;
    virtual LRESULT wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    HWND hwnd_;
    HINSTANCE hInstance_;

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
    hInstance_ = GetModuleHandle(NULL);
    WNDCLASS wc{};

    wc.lpfnWndProc = BaseWindow::WndProc;
    wc.hInstance = hInstance_;
    wc.lpszClassName = class_name();

    if (!RegisterClass(&wc)) return FALSE;

    hwnd_ = CreateWindowEx(
        dwExStyle,
        class_name(),
        lpWindowName,
        dwStyle,
        x, y, w, h,
        hWndParent,
        hMenu,
        hInstance_,
        this
    );

    return (hwnd_ ? TRUE : FALSE);
}
