#pragma once

#ifndef _H_ASM_DF
#define _H_ASM_DF

#ifndef _H_EMU_DF

typedef unsigned short int USHORT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
typedef unsigned char BYTE;

struct opcode
{
	USHORT op, b, a;
};

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include <cstring>
#include <ctime>
#endif

#include <list>
#include <string>

#define _ERR_ASM_NOERR 0
#define _ERR_ASM_NOT_SUPPORTED -1
#define _ERR_ASM_ILLEGAL -2
#define _ERR_ASM_ILLEGAL_OP -3
#define _ERR_ASM_ILLEGAL_ARG -4
#define _ERR_ASM_TOO_LONG -5

#endif