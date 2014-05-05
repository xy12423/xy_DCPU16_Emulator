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

typedef hardware(*fGetInfo)();
typedef void(*fSetHandle)(void *, void *, void *, void *, void *);

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

#define FLOPPY_MESSAGE_STATE 0x0000
#define FLOPPY_MESSAGE_SETINTURRUPT 0x0001
#define FLOPPY_MESSAGE_READ 0x0002
#define FLOPPY_MESSAGE_WRITE 0x0003

#define FLOPPY_STATE_NO_MEDIA 0x0000
#define FLOPPY_STATE_READY 0x0001
#define FLOPPY_STATE_READY_WP 0x0002
#define FLOPPY_STATE_BUSY 0x0003

#define FLOPPY_ERROR_NONE 0x0000
#define FLOPPY_ERROR_BUSY 0x0001
#define FLOPPY_ERROR_NO_MEDIA 0x0002
#define FLOPPY_ERROR_PROTECTED 0x0003
#define FLOPPY_ERROR_EJECT 0x0004
#define FLOPPY_ERROR_BAD_SECTOR 0x0005
#define FLOPPY_ERROR_BROKEN 0xFFFF

#define FLOPPY_VER_HI 0x01
#define FLOPPY_VER_LO 0x00

#define FLOPPY_INFO_FLAG_PROTECTED 0x0001

#endif