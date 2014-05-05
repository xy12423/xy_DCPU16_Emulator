#pragma once

#ifndef _H_EMU_FU
#define _H_EMU_FU

#include "define.h"

opcode US2OP(USHORT code)
{
	opcode ret;
	ret.op = code & 0x1F;
	ret.b = (code >> 5) & 0x1F;
	ret.a = code >> 10;
	return ret;
}

USHORT OP2US(opcode code)
{
	return ((code.a << 10) | (code.b << 5)) | code.op;
}

bool _skip_incpc[0x40] = {
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
	true, true, true, true, true, true, true, true,
	false, false, true, false, false, false, true, true,
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false
};

void skipsingle()
{
	opcode code = US2OP(mem[pc++]);
	if (_skip_incpc[code.a])
		pc++;
	if (_skip_incpc[code.b])
		pc++;
	if (pc > 0xFFFF)
	{
		pcOf = true;
		pc &= 0xFFFF;
	}
}

int skip()
{
	USHORT ocode = mem[pc++];
	opcode code = US2OP(ocode);
	int ret = 0;
	while (0x10 <= code.op && code.op <= 0x17)
	{
		if (_skip_incpc[code.a])
			pc++;
		if (_skip_incpc[code.b])
			pc++;
		if (pc > 0xFFFF)
		{
			pcOf = true;
			pc &= 0xFFFF;
		}
		ocode = mem[pc++];
		code = US2OP(ocode);
		ret++;
	}
	if (_skip_incpc[code.a])
		pc++;
	if (_skip_incpc[code.b])
		pc++;
	if (pc > 0xFFFF)
	{
		pcOf = true;
		pc &= 0xFFFF;
	}
	ret++;
	return ret;
}

int popitr(USHORT &_itr)
{
	if (itrp == itre)
		return _ERR_EMU_ITR_EMPTY;
	_itr = itr[itrp++];
	return 0;
}

#endif