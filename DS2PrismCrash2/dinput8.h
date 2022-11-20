#pragma once

#include <unknwn.h>
#include "MemReader.h"

using DirectInput8Create_t = HRESULT(*)(
	HINSTANCE hinst,
	DWORD dwVersion,
	REFIID riidltf,
	LPVOID * ppvOut,
	LPUNKNOWN punkOuter
	);

extern DirectInput8Create_t OriginalFunction;
extern HMODULE DInput8DLL;
extern HHOOK SysHook;

LRESULT CALLBACK keyDownHook(int code, WPARAM wParam, LPARAM lParam);

extern "C"
{
	DINPUT8_API HRESULT DirectInput8Create(
		HINSTANCE hinst,
		DWORD dwVersion,
		REFIID riidltf,
		LPVOID * ppvOut,
		LPUNKNOWN punkOuter
	);
}