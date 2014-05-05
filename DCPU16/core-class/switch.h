#pragma once

#ifndef _H_EMU_CLS_SW
#define _H_EMU_CLS_SW

#include "main.h"

int DCPU16::reta(USHORT a, USHORT &ret)
{
	if (a > 0x3F)
		return -1;
	if (0x0 <= a && a <= 0x7)
		ret = reg[a];
	else if (0x8 <= a && a <= 0xF)
		ret = mem[reg[a - 0x8]];
	else if (0x10 <= a && a <= 0x17)
	{
		ret = mem[reg[a - 0x10] + mem[pc++]];
		if (pc > 0xFFFF)
		{
			pcOf = true;
			pc &= 0xFFFF;
		}
		return 1;
	}
	else if (0x20 <= a && a <= 0x3F)
		ret = a - 0x21;
	else
	{
		switch (a)
		{
			case 0x18:
				ret = mem[sp++];
				break;
			case 0x19:
				ret = mem[sp];
				break;
			case 0x1A:
				ret = mem[sp + mem[pc++]];
				if (pc > 0xFFFF)
				{
					pcOf = true;
					pc &= 0xFFFF;
				}
				return 1;
				break;
			case 0x1B:
				ret = sp;
				break;
			case 0x1C:
				ret = pc;
				break;
			case 0x1D:
				ret = ex;
				break;
			case 0x1E:
				ret = mem[mem[pc++]];
				if (pc > 0xFFFF)
				{
					pcOf = true;
					pc &= 0xFFFF;
				}
				return 1;
				break;
			case 0x1F:
				ret = mem[pc++];
				if (pc > 0xFFFF)
				{
					pcOf = true;
					pc &= 0xFFFF;
				}
				return 1;
				break;
			default:
				return -1;
		}
	}
	return 0;
}

int DCPU16::retb(USHORT b, USHORT &ret)
{
	if (b > 0x1F)
		return -1;
	if (0x0 <= b && b <= 0x7)
		ret = reg[b];
	else if (0x8 <= b && b <= 0xF)
		ret = mem[reg[b - 0x8]];
	else if (0x10 <= b && b <= 0x17)
	{
		ret = mem[reg[b - 0x10] + mem[pc++]];
		if (pc > 0xFFFF)
		{
			pcOf = true;
			pc &= 0xFFFF;
		}
		return 1;
	}
	else
	{
		switch (b)
		{
			case 0x18:
				ret = mem[sp];
				break;
			case 0x19:
				ret = mem[sp];
				break;
			case 0x1A:
				ret = mem[sp + mem[pc++]];
				if (pc > 0xFFFF)
				{
					pcOf = true;
					pc &= 0xFFFF;
				}
				return 1;
				break;
			case 0x1B:
				ret = sp;
				break;
			case 0x1C:
				ret = pc;
				break;
			case 0x1D:
				ret = ex;
				break;
			case 0x1E:
				ret = mem[mem[pc++]];
				if (pc > 0xFFFF)
				{
					pcOf = true;
					pc &= 0xFFFF;
				}
				return 1;
				break;
			case 0x1F:
				ret = mem[pc++];
				if (pc > 0xFFFF)
				{
					pcOf = true;
					pc &= 0xFFFF;
				}
				return 1;
				break;
			default:
				return -1;
		}
	}
	return 0;
}

int DCPU16::setb(USHORT b, USHORT dat, int shift)
{
	if (b > 0x1F)
		return -1;
	if (0x0 <= b && b <= 0x7)
		reg[b] = dat;
	else if (0x8 <= b && b <= 0xF)
		mem[reg[b - 0x8]] = dat;
	else if (0x10 <= b && b <= 0x17)
		mem[reg[b - 0x10] + mem[pc - shift]] = dat;
	else
	{
		switch (b)
		{
			case 0x18:
				mem[--sp] = dat;
				break;
			case 0x19:
				mem[sp] = dat;
				break;
			case 0x1A:
				mem[sp + mem[pc - shift]] = dat;
				break;
			case 0x1B:
				sp = dat;
				break;
			case 0x1C:
				pc = dat;
				break;
			case 0x1D:
				ex = dat;
				break;
			case 0x1E:
				mem[mem[pc - shift]] = dat;
				break;
			default:
				return -1;
		}
	}
	return 0;
}

#endif