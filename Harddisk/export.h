#pragma once

#ifndef _H_EXP
#define _H_EXP

#include "defines.h"

#define HW_COUNT 1
hardware info[HW_COUNT];

int __cdecl intrpt()
{
	USHORT itr = 0;
	int cycle = 0;
	(*getReg)(0, &itr);
	switch (itr)
	{

	}
	return cycle;
}

extern "C" __declspec(dllexport) int init()
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl getHWCount()
{
	return HW_COUNT;
}

extern "C" __declspec(dllexport) hardware __cdecl getInfo(int count)
{
	return info[count];
}

extern "C" __declspec(dllexport) void __cdecl setHandle(void *p1, void *p2, void *p3, void *p4, void *p5)
{
	setMem = (fSet)(p1);
	getMem = (fGet)(p2);
	setReg = (fSet)(p3);
	getReg = (fGet)(p4);
	additr = (fAdditr)(p5);
}

#endif