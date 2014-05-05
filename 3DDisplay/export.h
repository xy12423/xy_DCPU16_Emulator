#pragma once

#ifndef _H_EXP
#define _H_EXP

#include "defines.h"
#include "3DDisplay.h"

#define HW_COUNT 1
hardware info[HW_COUNT];

typedef hardware(*fGetInfo)();
typedef void(*fSetHandle)(void *, void *, void *, void *, void *);

int __cdecl intrpt()
{
	USHORT itr = 0;
	int cycle = 0;
	(*getReg)(REG_A, &itr);
	switch (itr)
	{
		case 0:
			(*setReg)(REG_B, dpState);
			(*setReg)(REG_C, dpError);
			break;
		case 1:
			(*getReg)(REG_X, &itr);
			mvStart = itr;
			(*getReg)(REG_Y, &itr);
			mvCount = itr;
			mvEnd = mvStart + itr * 2;
			break;
		case 2:
			(*getReg)(REG_X, &itr);
			rotAngle = itr;
			break;
	}
	return cycle;
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