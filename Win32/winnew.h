#pragma once

#include <Windows.h>
#include "winoo.h"

class NewWindow : public BaseWindow<NewWindow>
{
public:
    PCWSTR class_name() const { return L"NewWindow Class"; }
    LRESULT wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
