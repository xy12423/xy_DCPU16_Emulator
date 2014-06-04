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

typedef hardware(*fGetInfo)();
typedef void(*fSetHandle)(void *, void *, void *, void *, void *);

typedef int(_cdecl *fSet)(USHORT, USHORT);
typedef int(_cdecl *fGet)(USHORT, USHORT *);
typedef int(_cdecl *fAdditr)(USHORT);

fSet setMem, setReg;
fGet getMem, getReg;
fAdditr addItr;

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

#define HDD_STATE_NO_PLATTER 0x0000
#define HDD_STATE_READY 0x0001
#define HDD_STATE_BUSY 0x0002

#define HDD_ERROR_NO_ERROR 0x0000
#define HDD_ERROR_BUSY 0x0001
#define HDD_ERROR_NO_PLATTER 0x0002
#define HDD_ERROR_BAD_PLATTER 0x0003
#define HDD_ERROR_BAD_SECTOR 0x0003

#define VER_HI 0x00
#define VER_LO 0x01

#define SIZE_SECTOR 512
#define SIZE_PLATTER 1024

#endif