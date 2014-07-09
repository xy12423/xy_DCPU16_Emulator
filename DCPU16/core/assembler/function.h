#pragma once

#ifndef _H_ASM_FU
#define _H_ASM_FU

#include "define.h"
#include <sstream>
#include <climits>
#include <cctype>

#ifndef _H_EMU_FU

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

#endif

void ltrim(std::string &str)
{
	int len = str.length();
	int i;
	for (i = 0; i < len; i++)
		if (!isspace(str[i]))
			break;
	str.erase(0, i);
}

void rtrim(std::string &str)
{
	int len = str.length();
	int i;
	for (i = len - 1; i > -1; i--)
		if (!isspace(str[i]))
			break;
	if (i < len - 1)
		str.erase(i + 1);

}

void trim(std::string &str)
{
	ltrim(str);
	rtrim(str);
}

void lcase(std::string &str)
{
	int len = str.length();
	for (int i = 0; i < len; i++)
		str[i] = tolower(str[i]);
}

void ucase(std::string &str)
{
	int len = str.length();
	for (int i = 0; i < len; i++)
		str[i] = toupper(str[i]);
}

int numLevel[] = {
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5,
	5, 2, 2, 2, 2, 2, 2, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

bool canBeNum(std::string str)
{
	int len = str.length(), level = -1;
	if (len == 0)
		return false;
	lcase(str);
	if (str.front() == '-' || str.front() == '+')
	{
		str.erase(0, 1);
		len--;
	}
	if (str.back() == 'h' || str.back() == 'H')
	{
		level = 2;
		str.erase(len - 1, 1);
		len--;
	}
	else if (str.front() == '0' && len > 1)
	{
		if (len > 2 && str[1] == 'x')
		{
			level = 2;
			str.erase(0, 2);
			len -= 2;
		}
		else
		{
			level = 0;
			str.erase(0, 1);
			len--;
		}
	}
	else if (str.front() == '\'' && str.back() == '\'')
		return str.length() == 3;
	else
		level = 1;
	if (len < 1)
		return false;
	for (int i = 0; i < len; i++)
		if (numLevel[static_cast<BYTE>(str[i])] > level)
			return false;
	return true;
}

int toNum(std::string str, int type = 0)
{
	int ret = INT_MIN, len = str.length();
	if (len < 1)
		return INT_MIN;
	std::stringstream ss;
	bool negative = false;
	if (str.front() == '-')
	{
		str.erase(0, 1);
		negative = true;
		len--;
	}
	else if (str.front() == '+')
	{
		str.erase(0, 1);
		len--;
	}
	switch (type)
	{
		case 0:
			if (len == 0)
				return INT_MIN;
			if (str.back() == 'h' || str.back() == 'H')
			{
				str.erase(len - 1, 1);
				ss << std::hex << str;
				ss >> ret;
			}
			else if (str.front() == '0' && len > 1)
			{
				if (len > 2 && str[1] == 'x')
				{
					str.erase(0, 2);
					ss << std::hex << str;
					ss >> ret;
				}
				else
				{
					str.erase(0, 1);
					ss << std::oct << str;
					ss >> ret;
				}
			}
			else if (str.front() == '\'' && str.back() == '\'')
			{
				if (str.length() == 3)
					ret = (int)(str[1]);
			}
			else
			{
				ss << std::dec << str;
				ss >> ret;
			}
			break;
		case 4:
			if (str.front() == '\'' && str.back() == '\'')
				str.erase(0, 1);
			ret = (int)(str[0]);
			break;
		case 3:
			ss << std::hex << str;
			ss >> ret;
			break;
		case 2:
			ss << std::dec << str;
			ss >> ret;
			break;
		case 1:
			ss << std::oct << str;
			ss >> ret;
			break;
	}
	if (negative)
		ret = -ret;
	return ret;
}

std::string toHEX(unsigned int n)
{
	std::stringstream ss;
	std::string ret;
	ss << std::hex << n;
	ss >> ret;
	if (ret.length() < 4)
		ret = std::string(4 - ret.length(), '0') + ret;
	ucase(ret);
	return ret;
}

std::string toStr(unsigned int n)
{
	std::stringstream ss;
	std::string ret;
	ss << std::dec << n;
	ss >> ret;
	return ret;
}

long long power(int a, int b)
{
	long long ret = 1;
	for (int i = 0; i < b; i++)
		ret *= a;
	return ret;
}

int opLv[128] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 2, 1, 2, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

struct postExpUnit
{
	std::string str;
	BYTE type;
	postExpUnit(){};
	postExpUnit(std::string _str, BYTE _type)
	{
		str = _str;
		type = _type;
	};
};
typedef std::list < postExpUnit > postExp;

postExp* in2Post(std::string str)
{
	std::string num, tmp;
	postExp* ret = new postExp;
	std::list<char> opStack;
	std::list<std::string> bracketStack;
	std::string::const_iterator p, end = str.cend();
	for (p = str.cbegin(); p != end; p++)
	{
		if (opLv[*p] != 0)
		{
			if (*p == '(')
			{
				bracketStack.push_back(num);
				num = "";
				opStack.push_back(*p);
				continue;
			}
			else
			{
				ret->push_back(postExpUnit(num, 0));
				num = "";
				if (*p == ',')
					continue;
				else if (*p == ')')
				{
					if (opStack.empty())
					{
						delete ret;
						return NULL;
					}
					while (opStack.back() != '(')
					{
						ret->push_back(postExpUnit(std::string(1, opStack.back()), 1));
						opStack.pop_back();
						if (opStack.empty())
						{
							delete ret;
							return NULL;
						}
					}
					opStack.pop_back();
					if (bracketStack.back() != "")
						ret->push_back(postExpUnit(bracketStack.back(), 1));
					bracketStack.pop_back();
				}
				else
				{
					while ((!opStack.empty()) && opLv[*p] <= opLv[opStack.back()])
					{
						ret->push_back(postExpUnit(std::string(1, opStack.back()), 1));
						opStack.pop_back();
					}
					opStack.push_back(*p);
				}
			}
		}
		else
		{
			num.push_back(*p);
		}
	}
	if (num != "")
		ret->push_back(postExpUnit(num, 0));
	while (!opStack.empty())
	{
		ret->push_back(postExpUnit(std::string(1, opStack.back()), 1));
		opStack.pop_back();
	}
	return ret;
}

int calcPost(postExp* exp, long long &ret)
{
	std::list<long long> numStack;
	postExp::const_iterator p, pEnd = exp->cend();
	long long temp1 = 0, temp2 = 0;
	for (p = exp->cbegin(); p != pEnd; p++)
	{
		switch (p->type)
		{
			case 0:
				if (canBeNum(p->str))
					numStack.push_back(toNum(p->str));
				else
					return -1;
				break;
			case 1:
				if (numStack.empty())
					return -1;
				temp2 = numStack.back();
				numStack.pop_back();
				if (numStack.empty())
					return -1;
				temp1 = numStack.back();
				numStack.pop_back();
				switch (p->str.front())
				{
					case '+':
						numStack.push_back(temp1 + temp2);
						break;
					case '-':
						numStack.push_back(temp1 - temp2);
						break;
					case '*':
						numStack.push_back(temp1 * temp2);
						break;
					case '/':
						numStack.push_back(temp1 / temp2);
						break;
					case '^':
						numStack.push_back(power((int)(temp1), (int)(temp2)));
						break;
					default:
						return -1;
				}
				break;
		}
	}
	if (numStack.size() != 1)
		return -1;
	ret = numStack.back();
	return 0;
}

int calcStr(std::string str, long long &ret)
{
	postExp *exp = in2Post(str);
	if (exp == NULL)
		return -1;
	int res = calcPost(exp, ret);
	delete exp;
	return res;
}

std::list<std::string>* divideStr(std::string str, char sep)
{
	std::list<std::string> *ret = new std::list<std::string>;
	if (str.length() < 1)
		return ret;
	/*
	int dotPos = str.find(sep);
	while (dotPos != std::string::npos)
	{
		ret->push_back(str.substr(0, dotPos));
		str.erase(0, dotPos + 1);
		dotPos = str.find(sep);
	}
	ret->push_back(str);
	*/
	if (sep == '"')
	{
		int dotPos = str.find('"');
		while (dotPos != std::string::npos)
		{
			ret->push_back(str.substr(0, dotPos));
			str.erase(0, dotPos + 1);
			dotPos = str.find('"');
		}
		ret->push_back(str);
	}
	else
	{
		std::string::const_iterator pBeg = str.cbegin(), p, pEnd = str.cend();
		std::string strtmp;
		bool isstr = false;
		for (p = pBeg; p != pEnd; p++)
		{
			if (*p == '"' && (p == pBeg ? true : *(p - 1) != '\\'))
				isstr = !isstr;
			if ((!isstr) && *p == sep)
			{
				ret->push_back(strtmp);
				strtmp.clear();
			}
			else
				strtmp.push_back(*p);
		}
		ret->push_back(strtmp);
	}
	return ret;
}

#endif