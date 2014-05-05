#pragma once

#ifndef _H_EXP
#define _H_EXP

#include "defines.h"
#include "floppy.h"

#define HW_COUNT 1
hardware info[HW_COUNT];
ULONGLONG thrArg;

int __cdecl intrpt()
{
	USHORT itr = 0;
	int cycle = 0;
	getReg(0, &itr);
	switch (itr)
	{
		case 0:
			setReg(1, floppyState);
			setReg(2, floppyError);
			break;
		case 1:
			getReg(REG_X, const_cast<USHORT *>(&throwItrpt));
			break;
		case 2:
			switch (floppyState)
			{
				case FLOPPY_STATE_NO_MEDIA:
					setError(FLOPPY_ERROR_NO_MEDIA);
					setReg(REG_B, 0);
					break;
				case FLOPPY_STATE_READY:
				case FLOPPY_STATE_READY_WP:
					(getReg)(REG_X, &itr);
					thrArg = (thrArg << 16) + itr;
					(getReg)(REG_Y, &itr);
					thrArg = (thrArg << 16) + itr;
					threadH = CreateThread(NULL, 0, &FloppyThreadRead, (LPVOID)(&thrArg), 0, &threadID);
					setReg(REG_B, 1);
					break;
				case FLOPPY_STATE_BUSY:
					setError(FLOPPY_ERROR_BUSY);
					setReg(REG_B, 0);
					break;
			}
			break;
		case 3:
			switch (floppyState)
			{
				case FLOPPY_STATE_NO_MEDIA:
					setError(FLOPPY_ERROR_NO_MEDIA);
					setReg(REG_B, 0);
					break;
				case FLOPPY_STATE_READY:
					(getReg)(REG_X, &itr);
					thrArg = itr;
					(getReg)(REG_Y, &itr);
					thrArg = (thrArg << 16) + itr;
					threadH = CreateThread(NULL, 0, &FloppyThreadWrite, (LPVOID)(&thrArg), 0, &threadID);
					setReg(REG_B, 1);
					break;
				case FLOPPY_STATE_READY_WP:
					setError(FLOPPY_ERROR_PROTECTED);
					setReg(REG_B, 0);
					break;
				case FLOPPY_STATE_BUSY:
					setError(FLOPPY_ERROR_BUSY);
					setReg(REG_B, 0);
					break;
			}
			break;
	}
	return 0;
}

extern "C" __declspec(dllexport) int init()
{
	int ret = LoadDisk("floppy.dfd");
	return ret;
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