#pragma once

#ifndef _H_MAIN
#define _H_MAIN

#include <fstream>

#define HDD_FILENAME "harddisk.dhd"

struct hdinfo
{
	BYTE verHI, verLO;
	USHORT size;
	USHORT reserved[254];
} hdInfo;

USHORT hddState = 0, hddError = 0;
USHORT throwItr = 0;

void setState(USHORT newState)
{
	hddState = newState;
	if (throwItr != 0)
		(*addItr)(throwItr);
}

void setError(USHORT newError)
{
	hddError = newError;
	if (throwItr != 0)
		(*addItr)(throwItr);
}

std::fstream hdd;

DWORD WINAPI HDDThreadRead(LPVOID lpParam)
{
	USHORT pastState = hddState;
	setState(HDD_STATE_BUSY);
	ULONGLONG itr = *(ULONGLONG*)(lpParam);
	USHORT x = (USHORT)(itr >> 32), y = (USHORT)(itr >> 16), z = (USHORT)(itr);
	if (x >= hdInfo.size)
	{
		setError(HDD_ERROR_BAD_PLATTER);
		setState(pastState);
		return 0;
	}
	if (y >= SIZE_PLATTER)
	{
		setError(HDD_ERROR_BAD_SECTOR);
		setState(pastState);
		return 0;
	}
	int i, j;
	hdd.seekg(sizeof(hdInfo) + (x * SIZE_PLATTER + y) * SIZE_SECTOR * sizeof(USHORT), std::fstream::beg);
	USHORT dat;
	for (i = 0, j = y; i < SIZE_SECTOR; i++, j++)
	{
		hdd.read((char *)(&dat), sizeof(USHORT));
		(*setMem)(j, dat);
	}
	setState(pastState);
	return 0;
}

DWORD WINAPI HDDThreadWrite(LPVOID lpParam)
{
	USHORT pastState = hddState;
	setState(HDD_STATE_BUSY);
	ULONGLONG itr = *(ULONGLONG*)(lpParam);
	USHORT x = (USHORT)(itr >> 32), y = (USHORT)(itr >> 16), z = (USHORT)(itr);
	if (x >= hdInfo.size)
	{
		setError(HDD_ERROR_BAD_PLATTER);
		setState(pastState);
		return 0;
	}
	if (y >= SIZE_PLATTER)
	{
		setError(HDD_ERROR_BAD_SECTOR);
		setState(pastState);
		return 0;
	}
	int i, j;
	hdd.seekp(sizeof(hdInfo) + (x * SIZE_PLATTER + y) * SIZE_SECTOR * sizeof(USHORT), std::fstream::beg);
	USHORT dat;
	for (i = 0, j = y; i < SIZE_SECTOR; i++, j++)
	{
		(*getMem)(j, &dat);
		hdd.write((char *)(&dat), sizeof(USHORT));
	}
	setState(pastState);
	return 0;
}

#endif