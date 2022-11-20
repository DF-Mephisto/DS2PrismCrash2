// DS2 Crash dll.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "dinput8.h"

DirectInput8Create_t OriginalFunction = nullptr;
HMODULE DInput8DLL = nullptr;
HHOOK SysHook = nullptr;

DINPUT8_API HRESULT DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter)
{
	if (OriginalFunction)
	{
		return OriginalFunction(hinst, dwVersion, riidltf, ppvOut, punkOuter);
	}
	return S_FALSE;
}

LRESULT CALLBACK keyDownHook(int code, WPARAM wParam, LPARAM lParam)
{
	if (code == HC_ACTION && 
		((MSG*)lParam)->message == WM_KEYDOWN &&  
		FindWindow(NULL, L"DARK SOULS II") == ((MSG*)lParam)->hwnd)
	{
		switch (((MSG*)lParam)->wParam)
		{
		case VK_F1:
			MessageBeep(0);
			enableScript(ScriptEnum::PrismCrash);
			break;

		case VK_F2:
			MessageBeep(0);
			disableScript(ScriptEnum::PrismCrash);
			break;
		}
	}

	return CallNextHookEx(SysHook, code, wParam, lParam);
}