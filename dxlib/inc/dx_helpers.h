#pragma once

#include <d3d11_1.h>

#if !defined(NO_D3D11_DEBUG_NAME) && ( defined(_DEBUG) || defined(PROFILE) )
#if !defined(_XBOX_ONE) || !defined(_TITLE)
#pragma comment(lib,"dxguid.lib")
#endif
#endif

#ifndef IID_GRAPHICS_PPV_ARGS
#define IID_GRAPHICS_PPV_ARGS(x) IID_PPV_ARGS(x)
#endif

#include <exception>
#include <stdint.h>

namespace DirectX
{
	// Helper sets a D3D resource name string (used by PIX and debug layer leak reporting).
	template<UINT TNameLength>
	inline void SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_z_ const char(&name)[TNameLength])
	{
		#if !defined(NO_D3D11_DEBUG_NAME) && ( defined(_DEBUG) || defined(PROFILE) )
			#if defined(_XBOX_ONE) && defined(_TITLE)
				wchar_t wname[MAX_PATH];
				int result = MultiByteToWideChar(CP_UTF8, 0, name, TNameLength, wname, MAX_PATH);
				if (result > 0)
				{
					resource->SetName(wname);
				}
			#else
				resource->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
			#endif
		#else
			UNREFERENCED_PARAMETER(resource);
			UNREFERENCED_PARAMETER(name);
		#endif
	}

	template<UINT TNameLength>
	inline void SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_z_ const wchar_t(&name)[TNameLength])
	{
		#if !defined(NO_D3D11_DEBUG_NAME) && ( defined(_DEBUG) || defined(PROFILE) )
			#if defined(_XBOX_ONE) && defined(_TITLE)
				resource->SetName(name);
			#else
				char aname[MAX_PATH];
				int result = WideCharToMultiByte(CP_UTF8, 0, name, TNameLength, aname, MAX_PATH, nullptr, nullptr);
				if (result > 0)
				{
					resource->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, aname);
				}
			#endif
		#else
			UNREFERENCED_PARAMETER(resource);
			UNREFERENCED_PARAMETER(name);
		#endif
	}
}
