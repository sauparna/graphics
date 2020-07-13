#include "winoo.h"

static inline void PrintLastError()
{
    wchar_t buf[512];
    size_t size = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
    std::wstring msg(buf, size);
    OutputDebugString((LPCWSTR)msg.c_str());
}

