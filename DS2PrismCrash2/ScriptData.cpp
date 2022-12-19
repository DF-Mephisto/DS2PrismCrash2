#include "stdafx.h"
#include "ScriptData.h"

vector<ScriptData> scripts = {
	//PrismCrash
	{
		crashProc,
		{
			SearchType::AOBSCAN,
			0,
			11,
			new char[12] {"\x0F\x2F\x82\xAC\x01\x00\x00\x0F\x93\xC0\xC3"},
			7,
			new char[8] {"\x0F\x2F\x82\xAC\x01\x00\x00"}
		},
		{
			{
				1024,
				new char[1024] {"\x00\x00\x00\x00\x00\x00\x00\x00\x72\xE5\x18\x00\x03\x00\x30\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"}
			}
		},
		false
	},
	{
		zeroMap,
		{
			SearchType::OFFSET,
			0x2575B4,
			0,
			nullptr,
			5,
			new char[6] {"\x48\x89\x4C\x24\x28"}
		},
		{

		},
		false
	}
};