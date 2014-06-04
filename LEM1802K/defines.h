#pragma once

#ifndef _H_DEF
#define _H_DEF

typedef unsigned short int USHORT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
typedef unsigned char BYTE;

typedef int(_cdecl *fHWInt)();

struct hardware
{
	USHORT a, b, c, x, y;
	fHWInt hwi;
};

typedef int(_cdecl *fSet)(USHORT, USHORT);
typedef int(_cdecl *fGet)(USHORT, USHORT *);
typedef int(_cdecl *fAdditr)(USHORT);

fSet setMem, setReg;
fGet getMem, getReg;
fAdditr additr;

#endif