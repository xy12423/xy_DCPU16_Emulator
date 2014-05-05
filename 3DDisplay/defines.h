#pragma once

#ifndef _H_DEF
#define _H_DEF

typedef unsigned short int USHORT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
typedef unsigned char BYTE;

typedef int(__cdecl *fHWInt)();

struct hardware
{
	USHORT a, b, c, x, y;
	fHWInt hwi;
};

typedef int(__cdecl *fSet)(USHORT, USHORT);
typedef int(__cdecl *fGet)(USHORT, USHORT *);
typedef int(__cdecl *fAdditr)(USHORT);

fSet setMem, setReg;
fGet getMem, getReg;
fAdditr additr;

#define REG_A 0x0
#define REG_B 0x1
#define REG_C 0x2
#define REG_X 0x3
#define REG_Y 0x4
#define REG_Z 0x5
#define REG_I 0x6
#define REG_J 0x7
#define REG_PC 0x8
#define REG_SP 0x9
#define REG_EX 0xA
#define REG_IA 0xB

#endif