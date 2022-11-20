#pragma once
#include <vector>
#include <memory>
extern "C"
{
	void crashProc();
}

enum class ScriptEnum
{
	PrismCrash = 0
};

struct Buffer
{
	const int size;
	const char* const data;
};

struct ScriptData
{
	const LPVOID scriptCode;
	const DWORD64 instructionOffset;
	const uint16_t instructionSize;
	const char* const originalInstruction;
	const vector<Buffer> buffers;
	bool enabled;
	DWORD64 scriptAddress;
	vector<DWORD64> bufferAddresses;
};

extern vector<ScriptData> scripts;