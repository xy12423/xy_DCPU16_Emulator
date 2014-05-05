#pragma once

#ifndef _H_EMU_EM
#define _H_EMU_EM

#include "define.h"
#include "switch.h"
#include "function.h"
#include "export.h"

int doL1(opcode code)
{

	return -1;
}

int doL2(opcode code)
{
	USHORT op = code.b, a;
	if (op == 0)
		return doL1(code);
	int cycle = reta(code.a, a);
	if (cycle == -1)
		return -1;
	switch (op)
	{
		case 0x01:
			mem[--sp] = pc;
			pc = a;
			cycle += 3;
			break;
		case 0x08:
			if (ia != 0)
				additr(a);
			cycle += 4;
			break;
		case 0x09:
			setb(code.a, ia, cycle);
			cycle += 1;
			break;
		case 0x0A:
			ia = a;
			cycle += 1;
			break;
		case 0x0B:
			itri = true;
			reg[0] = mem[sp++];
			pc = mem[sp++];
			cycle += 3;
			break;
		case 0x0C:
			if (a == 0)
				itri = true;
			else
				itri = false;
			cycle += 2;
			break;
		case 0x10:
			setb(code.a, hwn, cycle);
			cycle += 2;
			break;
		case 0x11:
			if (a < hwn)
			{
				reg[0] = hwt[a].a;
				reg[1] = hwt[a].b;
				reg[2] = hwt[a].c;
				reg[3] = hwt[a].x;
				reg[4] = hwt[a].y;
			}
			cycle += 4;
			break;
		case 0x12:
			if (a < hwn)
				cycle += (4 + hwt[a].hwi());
			break;
		default:
			return -1;
	}
	return cycle;
}

int doL3()
{
	opcode ocode = US2OP(mem[pc++]);
	if (pc > 0xFFFF)
	{
		pcOf = true;
		pc &= 0xFFFF;
	}
	if (ocode.op == 0x00)
		return doL2(ocode);
	opcode code;
	int cycleA = 0, cycle = 0;
	code.op = ocode.op;
	cycleA = retb(ocode.b, code.b);
	cycleA += reta(ocode.a, code.a);
	UINT res = 0;
	USHORT op = code.op, b = code.b, a = code.a;
	switch (op)
	{
		case 0x01:
			res = a;
			cycle = cycleA + 1;
			break;
		case 0x02:
			res = b + a;
			ex = res >> 16;
			res = res & 0xFFFF;
			cycle = cycleA + 2;
			break;
		case 0x03:
			res = b - a;
			ex = res >> 16;
			res = res & 0xFFFF;
			cycle = cycleA + 2;
			break;
		case 0x04:
			res = b * a;
			ex = res >> 16;
			res = res & 0xFFFF;
			cycle = cycleA + 2;
			break;
		case 0x05:
			res = (UINT)((short int)(b) * (short int)(a));
			ex = res >> 16;
			res = res & 0xFFFF;
			cycle = cycleA + 2;
			break;
		case 0x06:
			if (a == 0)
			{
				res = 0;
				ex = 0;
			}
			else
			{
				res = (b / a) & 0xFFFF;
				ex = (b << 16) / a;
			}
			cycle = cycleA + 3;
			break;
		case 0x07:
			if (a == 0)
			{
				res = 0;
				ex = 0;
			}
			else
			{
				short int sa = (short int)(a), sb = (short int)(b);
				res = (USHORT)(sa / sb);
				ex = (USHORT)((b * 65536) / a);
			}
			cycle = cycleA + 3;
			break;
		case 0x08:
			if (a == 0)
				res = 0;
			else
				res = (USHORT)(b % a);
			cycle = cycleA + 3;
			break;
		case 0x09:
			if (a == 0)
				res = 0;
			else
			{
				short int sa = (short int)(a), sb = (short int)(b);
				res = (USHORT)(sa % sb);
			}
			cycle = cycleA + 3;
			break;
		case 0x0A:
			res = b & a;
			cycle = cycleA + 1;
			break;
		case 0x0B:
			res = b | a;
			cycle = cycleA + 1;
			break;
		case 0x0C:
			res = b ^ a;
			cycle = cycleA + 1;
			break;
		case 0x0D:
			res = b >> a;
			ex = (b << 16) >> a;
			cycle = cycleA + 1;
			break;
		case 0x0E:
			res = ((short)(b)) >> a;
			ex = ((short)(b << 16)) >> a;
			cycle = cycleA + 1;
			break;
		case 0x0F:
			res = b << a;
			ex = (b << a) >> 16;
			cycle = cycleA + 1;
			break;
		case 0x10:
			cycle = cycleA + 2;
			if ((b&a) == 0)
				cycle += skip();
			break;
		case 0x11:
			cycle = cycleA + 2;
			if ((b&a) != 0)
				cycle += skip();
			break;
		case 0x12:
			cycle = cycleA + 2;
			if (b != a)
				cycle += skip();
			break;
		case 0x13:
			cycle = cycleA + 2;
			if (b == a)
				cycle += skip();
			break;
		case 0x14:
			cycle = cycleA + 2;
			if (b <= a)
				cycle += skip();
			break;
		case 0x15:
			cycle = cycleA + 2;
			if ((short int)(b) <= (short int)(a))
				cycle += skip();
			break;
		case 0x16:
			cycle = cycleA + 2;
			if (b >= a)
				cycle += skip();
			break;
		case 0x17:
			cycle = cycleA + 2;
			if ((short int)(b) >= (short int)(a))
				cycle += skip();
			break;
		case 0x1A:
			res = b + a + ex;
			ex = res >> 16;
			res = res & 0xFFFF;
			cycle = cycleA + 3;
			break;
		case 0x1B:
			res = b - a + ex;
			ex = res >> 16;
			res = res & 0xFFFF;
			cycle = cycleA + 3;
			break;
		case 0x1E:
			res = a;
			reg[6]++;
			reg[7]++;
			cycle = cycleA + 2;
			break;
		case 0x1F:
			res = a;
			reg[6]--;
			reg[7]--;
			cycle = cycleA + 2;
			break;
		default:
			return -1;
	}
	if ((0x01 <= op && op <= 0x05) || op == 0x1A || op == 0x1B)
	{
		
	}
	if (op < 0x10 || 0x17 < op)
		setb(ocode.b, res, cycleA);
	return cycle;
}

int doCode()
{
	int cycle = doL3();
	if (cycle == -1)
		return _ERR_EMU_OTHER;
	if (itri)
	{
		USHORT itrt = 0;
		if (ia != 0 && popitr(itrt) != _ERR_EMU_ITR_EMPTY)
		{
			itri = false;
			mem[--sp] = pc;
			mem[--sp] = reg[0];
			pc = ia;
			reg[0] = itrt;
		}
	}
	return cycle;
}

volatile bool doCodeB;
volatile int cycleAll = 0;
void doCodeThread()
{
	doCodeB = true;
	int cyclePassed = 0;
	cycleAll = 0;
	pc = 0;
	int cycle = 0;
	clock_t start;
	while (doCodeB)
	{
		if (cycleAll < cyclePassed)
			continue;
		start = clock();
		cycle = doCode();
		if (cycle < 1)
			return;
		cyclePassed += cycle;
	}
}

#endif
