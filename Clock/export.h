#pragma once

#ifndef _H_EXP
#define _H_EXP

#include "clock.h"

#define HW_COUNT 1
hardware info[HW_COUNT];

typedef hardware(*fGetInfo)();
typedef void(*fSetHandle)(void *, void *, void *, void *, void *);

int __cdecl intrpt()
{
	USHORT itr = 0;
	int cycle = 0;
	(*getReg)(0, &itr);
	switch (itr)
	{
		case 0:
			(*getReg)(1, &itr);
			if (itr == 0)
			{
				if (state != 0)
				{
					timeKillEvent(res);
					state = 0;
				}
			}
			else
			{
				if (state != 0)
					timeKillEvent(res);
				count = 0;
				res = timeSetEvent((UINT)((double)(itr) * 1000.0 / 60.0), 1, timer, 0, TIME_PERIODIC);
				state = 1;
			}
			break;
		case 1:
			(*setReg)(2, count);
			break;
		case 2:
			(*getReg)(1, &itr);
			itrpt = itr;
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