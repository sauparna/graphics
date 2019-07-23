#pragma once

// Use standard C++ min/max.
#define NOMINMAX

// Exclude GDI, DirectX dosen't use it.
#define NODRAWTEXT
//#define NOGDI
#define NOBITMAP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl/client.h>
#include "resource.h"

#include <d3d11_4.h>
#include <d2d1_3.h>
#include <d2d1_1helper.h>
#include <d2d1effects.h>
#include <d2d1effectauthor_1.h>
#include <d2d1effecthelpers.h>
#include <wincodec.h>
#include <dwrite_3.h>

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <assert.h>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <string>

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <array>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

namespace DX
{
	const size_t kBitmapCount = 6;

	enum kBitmapNames
	{
		BITMAP_PRIMITIVES = 0,
		BITMAP_PRIMITIVES_GRID,
		BITMAP_TEXT,
		BITMAP_FROM_FILE,
		BITMAP_FROM_RSRC,
		BITMAP_FROM_MEM
	};

	const size_t kBrushCount = 3;

	enum kBrushNames
	{
		BRUSH_BLACK = 0,
		BRUSH_WHITE,
		BRUSH_SKYBLUE
	};

	// Helper class for COM exceptions.
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		virtual const char* what() const override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08x", static_cast<unsigned int>(result));
			return s_str;
		}
	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures to exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}
}
