#pragma once
#include <vector>

using std::vector;

struct Pointer
{
	DWORD32* offsets;
	size_t length;

	Pointer(DWORD32* offsets, size_t length);
	Pointer(const Pointer& p);
	~Pointer();
};

enum class PNAME
{
	X1 = 0, X2, Y1, Y2, GODMODE
};

extern vector<Pointer> pointers;