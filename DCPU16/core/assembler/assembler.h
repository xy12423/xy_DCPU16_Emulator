#pragma once

#ifndef _H_ASM_AS
#define _H_ASM_AS

#include <string>
#include "define.h"
#include "function.h"
#include "preprocess.h"
#include "switch.h"

int errD = 0;

int assembler(std::string code, USHORT ret[], int retLen)
{
	trim(code);
	lcase(code);
	opcode retop;
	retop.op = 0;
	retop.b = 0;
	retop.a = 0;
	int len = code.length();
	int codelen = 1;
	bool setRetOP = true;
	std::string op, b, a;
	{
		int spacePos = code.find(' ');
		if (spacePos == std::string::npos)
			return _ERR_ASM_ILLEGAL;
		op = code.substr(0, spacePos);
		int dotPos = code.find(',');
		if (dotPos == std::string::npos)
		{
			b = "";
			a = code.substr(spacePos + 1);
		}
		else if (dotPos >= len - 1)
			return _ERR_ASM_ILLEGAL;
		else
		{
			b = code.substr(spacePos + 1, dotPos - spacePos - 1);
			a = code.substr(dotPos + 1);
		}
	}
	trim(b);
	trim(a);
	preprcs(op, b, a);
	if (b.length() == 0)
	{
		retop.op = 0;
		int retcode = retOpNum2(op, retop.b);
		if (retcode != _ERR_ASM_NOERR)
			return retcode;
		if (retop.b > 0x1F)
		{
			retop.b -= 0x20;
			switch (retop.b)
			{
				case 0x00:
					int dotPos = a.find(',');
					codelen = 0;
					while (dotPos != std::string::npos && codelen < retLen)
					{
						ret[codelen++] = toNum(a.substr(0, dotPos));
						a.erase(0, dotPos + 1);
						dotPos = a.find(',');
					}
					ret[codelen++] = toNum(a);
					setRetOP = false;
					break;
			}
		}
		else
		{
			USHORT nw = 0;
			retcode = retArgNum(a, retop.a, nw);
			if (retcode < 0)
				return retcode;
			else if (retcode == 2)
				ret[codelen++] = nw;
		}
	}
	else
	{
		int retcode = retOpNum3(op, retop.op);
		if (retcode != _ERR_ASM_NOERR)
			return retcode;
		USHORT nw = 0;
		retcode = retArgNum(b, retop.b, nw);
		if (retcode < 0)
			return retcode;
		else if (retcode == 2)
			ret[codelen++] = nw;
		nw = 0;
		retcode = retArgNum(a, retop.a, nw);
		if (retcode < 0)
			return retcode;
		else if (retcode == 2)
			ret[codelen++] = nw;
		
	}
	if (setRetOP)
		ret[0] = OP2US(retop);
	return codelen;
}

int unassembler(USHORT arg1, USHORT arg2, USHORT arg3, std::string &ret)
{
	std::string b(""), a("");
	opcode op;
	int retcode, codetype = 3, codelen = 1;
	op = US2OP(arg1);
	ret = "";
	retcode = retOpStr3(op.op, ret);
	ret = ret + " ";
	if (retcode == _ERR_ASM_ILLEGAL)
			ret = "DAT " + toHEX(arg1);
	else if (retcode == _ERR_ASM_NOT_SUPPORTED)
	{
		codetype = 2;
		retcode = retOpStr2(op.b, ret);
		ret = ret + " ";
		if (retcode == _ERR_ASM_ILLEGAL)
			ret = "DAT " + toHEX(arg1);
		else if (retcode == _ERR_ASM_NOT_SUPPORTED)
		{
			codetype = 1;
			retcode = retOpStr1(op.a, ret);
			ret = ret + " ";
			if (retcode == _ERR_ASM_ILLEGAL)
				ret = "DAT " + toHEX(arg1);
		}
	}
	if (codetype == 3)
	{
		retcode = retArgStr(op.b, arg2, true, b);
		if (retcode == _ERR_ASM_ILLEGAL)
		{
			ret = "DAT " + toHEX(arg1);
			codetype = 0;
		}
		else if (retcode > -1)
		{
			ret = ret + b + ",";
			codelen += retcode;
		}
	}
	if (codetype > 1)
	{
		if (codetype == 3 && codelen > 1)
			retcode = retArgStr(op.a, arg3, false, a);
		else
			retcode = retArgStr(op.a, arg2, false, a);
		if (retcode == _ERR_ASM_ILLEGAL)
		{
			ret = "DAT " + toHEX(arg1);
			codetype = 0;
		}
		else if (retcode > -1)
		{
			ret = ret + a;
			codelen += retcode;
		}
	}
	return codelen;
}

#endif