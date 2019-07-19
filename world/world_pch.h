#pragma once
#include <WinSDKVer.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <SDKDDKVer.h>

// Use standard C++ min/max.
#define NOMINMAX

// Exclude GDI, DirectX doen't use it.
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if needed.
#define NOMCX

// Include <winsvc.h> if needed.
#define NOSERVICE

// WinHelp is deprecated.
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl/client.h>

#include <d3d11_4.h>
#include <dxgi1_6.h>

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <cassert>

#ifdef DXTK_AUDIO
#include "Audio.h"
#endif

#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PrimitiveBatch.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"

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
