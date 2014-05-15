#pragma once

#ifndef _H_ASM_AS
#define _H_ASM_AS

#include <string>
#include "define.h"
#include "function.h"
#include "switch.h"

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
	}
	int bracketPos = b.find('[');
	if (bracketPos != std::string::npos && bracketPos != 0 && b.back() == ']')
		b = "[" + b.substr(0, bracketPos) + "+" + b.substr(bracketPos + 1);
	bracketPos = a.find('[');
	if (bracketPos != std::string::npos && bracketPos != 0 && a.back() == ']')
		a = "[" + a.substr(0, bracketPos) + "+" + a.substr(bracketPos + 1);
}

int assembler(std::string code, USHORT ret[], int retLen)
{
	trim(code);
	if (code.length() < 1)
		return _ERR_ASM_ILLEGAL;
	{
		std::list<std::string> *cmdList = divideStr(code, ';');
		if (cmdList->size() > 1)
		{
			std::list<std::string>::const_iterator p, pEnd = cmdList->cend();
			USHORT *tRet = new USHORT[65536];
			int codeLen = 0, retCode = 0, i;
			for (p = cmdList->cbegin(); p != pEnd; p++)
			{
				if (p->length() < 1)
					continue;
				retCode = assembler(*p, tRet, 65536);
				if (retCode < 1)
					return retCode;
				for (i = 0; i < retCode && codeLen < retLen; i++)
					ret[codeLen++] = tRet[i];
				if (codeLen > retLen)
				{
					delete tRet;
					delete cmdList;
					return _ERR_ASM_TOO_LONG;
				}
			}
			delete tRet;
			delete cmdList;
			return codeLen;
		}
		delete cmdList;
	}
	opcode retop;
	retop.op = 0;
	retop.b = 0;
	retop.a = 0;
	int len = code.length();
	int codelen = 1;
	bool setRetOP = true;
	std::string op, b, a;
	int codeType = -1;
	{
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
	preprcs(op, b, a, codeType);
	lcase(op);
	int retcode;
	switch (codeType)
	{
		case 0:
			retop.op = 0;
			retcode = retOpNum1(op, retop.a);
			if (retcode != _ERR_ASM_NOERR)
				return retcode;
			break;
		case 1:
			retop.op = 0;
			retcode = retOpNum2(op, retop.b);
			if (retcode != _ERR_ASM_NOERR)
				return retcode;
			if (retop.b > 0x1F)
			{
				retop.b -= 0x20;
				std::list<std::string> *datList;
				std::list<std::string>::iterator pItr, pEnd;
				USHORT nw = 0;
				switch (retop.b)
				{
					case 0x00:
						datList = divideStr(a, ',');
						codelen = 0;
						pEnd = datList->end();
						for (pItr = datList->begin(); pItr != pEnd; pItr++)
						{
							if (canBeNum(*pItr))
								ret[codelen++] = toNum(*pItr);
							else if (pItr->front() == '[' && pItr->back() == ']')
							{
								pItr->erase(0, 1);
								pItr->pop_back();
								for (int i = toNum(*pItr); i > 0 && codelen < retLen; i--)
									ret[codelen++] = 0;
							}
							else if (pItr->front() == '"' && pItr->back() == '"')
							{
								pItr->erase(0, 1);
								pItr->pop_back();
								std::string::const_iterator p = pItr->cbegin();
								for (int i = pItr->length(); i > 0 && codelen < retLen; i--, p++)
									ret[codelen++] = *p;
							}
							else
								return _ERR_ASM_ILLEGAL_ARG;
						}
						setRetOP = false;
						delete datList;
						break;
					case 0x02:
						lcase(a);
						codelen = 0;
						retop.op = 0x01;
						retop.b = 0x00;
						retcode = retArgNum(a, retop.a, nw);
						if (retcode < 0)
							return retcode;
						else
						{
							ret[codelen++] = OP2US(retop);
							if (retcode == 2)
								ret[codelen++] = nw;
						}
						ret[codelen++] = 0x6701;
						ret[codelen++] = 0x0341;
						ret[codelen++] = 0x0001;
						ret[codelen++] = 0x6381;
						setRetOP = false;
						break;
				}
			}
			else
			{
				USHORT nw = 0;
				lcase(a);
				retcode = retArgNum(a, retop.a, nw);
				if (retcode < 0)
					return retcode;
				else if (retcode == 2)
					ret[codelen++] = nw;
			}
			break;
		case 2:
			retcode = retOpNum3(op, retop.op);
			if (retcode != _ERR_ASM_NOERR)
				return retcode;
			if (retop.op > 0x1F)
			{
				retop.op -= 0x20;
				USHORT nw = 0;
				std::list<std::string> *argList;
				std::list<std::string>::iterator pItr, pEnd;
				int argc = 0;
				switch (retop.op)
				{
					case 0x01:
						codelen = 0;
						ret[codelen++] = 0x0301;
						ret[codelen++] = 0x0701;
						ret[codelen++] = 0x0B01;
						ret[codelen++] = 0x0F01;
						ret[codelen++] = 0x1301;
						ret[codelen++] = 0x1701;
						ret[codelen++] = 0x1B01;
						ret[codelen++] = 0x1F01;
						lcase(a);
						argList = divideStr(a, ',');
						argList->reverse();
						pEnd = argList->end();
						retop.op = 0x01;
						retop.b = 0x18;
						for (pItr = argList->begin(); pItr != pEnd; pItr++)
						{
							retcode = retArgNum(*pItr, retop.a, nw);
							if (retcode < 0)
								return retcode;
							else
							{
								ret[codelen++] = OP2US(retop);
								if (retcode == 2)
									ret[codelen++] = nw;
								argc++;
							}
						}
						delete argList;
						lcase(b);
						retop.op = 0x00;
						retop.b = 0x01;
						retcode = retArgNum(b, retop.a, nw);
						if (retcode < 0)
							return retcode;
						else
						{
							ret[codelen++] = OP2US(retop);
							if (retcode == 2)
								ret[codelen++] = nw;
						}
						ret[codelen++] = 0x6001;
						if (argc > 0)
						{
							retop.op = 0x02;
							retop.b = 0x1B;
							if (argc < 0x1F)
							{
								retop.a = (USHORT)(argc)+0x21;
								ret[codelen++] = OP2US(retop);
							}
							else
							{
								retop.a = 0x1F;
								ret[codelen++] = OP2US(retop);
								ret[codelen++] = (USHORT)(argc);
							}
						}
						ret[codelen++] = 0x60E1;
						ret[codelen++] = 0x60C1;
						ret[codelen++] = 0x60A1;
						ret[codelen++] = 0x6081;
						ret[codelen++] = 0x6061;
						ret[codelen++] = 0x6041;
						ret[codelen++] = 0x6021;
						ret[codelen++] = 0x0301;
						ret[codelen++] = 0x6801;
						ret[codelen++] = 0x0001;
						ret[codelen++] = 0x6741;
						ret[codelen++] = 0x0001;
						ret[codelen++] = 0x8B62;
						setRetOP = false;
						break;
				}
			}
			else
			{
				USHORT nw = 0;
				lcase(b);
				retcode = retArgNum(b, retop.b, nw);
				if (retcode < 0)
					return retcode;
				else if (retcode == 2)
					ret[codelen++] = nw;
				nw = 0;
				lcase(a);
				retcode = retArgNum(a, retop.a, nw);
				if (retcode < 0)
					return retcode;
				else if (retcode == 2)
					ret[codelen++] = nw;
			}
			break;
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