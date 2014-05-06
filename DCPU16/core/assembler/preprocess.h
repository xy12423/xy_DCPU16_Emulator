#pragma once

#ifndef _H_ASM_PP
#define _H_ASM_PP

#include "define.h"

void preprcs(std::string &op, std::string &b, std::string &a)
{
	switch (op.front())
	{
		case 'm':
			if (op == "mov")
				op = "set";
			break;
		case 'd':
			if (op == "dat")
			{
				if (b != "")
				{
					a = b + "," + a;
					b = "";
				}
			}
			break;
		case 'p':
			if (op == "pop")
			{
				op = "set";
				b = a;
				a = "pop";
			}
			else if (op == "push")
			{
				op = "set";
				b = "push";
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