#pragma once

#ifndef _H_EXP
#define _H_EXP

#include "defines.h"
#include "harddisk.h"

#define HW_COUNT 1
hardware info[HW_COUNT];
ULONGLONG thrArg;

HANDLE threadH;
DWORD threadID;

int _cdecl intrpt()
{
	USHORT itr = 0;
	int cycle = 0;
	(*getReg)(REG_A, &itr);
	switch (itr)
	{
		case 0:
			(*setReg)(REG_B, hddState);
			(*setReg)(REG_C, hddError);
		case 1:
			(*getReg)(REG_X, &throwItr);
		case 2:
			(*setReg)(REG_B, hdInfo.size);
		case 3:
		{
			switch (hddState)
			{
				case HDD_STATE_READY:
					(getReg)(REG_X, &itr);
					thrArg = (thrArg << 16) + itr;
					(getReg)(REG_Y, &itr);
					thrArg = (thrArg << 16) + itr;
					(getReg)(REG_Z, &itr);
					thrArg = (thrArg << 16) + itr;
					threadH = CreateThread(NULL, 0, &HDDThreadRead, (LPVOID)(&thrArg), 0, &threadID);
					setReg(REG_B, 1);
					break;
				case HDD_STATE_BUSY:
					setError(HDD_ERROR_BUSY);
					setReg(REG_B, 0);
					break;
			}
		}
		case 4:
		{
			switch (hddState)
			{
				case HDD_STATE_READY:
					(getReg)(REG_X, &itr);
					thrArg = (thrArg << 16) + itr;
					(getReg)(REG_Y, &itr);
					thrArg = (thrArg << 16) + itr;
					(getReg)(REG_Z, &itr);
					thrArg = (thrArg << 16) + itr;
					threadH = CreateThread(NULL, 0, &HDDThreadWrite, (LPVOID)(&thrArg), 0, &threadID);
					setReg(REG_B, 1);
					break;
				case HDD_STATE_BUSY:
					setError(HDD_ERROR_BUSY);
					setReg(REG_B, 0);
					break;
			}
		}
	}
	return cycle;
}

extern "C" __declspec(dllexport) int _cdecl init()
{
	hdd.open(HDD_FILENAME, std::ios::in | std::ios::out | std::ios::binary);
	if (!hdd.is_open())
	{
		std::fstream tfs(HDD_FILENAME, std::ios::out | std::ios::binary);
		if (!tfs.is_open())
			return -1;
		hdInfo.verHI = VER_HI;
		hdInfo.verLO = VER_LO;
		hdInfo.size = 0;
		tfs.seekg(0, std::fstream::beg);
		tfs.write((char *)(&hdInfo), sizeof(hdInfo));
		tfs.close();
		hdd.open(HDD_FILENAME, std::ios::in | std::ios::out | std::ios::binary);
		if (!hdd.is_open())
			return -1;
	}
	hdd.seekg(0, std::fstream::beg);
	hdd.read((char *)(&hdInfo), sizeof(hdInfo));
	if (hdInfo.verHI > VER_HI)
		return -2;
	else if (hdInfo.verHI == VER_HI && hdInfo.verLO > VER_LO)
		return -2;
	return 0;
}

extern "C" __declspec(dllexport) int _cdecl getHWCount()
{
	return HW_COUNT;
}

extern "C" __declspec(dllexport) hardware _cdecl getInfo(int count)
{
	return info[count];
}

extern "C" __declspec(dllexport) void _cdecl setHandle(void *p1, void *p2, void *p3, void *p4, void *p5)
{
	setMem = (fSet)(p1);
	getMem = (fGet)(p2);
	setReg = (fSet)(p3);
	getReg = (fGet)(p4);
	addItr = (fAdditr)(p5);
}

#endif