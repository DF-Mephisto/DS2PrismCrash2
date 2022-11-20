// DS2 Crash dll.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "MemReader.h"

using namespace std;

extern "C"
{
	DWORD64 moduleAddr;
}

void memoryInit()
{
	moduleAddr = (DWORD64)GetModuleHandle(L"DarkSoulsII.exe");

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
}

int64_t getScriptSize(LPVOID script)
{
	int64_t size = 0;
	BYTE* ptr = (BYTE*)script;

	for (; *((DWORD*)ptr) != 0x90909090; size++, ptr++);

	return size;
}

void enableScript(ScriptEnum id)
{
	ScriptData& data = scripts[static_cast<int>(id)];
	if (data.enabled)
		return;
	data.enabled = true;

	if (!allocScriptMem(id))
	{
		disableScript(id);
		return;
	}

	uint16_t scriptSize = getScriptSize(data.scriptCode);

	DWORD retAddress = 7 - 5 - ((DWORD64)data.scriptAddress - (moduleAddr + data.instructionOffset)) - scriptSize;
	char retInstr[5] = { 0xE9 };

	memcpy((LPVOID)data.scriptAddress, data.scriptCode, scriptSize);
	memcpy(retInstr + 1, &retAddress, 4);
	memcpy((char*)data.scriptAddress + scriptSize, &retInstr, 5);
	memcpy((char*)data.scriptAddress + scriptSize + 5, &moduleAddr, 8);

	int dataOffset = 8;

	for (int i = 0; i < data.buffers.size(); i++)
	{
		DWORD64 bufAddr = data.bufferAddresses[i];
		memcpy((LPVOID)bufAddr, data.buffers[i].data, data.buffers[i].size);
		memcpy((char*)data.scriptAddress + scriptSize + 5 + dataOffset, &bufAddr, 8);
		dataOffset += 8;
	}

	DWORD jmpAddress = 0 - ((moduleAddr + data.instructionOffset) - (DWORD64)data.scriptAddress) - 5;
	char* jmpInstr = new char[data.instructionSize];
	initJmpInstr(jmpInstr, data.instructionSize);
	memcpy(jmpInstr + 1, &jmpAddress, 4);
	memcpy((LPVOID)(moduleAddr + data.instructionOffset), jmpInstr, data.instructionSize);
	delete[] jmpInstr;
}

void disableScript(ScriptEnum id)
{
	ScriptData& data = scripts[static_cast<int>(id)];

	if (!data.enabled)
		return;
	data.enabled = false;

	memcpy((LPVOID)(moduleAddr + data.instructionOffset), data.originalInstruction, data.instructionSize);

	if (data.scriptAddress)
	{
		VirtualFree((LPVOID)data.scriptAddress, 0, MEM_RELEASE);
		data.scriptAddress = NULL;
	}

	for (DWORD64 bufAddr : data.bufferAddresses)
		VirtualFree((LPVOID)bufAddr, 0, MEM_RELEASE);

	data.bufferAddresses.clear();
}

bool allocScriptMem(ScriptEnum id)
{
	ScriptData& data = scripts[static_cast<int>(id)];

	try
	{
		if (!(data.scriptAddress = alloc(4096)))
			throw runtime_error("memory allocation error");

		for (Buffer buf : data.buffers)
		{
			DWORD64 bufAddr = 0;
			if (!(bufAddr = alloc(2048)))
				throw runtime_error("memory allocation error");

			data.bufferAddresses.push_back(bufAddr);
		}
	}
	catch (runtime_error& e)
	{
		return false;
	}

	return true;
}

DWORD64	alloc(int size)
{
	MEMORY_BASIC_INFORMATION mbi;
	DWORD64 regionAddress = 0;
	DWORD64 offset = 0;
	DWORD64 allocatedMemory = 0;
	while ((regionAddress = moduleAddr - offset) <= moduleAddr)
	{
		VirtualQuery((LPCVOID)regionAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

		if (mbi.State == MEM_FREE)
		{
			if (allocatedMemory = (DWORD64)VirtualAlloc((LPVOID)regionAddress, size, MEM_RESERVE, PAGE_EXECUTE_READWRITE))
			{
				allocatedMemory = (DWORD64)VirtualAlloc((LPVOID)allocatedMemory, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				break;
			}
		}

		offset += mbi.RegionSize;
	}

	return allocatedMemory;
}

void initJmpInstr(char* arr, int size)
{
	arr[0] = 0xE9;
	for (int i = 1; i < size; i++)
		arr[i] = 0x90;
}