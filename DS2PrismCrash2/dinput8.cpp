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
	if (code == HC_ACTION && lParam & (1 << 31))
	{
		switch (wParam)
		{
		case VK_F1:
			MessageBeep(0);
			enableScript(ScriptEnum::PRISM_CRASH);
			break;

		case VK_F2:
			MessageBeep(0);
			disableScript(ScriptEnum::PRISM_CRASH);
			break;

		case VK_F3:
			MessageBeep(0);
			writePointer<BYTE>(PNAME::GODMODE, 1);
			break;

		case VK_F4:
		{
			MessageBeep(0);
			HANDLE hThread = CreateThread(NULL, NULL, delayedGodMode, NULL, NULL, NULL);
			CloseHandle(hThread);
			break;
		}

		case VK_F5:
			MessageBeep(0);
			enableScript(ScriptEnum::ZERO_MAP);
			break;

		case VK_F6:
			MessageBeep(0);
			disableScript(ScriptEnum::ZERO_MAP);
			break;

		case VK_F7:
			MessageBeep(0);
			callScript(ScriptEnum::ITEM_SPAWN);
			break;

		case VK_F8:
			MessageBeep(0);
			disableScript(ScriptEnum::ITEM_SPAWN);
			break;
		}
	}

	return CallNextHookEx(SysHook, code, wParam, lParam);
}