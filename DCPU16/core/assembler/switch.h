#pragma once

#ifndef _H_ASM_SW
#define _H_ASM_SW

#include "define.h"

int retOpNum2(std::string op, USHORT &ret)
{
	switch (op[0])
	{
		case 'd':
			if (op[1] == 'a' && op[2] == 't')
				ret = 0x20;
			else
				return _ERR_ASM_ILLEGAL_OP;
			break;
		case 'h':
			if (op[1] == 'w')
			{
				if (op[2] == 'i')
					ret = 0x12;
				else if (op[2] == 'n')
					ret = 0x10;
				else if (op[2] == 'q')
					ret = 0x11;
				else
					return _ERR_ASM_ILLEGAL_OP;
				break;
			}
			else
				return _ERR_ASM_ILLEGAL_OP;
			break;
		case 'i':
			if (op[1] == 'a')
			{
				if (op[2] == 'g')
					ret = 0x09;
				else if (op[2] == 'q')
					ret = 0x0C;
				else if (op[2] == 's')
					ret = 0x0A;
				else
					return _ERR_ASM_ILLEGAL_OP;
			}
			else if (op[1] == 'n' && op[2] == 't')
				ret = 0x08;
			else
				return _ERR_ASM_ILLEGAL_OP;
			break;
		case 'j':
			if (op[1] == 's' && op[2] == 'r')
				ret = 0x01;
			else
				return _ERR_ASM_ILLEGAL_OP;
			break;
		case 'r':
			if (op[1] == 'f' && op[2] == 'i')
				ret = 0x0B;
			else
				return _ERR_ASM_ILLEGAL_OP;
			break;
		default:
			return _ERR_ASM_ILLEGAL_OP;
	}
	return _ERR_ASM_NOERR;
}

int retOpNum3(std::string op, USHORT &ret)
{
	switch (op[0])
	{
		case 'a':
			switch (op[1])
			{
				case 'd':
					if (op[2] == 'd')
						ret = 0x02;
					else if (op[2] == 'x')
						ret = 0x1A;
					else
						return _ERR_ASM_ILLEGAL_OP;
					break;
				case 'n':
					if (op[2] == 'd')
						ret = 0x0A;
					else
						return _ERR_ASM_ILLEGAL_OP;
					break;
				case 's':
					if (op[2] == 'r')
						ret = 0x0E;
					else
						return _ERR_ASM_ILLEGAL_OP;
					break;
				default:
					return _ERR_ASM_ILLEGAL_OP;
			}
			break;
		case 'b':
			if (op[1] == 'o' && op[2] == 'r')
				ret = 0x0B;
			else
				return _ERR_ASM_ILLEGAL_OP;
			break;
		case 'd':
			if (op[1] == 'i' && op[2] == 'v')
				ret = 0x06;
			else if (op[1] == 'v' && op[2] == 'i')
				ret = 0x07;
			else
				return _ERR_ASM_ILLEGAL_OP;
			break;
		case 'i':
			if (op[1] == 'f')
			{
				switch (op[2])
				{
					case 'a':
						ret = 0x15;
						break;
					case 'b':
						ret = 0x10;
						break;
					case 'c':
						ret = 0x11;
						break;
					case 'e':
						ret = 0x12;
						break;
					case 'g':
						ret = 0x14;
						break;
					case 'l':
						ret = 0x16;
						break;
					case 'n':
						ret = 0x13;
						break;
					case 'u':
						ret = 0x17;
						break;
					default:
						return _ERR_ASM_ILLEGAL_OP;
				}
			}
			else
				return _ERR_ASM_ILLEGAL_OP;
			break;
		case 'm':
			if (op[1] == 'd' && op[2] == 'i')
				ret = 0x09;
			else if (op[1] == 'l' && op[2] == 'i')
				ret = 0x05;
			else if (op[1] == 'o' && op[2] == 'd')
				ret = 0x08;
			else if (op[1] == 'u' && op[2] == 'l')
				ret = 0x04;
			else
				return _ERR_ASM_ILLEGAL_OP;
			break;
		case 's':
			switch (op[1])
			{
				case 'b':
					if (op[2] == 'x')
						ret = 0x1B;
					else
						return _ERR_ASM_ILLEGAL_OP;
					break;
				case 'e':
					if (op[2] == 't')
						ret = 0x01;
					else
						return _ERR_ASM_ILLEGAL_OP;
					break;
				case 'h':
					if (op[2] == 'l')
						ret = 0x0F;
					else if (op[2] == 'r')
						ret = 0x0D;
					else
						return _ERR_ASM_ILLEGAL_OP;
					break;
				case 't':
					if (op[2] == 'd')
						ret = 0x1F;
					else if (op[2] == 'i')
						ret = 0x1E;
					else
						return _ERR_ASM_ILLEGAL_OP;
					break;
				case 'u':
					if (op[2] == 'b')
						ret = 0x03;
					else
						return _ERR_ASM_ILLEGAL_OP;
					break;
				default:
					return _ERR_ASM_ILLEGAL_OP;
			}
			break;
		case 'x':
			if (op[1] == 'o' && op[2] == 'r')
				ret = 0x0C;
			else
				return _ERR_ASM_ILLEGAL_OP;
			break;
		default:
			return _ERR_ASM_ILLEGAL_OP;
	}
	return _ERR_ASM_NOERR;
}

int retOpStr1(USHORT op, std::string &ret)
{
	return _ERR_ASM_ILLEGAL;
}

std::string op2[] = {
	std::string(""),
	std::string("JSR"),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string("INT"),
	std::string("IAG"),
	std::string("IAS"),
	std::string("RFI"),
	std::string("IAQ"),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string("HWN"),
	std::string("HWQ"),
	std::string("HWI"),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string(""),
	std::string("")
};

int retOpStr2(USHORT op, std::string &ret)
{
	if (op == 0)
		return _ERR_ASM_NOT_SUPPORTED;
	else if (op < 0x20)
		ret = op2[op];
	else
		return _ERR_ASM_ILLEGAL;
	if (ret == "")
		return _ERR_ASM_ILLEGAL;
	return _ERR_ASM_NOERR;
}

std::string op3[] = {
	std::string(""),
	std::string("SET"),
	std::string("ADD"),
	std::string("SUB"),
	std::string("MUL"),
	std::string("MLI"),
	std::string("DIV"),
	std::string("DVI"),
	std::string("MOD"),
	std::string("MDI"),
	std::string("AND"),
	std::string("BOR"),
	std::string("XOR"),
	std::string("SHR"),
	std::string("ASR"),
	std::string("SHL"),
	std::string("IFB"),
	std::string("IFC"),
	std::string("IFE"),
	std::string("IFN"),
	std::string("IFG"),
	std::string("IFA"),
	std::string("IFL"),
	std::string("IFU"),
	std::string(""),
	std::string(""),
	std::string("ADX"),
	std::string("SBX"),
	std::string(""),
	std::string(""),
	std::string("STI"),
	std::string("STD")
};

int retOpStr3(USHORT op, std::string &ret)
{
	if (op == 0)
		return _ERR_ASM_NOT_SUPPORTED;
	else if (op < 0x20)
		ret = op3[op];
	else
		return _ERR_ASM_ILLEGAL;
	if (ret == "")
		return _ERR_ASM_ILLEGAL;
	return _ERR_ASM_NOERR;
}

int retGRegNum(std::string reg, USHORT &ret)
{
	switch (reg[0])
	{
		case 'a':
			ret = 0x00;
			break;
		case 'b':
			ret = 0x01;
			break;
		case 'c':
			ret = 0x02;
			break;
		case 'x':
			ret = 0x03;
			break;
		case 'y':
			ret = 0x04;
			break;
		case 'z':
			ret = 0x05;
			break;
		case 'i':
			ret = 0x06;
			break;
		case 'j':
			ret = 0x07;
			break;
		default:
			return _ERR_ASM_ILLEGAL_ARG;
	}
	return _ERR_ASM_NOERR;
}

std::string gReg[] = {
	std::string("a"),
	std::string("b"),
	std::string("c"),
	std::string("x"),
	std::string("y"),
	std::string("z"),
	std::string("i"),
	std::string("j")
};

int retGRegStr(USHORT reg, std::string &ret)
{
	if (reg < 8)
		ret = gReg[reg];
	else
		return _ERR_ASM_ILLEGAL;
	return _ERR_ASM_NOERR;
}

int retArgNum(std::string arg, USHORT &ret1, USHORT &ret2)
{
	int inslen = 1;
	long long temp = 0;
	if (arg[0] == '[')
	{
		arg.erase(0, 1);
		arg.erase(arg.length() - 1, 1);
		if (calcStr(arg, temp) == 0)
			arg = "0x" + toHEX((USHORT)(temp));
		if (arg.length() < 1)
			return _ERR_ASM_ILLEGAL;
		int plusPos = arg.find('+');
		if (plusPos != std::string::npos)
		{
			if (arg.length() < 3)
				return _ERR_ASM_ILLEGAL;
			std::string reg = arg.substr(0, plusPos);
			std::string shift = arg.substr(plusPos + 1);
			bool again = true;
		_ran_begin:
			if (reg == "sp")
				ret1 = 0x1A;
			else
			{
				if (retGRegNum(reg, ret1) != _ERR_ASM_NOERR)
				{
					if (again)
					{
						swap(reg, shift);
						again = false;
						goto _ran_begin;
					}
					else
						return _ERR_ASM_ILLEGAL_ARG;
				}
				ret1 += 0x10;
			}
			if (!canBeNum(shift))
			{
				if (again)
				{
					swap(reg, shift);
					again = false;
					goto _ran_begin;
				}
				else
					return _ERR_ASM_ILLEGAL_ARG;
			}
			ret2 = (USHORT)toNum(shift);
			inslen = 2;
		}
		else if (canBeNum(arg))
		{
			ret1 = 0x1E;
			ret2 = toNum(arg);
			inslen = 2;
		}
		else
		{
			if (arg == "sp")
				ret1 = 0x19;
			else
			{
				if (retGRegNum(arg, ret1) != _ERR_ASM_NOERR)
					return _ERR_ASM_ILLEGAL_ARG;
				ret1 += 0x08;
			}
		}
	}
	else
	{
		if (calcStr(arg, temp) == 0)
			arg = "0x" + toHEX((USHORT)(temp));
		if (canBeNum(arg))
		{
			short int n = (USHORT)toNum(arg);
			if (-1 <= n && n <= 30)
				ret1 = (USHORT)(n + 0x21);
			else
			{
				ret1 = 0x1F;
				ret2 = (USHORT)(n);
				inslen = 2;
			}
		}
		else
		{
			int arglen = arg.length();
			if (arglen == 1)
			{
				if (retGRegNum(arg, ret1) != _ERR_ASM_NOERR)
					return _ERR_ASM_ILLEGAL_ARG;
			}
			else if (arglen == 2)
			{
				if (arg == "sp")
					ret1 = 0x1B;
				else if (arg == "pc")
					ret1 = 0x1C;
				else if (arg == "ex")
					ret1 = 0x1D;
				else
					return _ERR_ASM_ILLEGAL_ARG;
			}
			else if (arglen == 3)
			{
				if (arg == "pop")
					ret1 = 0x18;
				else
					return _ERR_ASM_ILLEGAL_ARG;
			}
			else if (arglen == 4 && arg[0] == 'p')
			{
				if (arg == "push")
					ret1 = 0x18;
				else if (arg == "peek")
					ret1 = 0x19;
				else
					return _ERR_ASM_ILLEGAL_ARG;
			}
			else if (arg.substr(0, 4) == "pick")
			{
				ret1 = 0x1A;
				ret2 = (USHORT)toNum(arg.substr(5));
				inslen = 2;
			}
			else
				return _ERR_ASM_ILLEGAL_ARG;
		}
	}
	return inslen;
}

int retArgStr(USHORT arg1, USHORT arg2, bool isB, std::string &ret)
{
	int inc = 0;
	if ((isB && arg1 > 0x1F) || ((!isB) && arg1 > 0x3F))
		return _ERR_ASM_ILLEGAL;
	else if (0x00 <= arg1 && arg1 <= 0x07)
		retGRegStr(arg1, ret);
	else if (0x08 <= arg1 && arg1 <= 0x0F)
	{
		retGRegStr(arg1 - 0x08, ret);
		ret = "[" + ret + "]";
	}
	else if (0x10 <= arg1 && arg1 <= 0x17)
	{
		retGRegStr(arg1 - 0x10, ret);
		ret = "[" + ret + "+" + toHEX(arg2) + "]";
		inc = 1;
	}
	else
	{
		switch (arg1)
		{
			case 0x18:
				if (isB)
					ret = "push";
				else
					ret = "pop";
				break;
			case 0x19:
				ret = "peek";
				break;
			case 0x1A:
				ret = "pick " + toHEX(arg2);
				inc = 1;
				break;
			case 0x1B:
				ret = "sp";
				break;
			case 0x1C:
				ret = "pc";
				break;
			case 0x1D:
				ret = "ex";
				break;
			case 0x1E:
				ret = "[" + toHEX(arg2) + "]";
				inc = 1;
				break;
			case 0x1F:
				ret = toHEX(arg2);
				inc = 1;
				break;
			default:
				ret = toHEX((USHORT)(arg1 - 0x21));
				break;
		}
	}
	return inc;
}

#endif