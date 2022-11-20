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
	int size;
	char* data;
};

struct ScriptData
{
	LPVOID scriptCode;
	DWORD64 instructionOffset;
	uint16_t instructionSize;
	char* originalInstruction;
	vector<Buffer> buffers;
	bool enabled;
	DWORD64 scriptAddress;
	vector<DWORD64> bufferAddresses;
};

extern vector<ScriptData> scripts;