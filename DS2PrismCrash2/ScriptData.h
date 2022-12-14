#pragma once
#include <vector>
#include <memory>
extern "C"
{
	void crashProc();
	void zeroMap();
	void itemSpawn();
}

enum class ScriptEnum
{
	PRISM_CRASH = 0,
	ZERO_MAP,
	ITEM_SPAWN
};

enum class SearchType
{
	NONE, OFFSET, AOBSCAN
};

struct InstructionInfo
{
	const SearchType searchType;
	const DWORD64 offset;
	const size_t aobSize;
	const char* const aob;
	const uint16_t instructionSize;
	const char* const originalInstruction;
};

struct Buffer
{
	const int size;
	const char* const data;
};

struct ScriptData
{
	const LPVOID scriptCode;
	const InstructionInfo instInfo;
	const vector<Buffer> buffers;
	bool enabled;
	DWORD64 instAddress;
	DWORD64 scriptAddress;
	vector<DWORD64> bufferAddresses;
};

extern vector<ScriptData> scripts;