#pragma once

#ifndef _H_EMU_CLS_DF
#define _H_EMU_CLS_DF

typedef unsigned short int USHORT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
typedef unsigned char BYTE;

struct opcode
{
	USHORT op, b, a;
};

typedef int(__cdecl *fHWInt)();

struct hardware
{
	USHORT a, b, c, x, y;
	fHWInt hwi;
};

hardware hdEmpty;

typedef hardware(__cdecl *fGetInfo)();
typedef void(__cdecl *fSetHandle)(void *, void *, void *, void *, void *);
typedef int(__cdecl *fInit)();

#define _ERR_EMU_NOERR 0
#define _ERR_EMU_OTHER -1
#define _ERR_EMU_ITR_EMPTY -2
#define _ERR_EMU_ITR_OVERFLOW -3
#define _ERR_EMU_EXP_MEMOVFL -4

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include <cstring>

#endif