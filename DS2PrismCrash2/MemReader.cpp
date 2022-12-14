// DS2 Crash dll.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "MemReader.h"

using namespace std;


MODULEINFO mInfo;


void memoryInit()
{
	GetModuleInformation(GetCurrentProcess(), GetModuleHandleW(L"DarkSoulsII.exe"), &mInfo, sizeof(mInfo));
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

	DWORD64 instAddress = getInstructionAddress(data.instInfo);
	data.instAddress = instAddress;
	
	uint16_t scriptSize = getScriptSize(data.scriptCode);

	DWORD retAddress = 7 - 5 - ((DWORD64)data.scriptAddress - instAddress) - scriptSize;
	char retInstr[5] = { 0xE9 };

	memcpy((LPVOID)data.scriptAddress, data.scriptCode, scriptSize);
	memcpy(retInstr + 1, &retAddress, 4);
	memcpy((char*)data.scriptAddress + scriptSize, &retInstr, 5);
	memcpy((char*)data.scriptAddress + scriptSize + 5, &mInfo.lpBaseOfDll, 8);

	int dataOffset = 8;

	for (int i = 0; i < data.buffers.size(); i++)
	{
		DWORD64 bufAddr = data.bufferAddresses[i];
		memcpy((LPVOID)bufAddr, data.buffers[i].data, data.buffers[i].size);
		memcpy((char*)data.scriptAddress + scriptSize + 5 + dataOffset, &bufAddr, 8);
		dataOffset += 8;
	}

	DWORD jmpAddress = 0 - (instAddress - (DWORD64)data.scriptAddress) - 5;
	char* jmpInstr = new char[data.instInfo.instructionSize]{ '\xE9' };
	memset(jmpInstr + 1, 0x90, data.instInfo.instructionSize - 1);

	memcpy(jmpInstr + 1, &jmpAddress, 4);
	memcpy((LPVOID)instAddress, jmpInstr, data.instInfo.instructionSize);
	delete[] jmpInstr;
}

void disableScript(ScriptEnum id)
{
	ScriptData& data = scripts[static_cast<int>(id)];

	if (!data.enabled)
		return;
	data.enabled = false;

	DWORD64 instAddress = data.instAddress;

	memcpy((LPVOID)instAddress, data.instInfo.originalInstruction, data.instInfo.instructionSize);

	if (data.scriptAddress)
	{
		VirtualFree((LPVOID)data.scriptAddress, 0, MEM_RELEASE);
		data.scriptAddress = NULL;
	}

	for (DWORD64 bufAddr : data.bufferAddresses)
		VirtualFree((LPVOID)bufAddr, 0, MEM_RELEASE);

	data.bufferAddresses.clear();
}

void nopInstruction(ScriptEnum id)
{
	ScriptData& data = scripts[static_cast<int>(id)];
	if (data.enabled)
		return;
	data.enabled = true;

	DWORD64 instAddress = getInstructionAddress(data.instInfo);
	memset((LPVOID)instAddress, 0x90, data.instInfo.instructionSize);
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
	while ((regionAddress = (DWORD64)mInfo.lpBaseOfDll - offset) <= (DWORD64)mInfo.lpBaseOfDll)
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

DWORD64 getInstructionAddress(const InstructionInfo& const info)
{
	DWORD64 instAddress;

	switch (info.searchType)
	{
	case SearchType::OFFSET:
		instAddress = (DWORD64)mInfo.lpBaseOfDll + info.offset;
		break;

	case SearchType::AOBSCAN:
		instAddress = findSignature(mInfo, info.aob, info.aobSize);
		break;
	}

	return instAddress;
}

DWORD64 findSignature(MODULEINFO& mInfo, const char* const aob , const size_t aobSize)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	DWORD64 offset = 0;
	DWORD64 res = 0;
	while (offset < mInfo.SizeOfImage) {
		VirtualQuery((LPCVOID)((DWORD64)mInfo.lpBaseOfDll + offset), &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		if (mbi.State != MEM_FREE) {
			for (int i = 0; i <= mbi.RegionSize - aobSize; i++) {
				if (dataCompare((char*)mbi.BaseAddress + i, aob, aobSize)) {
					res = (DWORD64)mbi.BaseAddress + i;
					goto ret;
				}
			}
		}
		offset += mbi.RegionSize;
	}

ret:
	return res;
}

bool dataCompare(const char* data, const char* sign, int size)
{
	for (int i = 0; i < size; i++) {
		if (data[i] != sign[i]) return false;
	}
	return true;
}

template<class T>
void writePointer(PNAME id, T val)
{
	Pointer& p = pointers[static_cast<int>(id)];

	if (p.length <= 0)
		return;

	DWORD64 addr = (DWORD64)mInfo.lpBaseOfDll;

	for (int i = 0; i < p.length - 1; i++)
	{
		addr = *(DWORD64*)(addr + p.offsets[i]);

		if (IsBadReadPtr((DWORD64*)addr, 8))
			return;
	}

	addr += p.offsets[p.length - 1];

	*(T*)addr = val;
}

//Template functions
template void writePointer<float>(PNAME id, float val);
template void writePointer<BYTE>(PNAME id, BYTE val);