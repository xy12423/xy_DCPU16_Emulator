#pragma once

#ifndef _H_EMU_EX
#define _H_EMU_EX

#include "define.h"

int __cdecl setMem(USHORT add, USHORT dat)
{
	mem[add] = dat;
	return 0;
}

int __cdecl getMem(USHORT add, USHORT *dat)
{
	*dat = mem[add];
	return 0;
}

int __cdecl setReg(USHORT _reg, USHORT dat)
{
	if (0x00 <= _reg && _reg <= 0x07)
		reg[_reg] = dat;
	else
	{
		switch (_reg)
		{
			case 0x08:
				pc = dat;
				break;
			case 0x09:
				sp = dat;
				break;
			case 0x0A:
				ex = dat;
				break;
			case 0x0B:
				ia = dat;
				break;
			default:
				return -1;
		}
	}
	return 0;
}

int __cdecl getReg(USHORT _reg, USHORT *dat)
{
	if (0x00 <= _reg && _reg <= 0x07)
		*dat = reg[_reg];
	else
	{
		switch (_reg)
		{
			case 0x08:
				*dat = pc;
				break;
			case 0x09:
				*dat = sp;
				break;
			case 0x0A:
				*dat = ex;
				break;
			case 0x0B:
				*dat = ia;
				break;
			default:
				return -1;
		}
	}
	return 0;
}


int __cdecl additr(USHORT _itr)
{
	if (itrp == (unsigned char)(itre - 1))
		return _ERR_EMU_ITR_OVERFLOW;
	itr[itre++] = _itr;
	return 0;
}

#endif