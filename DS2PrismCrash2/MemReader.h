#pragma once

#include <stdexcept>
#include "ScriptData.h"

void memoryInit();
int64_t getScriptSize(LPVOID script);
void enableScript(ScriptEnum id);
void disableScript(ScriptEnum id);
void initJmpInstr(char* arr, int size);
DWORD64	alloc(int size);
bool allocScriptMem(ScriptEnum id);

