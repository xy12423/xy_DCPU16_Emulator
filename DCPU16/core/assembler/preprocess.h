#pragma once

#ifndef _H_ASM_PP
#define _H_ASM_PP

#include "define.h"

void preprcs(std::string &op, std::string &b, std::string &a)
{
	switch (op[0])
	{
		case 'd':
			if (op.length() == 3 && op == "dat")
			{
				if (b != "")
				{
					a = b + "," + a;
					b = "";
				}
			}
			break;
		case 'p':
			if (op.length() == 3 && op == "pop")
			{
				op = "set";
				b = a;
				a = "pop";
			}
			else if (op.length() == 4 && op == "push")
			{
				op = "set";
				b = "push";
			}
			break;
	}
}

#endif