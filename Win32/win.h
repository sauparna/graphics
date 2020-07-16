#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

struct StateInfo
{
	std::wstring s = L"Hello world!";
	RECT box{ 10, 10, 100, 100 };
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

