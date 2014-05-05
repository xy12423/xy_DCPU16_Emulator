#pragma once

#ifndef _H_EMU_DF
#define _H_EMU_DF

#ifndef _H_EMU_CLS_DF

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

typedef int(__cdecl *fGetHWCount)();
typedef hardware(__cdecl *fGetInfo)(int n);
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

#endif

USHORT mem[0x10000];
USHORT reg[8];
UINT pc;
bool pcOf = false;
USHORT sp, ex, ia;
USHORT itr[256];
unsigned char itrp = 0, itre = 0;
bool itri = true;
hardware hwt[65536];
UINT hwn = 0;


#endif