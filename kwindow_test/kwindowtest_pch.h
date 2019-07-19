#pragma once

// Use standard C++ min/max.
#define NOMINMAX
// Exclude GDI, DirectX dosen't use it.
#define NODRAWTEXT
//#define NOGDI
#define NOBITMAP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <memory>
#include <string>
