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

#include <d3d11_4.h>
#include <d2d1_3.h>
#include <d2d1_1helper.h>
#include <d2d1effects.h>
#include <d2d1effectauthor_1.h>
#include <d2d1effecthelpers.h>

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

namespace DX
{
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
