#pragma once

#ifndef _H_FP
#define _H_FP

#include "stdafx.h"
#include "defines.h"

volatile USHORT floppyState = FLOPPY_STATE_NO_MEDIA;
volatile USHORT floppyError = FLOPPY_ERROR_NONE;
int track = -1;
string floppyPath;
volatile USHORT throwItrpt = 0;

struct FloppyInfo
{
	USHORT floppyVersion;
	USHORT floppyFlags;
	USHORT unuse[512 - 2];
};

FloppyInfo floppyInfo;
USHORT floppyBuffer[80 * 18][512];

HANDLE threadH = NULL;
DWORD threadID = 0;

void setState(USHORT _state)
{
	floppyState = _state;
	if (throwItrpt != 0)
		(*additr)(throwItrpt);
}

void setError(USHORT _error)
{
	floppyError = _error;
	if (throwItrpt != 0)
		(*additr)(throwItrpt);
}

int Eject()
{
	floppyPath = "";
	setState(FLOPPY_STATE_NO_MEDIA);
	track = -1;
	return 0;
}

int LoadDisk(string _path)
{
	if (floppyState != FLOPPY_STATE_NO_MEDIA)
		Eject();
	int ret = 0;
	fstream floppyFile(_path, ios::in | ios::binary);
	if (!floppyFile.is_open())
	{
		ret = -1;
		return -1;
	}
	floppyFile.read((char *)(&floppyInfo), sizeof(FloppyInfo) / sizeof(char));
	USHORT flVer = floppyInfo.floppyVersion;
	BYTE flVerHi = flVer >> 8;
	if (flVerHi > FLOPPY_VER_HI)
	{
		ret = -2;
		goto _ld_end;
	}
	else if ((flVerHi == FLOPPY_VER_HI) && ((flVer & 0xFF) > FLOPPY_VER_LO))
	{
		ret = -2;
		goto _ld_end;
	}
	USHORT flFlag = floppyInfo.floppyFlags;
	if (flFlag&FLOPPY_INFO_FLAG_PROTECTED)
		floppyState = FLOPPY_STATE_READY_WP;
	else
		floppyState = FLOPPY_STATE_READY;
	floppyFile.read((char *)(floppyBuffer), sizeof(floppyBuffer) / sizeof(char));
	track = 0;
	_ld_end:floppyFile.close();
	if (ret == 0)
		floppyPath = _path;
	return ret;
}

DWORD WINAPI FloppyThreadRead(LPVOID lpParam)
{
	USHORT pastState = floppyState;
	setState(FLOPPY_STATE_BUSY);
	ULONGLONG itr = *(ULONGLONG*)(lpParam);
	USHORT x = (USHORT)(itr >> 16), y = (USHORT)(itr);
	if (x >= 80 * 18)
	{
		setError(FLOPPY_ERROR_BAD_SECTOR);
		setState(pastState);
		return 0;
	}
	int _track = x / 18;
	int trackSeekingTime = (int)((double)(2.4) * abs(track - _track) * 1000) / CLOCKS_PER_SEC;
	track = _track;
	clock_t start = clock();
	while (clock() - start < trackSeekingTime);
	int i, j;
	for (i = 0, j = y; i < 512; i++, j++)
		(*setMem)(j, floppyBuffer[x][i]);
	setState(pastState);
	return 0;
}

DWORD WINAPI FloppyThreadWrite(LPVOID lpParam)
{
	USHORT pastState = floppyState;
	setState(FLOPPY_STATE_BUSY);
	ULONGLONG itr = *(ULONGLONG*)(lpParam);
	USHORT x = (USHORT)(itr >> 16), y = (USHORT)(itr);
	if (x >= 80 * 18)
	{
		setError(FLOPPY_ERROR_BAD_SECTOR);
		setState(pastState);
		return 0;
	}
	{
		int _track = x / 18;
		int trackSeekingTime = (int)((double)(2.4) * abs(track - _track) * 1000) / CLOCKS_PER_SEC;
		track = _track;
		clock_t start = clock();
		while (clock() - start < trackSeekingTime);
	}
	int i, j;
	USHORT data;
	for (i = 0, j = y; i < 512; i++, j++)
	{
		(*getMem)(j, &data);
		floppyBuffer[x][i] = data;
	}
	fstream file(floppyPath, ios::out | ios::binary);
	file.write((char *)(&floppyInfo), sizeof(FloppyInfo) / sizeof(char));
	file.write((char *)(floppyBuffer), sizeof(floppyBuffer) / sizeof(char));
	file.close();
	setState(pastState);
	return 0;
}

#endif