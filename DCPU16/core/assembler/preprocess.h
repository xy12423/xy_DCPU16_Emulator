#pragma once

#ifndef _H_ASM_PP
#define _H_ASM_PP

#include "define.h"

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

#endif