#pragma once

#ifndef _H_ASM_AS
#define _H_ASM_AS

#include <string>
#include "define.hpp"
#include "function.hpp"
#include "switch.hpp"

int errD = 0;

void preprcs(std::string &op, std::string &b, std::string &a, int &codeType)
{
	switch (op.front())
	{
		case 'c':
			if (op == "call")
			{
				if (codeType == 1)
				{
					b = a;
					a = "";
					codeType = 2;
				}
			}
			break;
		case 'd':
			if (op == "dat")
			{
				if (b != "")
				{
					a = b + "," + a;
					b = "";
					codeType = 1;
				}
			}
			break;
		case 'g':
			if (op == "goto")
			{
				op = "set";
				b = "pc";
				codeType = 2;
			}
			break;
		case 'm':
			if (op == "mov")
				op = "set";
			break;
		case 'p':
			if (op == "pop")
			{
				op = "set";
				b = a;
				a = "pop";
				codeType = 2;
			}
			else if (op == "push")
			{
				op = "set";
				b = "push";
				codeType = 2;
			}
			break;
		case 'r':
			if (op == "ret" && codeType == 0)
			{
				op = "set";
				b = "pc";
				a = "pop";
				codeType = 2;
			}
			break;
	}
	int bracketPos = b.find('[');
	if (bracketPos != std::string::npos && bracketPos != 0 && b.back() == ']')
	{
		b.pop_back();
		b = "[" + b.substr(0, bracketPos) + "+(" + b.substr(bracketPos + 1) + ")]";
	}
	bracketPos = a.find('[');
	if (bracketPos != std::string::npos && bracketPos != 0 && a.back() == ']')
	{
		a.pop_back();
		a = "[" + a.substr(0, bracketPos) + "+(" + a.substr(bracketPos + 1) + ")]";
	}
}

struct escchr
{
	std::string str;
	char val;
};

escchr escChr[] = {
	"\\n", '\n',
	"\\t", '\t',
	"\\\"", '"',
	"\\'", '\'',
};
const int escChrCount = sizeof(escChr) / sizeof(escchr);

int assembler(std::string code, USHORT ret[], int retSize)
{
	if (retSize < 1)
		return _ERR_ASM_TOO_LONG;
	trim(code);
	if (code.length() < 1)
		return _ERR_ASM_ILLEGAL;
	opcode retop;
	retop.op = 0;
	retop.b = 0;
	retop.a = 0;
	int len = code.length();
	int codeLen = 1;
	bool setRETOP = true;
	std::string op, b, a;
	int codeType = -1;
	{	//将指令分解为指令、操作数两部分
		int spacePos = code.find(' ');
		if (spacePos == std::string::npos)
		{
			b = "";
			a = "";
			op = code;
			codeType = 0;
		}
		else
		{
			op = code.substr(0, spacePos);
			int dotPos = code.find(',');
			if (dotPos == std::string::npos)
			{
				b = "";
				a = code.substr(spacePos + 1);
				codeType = 1;
			}
			else if (dotPos >= len - 1)
				return _ERR_ASM_ILLEGAL;
			else
			{
				b = code.substr(spacePos + 1, dotPos - spacePos - 1);
				a = code.substr(dotPos + 1);
				codeType = 2;
			}
		}
	}
	trim(b);
	trim(a);
	trim(op);
	lcase(op);
	preprcs(op, b, a, codeType);	//识别并处理 部分伪指令及一些奇怪的功能
	int retVal = 0;
	switch (codeType)	//按操作数多少分类
	{
		case 0:	//没有操作数
			retop.op = 0;
			retVal = retOpNum1(op, retop.a);
			if (retVal != _ERR_ASM_NOERR)
				return retVal;
			break;
		case 1:	//1个操作数
			retop.op = 0;
			retVal = retOpNum2(op, retop.b);
			if (retVal != _ERR_ASM_NOERR)
				return retVal;
			if (retop.b > 0x1F)	//如果是伪指令
			{
				retop.b -= 0x20;
				std::list<std::string> *datList;
				std::list<std::string>::iterator pItr, pEnd;
				USHORT nw = 0;
				long long temp = 0;
				switch (retop.b)
				{
					case 0x00:	//伪指令dat
						datList = divideStr(a, ',');
						codeLen = 0;
						pEnd = datList->end();
						for (pItr = datList->begin(); pItr != pEnd; pItr++)
						{
							if (codeLen >= retSize)
								return _ERR_ASM_TOO_LONG;
							trim(*pItr);
							if (calcStr(*pItr, temp) == 0)
							{
								if (temp > 0xFFFF || temp < -32768)
									return _ERR_ASM_ILLEGAL;
								ret[codeLen++] = static_cast<USHORT>(temp);
							}
							else if (pItr->front() == '[' && pItr->back() == ']')
							{
								pItr->erase(0, 1);
								pItr->pop_back();
								for (int i = static_cast<int>(toNum(*pItr)); i > 0 && codeLen < retSize; i--)
									ret[codeLen++] = 0;
							}
							else if (pItr->front() == '"' && pItr->back() == '"')
							{
								pItr->erase(0, 1);
								pItr->pop_back();
								int i, chrPos;
								for (i = 0; i < escChrCount; i++)	//替换转义字符
								{
									chrPos = pItr->find(escChr[i].str);
									while (chrPos != std::string::npos)
									{
										*pItr = pItr->substr(0, chrPos) + escChr[i].val + pItr->substr(chrPos + escChr[i].str.length());
										chrPos = pItr->find(escChr[i].str);
									}
								}
								std::string::const_iterator p = pItr->cbegin();
								for (i = pItr->length(); i > 0 && codeLen < retSize; i--, p++)
									ret[codeLen++] = *p;
							}
							else
								return _ERR_ASM_ILLEGAL_ARG;
						}
						setRETOP = false;
						delete datList;
						break;
				}
			}
			else //普通指令
			{
				USHORT nw = 0;
				lcase(a);
				retVal = retArgNum(a, retop.a, nw, true);
				if (retVal < 0)
					return retVal;
				else if (retVal == 2)
				{
					if (codeLen >= retSize)
						return _ERR_ASM_TOO_LONG;
					ret[codeLen++] = nw;
				}
			}
			break;
		case 2:	//2个操作数
			retVal = retOpNum3(op, retop.op);
			if (retVal != _ERR_ASM_NOERR)
				return retVal;
			if (retop.op > 0x1F)	//如果是伪指令
			{
				retop.op -= 0x20;
				USHORT nw = 0;
				std::list<std::string> *argList;
				std::list<std::string>::iterator pItr, pEnd;
				int argc = 0;
				switch (retop.op)
				{
					case 0x01:	//伪指令call
						codeLen = 0;
						if (codeLen + 1 > retSize)
							return _ERR_ASM_TOO_LONG;
						ret[codeLen++] = 0xA363;
						lcase(a);
						argList = divideStr(a, ',');
						argList->reverse();
						pEnd = argList->end();
						retop.op = 0x01;
						retop.b = 0x18;
						for (pItr = argList->begin(); pItr != pEnd; pItr++)
						{
							retVal = retArgNum(*pItr, retop.a, nw, true);
							if (retVal < 0)
								return retVal;
							else
							{
								if (codeLen + retVal > retSize)
									return _ERR_ASM_TOO_LONG;
								ret[codeLen++] = OP2US(retop);
								if (retVal == 2)
									ret[codeLen++] = nw;
								argc++;
							}
						}
						delete argList;
						argc += 7;
						if (codeLen + 1 > retSize)
							return _ERR_ASM_TOO_LONG;
						retop.op = 0x02;
						retop.b = 0x1B;
						if (argc < 0x1F)
						{
							retop.a = static_cast<USHORT>(argc) + 0x21;
							ret[codeLen++] = OP2US(retop);
						}
						else
						{
							retop.a = 0x1F;
							ret[codeLen++] = OP2US(retop);
							if (codeLen + 1 > retSize)
								return _ERR_ASM_TOO_LONG;
							ret[codeLen++] = static_cast<USHORT>(argc);
						}
						argc -= 7;
						if (codeLen + 7 > retSize)
							return _ERR_ASM_TOO_LONG;
						ret[codeLen++] = 0x0701;
						ret[codeLen++] = 0x0B01;
						ret[codeLen++] = 0x0F01;
						ret[codeLen++] = 0x1301;
						ret[codeLen++] = 0x1701;
						ret[codeLen++] = 0x1B01;
						ret[codeLen++] = 0x1F01;
						lcase(b);
						retop.op = 0x00;
						retop.b = 0x01;
						retVal = retArgNum(b, retop.a, nw, true);
						if (retVal < 0)
							return retVal;
						else
						{
							if (codeLen + retVal > retSize)
								return _ERR_ASM_TOO_LONG;
							ret[codeLen++] = OP2US(retop);
							if (retVal == 2)
								ret[codeLen++] = nw;
						}
						if (argc > 0)
						{
							if (codeLen + 1 > retSize)
								return _ERR_ASM_TOO_LONG;
							retop.op = 0x02;
							retop.b = 0x1B;
							if (argc < 0x1F)
							{
								retop.a = static_cast<USHORT>(argc)+0x21;
								ret[codeLen++] = OP2US(retop);
							}
							else
							{
								retop.a = 0x1F;
								ret[codeLen++] = OP2US(retop);
								if (codeLen + 1 > retSize)
									return _ERR_ASM_TOO_LONG;
								ret[codeLen++] = static_cast<USHORT>(argc);
							}
						}
						if (codeLen + 7 > retSize)
							return _ERR_ASM_TOO_LONG;
						ret[codeLen++] = 0x60E1;
						ret[codeLen++] = 0x60C1;
						ret[codeLen++] = 0x60A1;
						ret[codeLen++] = 0x6081;
						ret[codeLen++] = 0x6061;
						ret[codeLen++] = 0x6041;
						ret[codeLen++] = 0x6021;
						setRETOP = false;
						break;
				}
			}
			else //普通指令
			{
				USHORT nw = 0;
				lcase(b);
				retVal = retArgNum(b, retop.b, nw, false);
				if (codeLen + retVal > retSize)
					return _ERR_ASM_TOO_LONG;
				if (retVal < 0)
					return retVal;
				else if (retVal == 2)
					ret[codeLen++] = nw;
				nw = 0;
				lcase(a);
				retVal = retArgNum(a, retop.a, nw, true);
				if (codeLen + retVal > retSize)
					return _ERR_ASM_TOO_LONG;
				if (retVal < 0)
					return retVal;
				else if (retVal == 2)
					ret[codeLen++] = nw;
			}
			break;
	}
	if (setRETOP)
		ret[0] = OP2US(retop);
	return codeLen;
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