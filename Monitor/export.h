#pragma once

#ifndef _H_EXP
#define _H_EXP

#include "defines.h"
#include "main.h"

#define HW_COUNT 2
hardware info[HW_COUNT];

int __cdecl intrptMonitor()
{
	USHORT itr = 0;
	int cycle = 0;
	(*getReg)(0, &itr);
	USHORT i;
	BYTE j;
	switch (itr)
	{
		case 0:
			(*getReg)(1, &itr);
			if (itr == 0)
			{
				mvStart = 0;
				mOn = 0;
			}
			else
			{
				mvStart = itr;
				if (mOn == 0)
					mOn = 1;
			}
			break;
		case 1:
			(*getReg)(1, &itr);
			if (itr == 0)
			{
				mfStart = 0;
				usrMF = false;
			}
			else
			{
				mfStart = itr;
				usrMF = true;
			}
			break;
		case 2:
			(*getReg)(1, &itr);
			if (itr == 0)
			{
				mpStart = 0;
				usrMP = false;
			}
			else
			{
				mpStart = itr;
				usrMP = true;
			}
			break;
		case 3:

			break;
		case 4:
			(*getReg)(1, &itr);
			i = itr;
			j = 0;
			for (; i < itr + 256; i++, j++)
				(*setMem)(i, mfDef[j]);
			cycle = 256;
			break;
		case 5:
			(*getReg)(1, &itr);
			i = itr;
			j = 0;
			for (; i < itr + 16; i++, j++)
				(*setMem)(i, mpDef[j]);
			cycle = 16;
			break;
	}
	return cycle;
}

int __cdecl intrptKeyb()
{
	USHORT itr = 0;
	int cycle = 0;
	(*getReg)(0, &itr);
	switch (itr)
	{
		case 0:
			keyList.clear();
			break;
		case 1:
			if (keyList.empty())
				(*setReg)(2, 0);
			else
			{
				(*setReg)(2, keyList.front());
				keyList.pop_front();
			}
			break;
		case 2:
			(*getReg)(1, &itr);
			if (keys[itr])
				(*setReg)(2, 1);
			else
				(*setReg)(2, 0);
			break;
		case 3:
			(*getReg)(1, &keyItr);
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