#pragma once

#include <stdexcept>
#include <psapi.h>
#include "ScriptData.h"

void memoryInit();
int64_t getScriptSize(LPVOID script);
void enableScript(ScriptEnum id);
void nopInstruction(ScriptEnum id);
void disableScript(ScriptEnum id);
DWORD64	alloc(int size);
bool allocScriptMem(ScriptEnum id);
DWORD64 getInstructionAddress(const InstructionInfo& const info);
DWORD64 findSignature(MODULEINFO& mInfo, const char* const aob, const size_t aobSize);
bool dataCompare(const char* data, const char* sign, int size);
