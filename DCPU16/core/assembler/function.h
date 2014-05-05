#pragma once

#ifndef _H_ASM_FU
#define _H_ASM_FU

#include "define.h"
#include <limits.h>

void ltrim(std::string &str)
{
	int len = str.length();
	int i;
	for (i = 0; i < len;i++)
		if (str[i] != ' ' && str[i] != '\t')
			break;
	str.erase(0, i);
}

void rtrim(std::string &str)
{
	int len = str.length();
	int i;
	for (i = len - 1; i > -1; i--)
		if (str[i] != ' ' && str[i] != '\t')
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
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 3, 3, 
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
3, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

bool canBeNum(std::string str)
{
	int len = str.length(), level = -1;
	if (len == 0)
		return false;
	lcase(str);
	if (str[0] == '0' && len > 1)
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
	else if (str[len - 1] == 'h')
	{
		level = 2;
		str.erase(len - 1, 1);
		len--;
	}
	else
		level = 1;
	for (int i = 0; i < len; i++)
		if (numLevel[(BYTE)(str[i])] > level)
			return false;
	return true;
}

int toNum(std::string str, int type = 3)
{
	int ret = INT_MIN, len = str.length();
	if (len < 1)
		return 0;
	std::stringstream ss;
	if (str[len - 1] == 'h')
	{
		str.erase(len - 1, 1);
		ss << std::hex << str;
		ss >> ret;
		//sscanf(str.c_str(), "%x", &ret);
	}
	else
	{
		switch (type)
		{
			case 3:
				//sscanf(str.c_str(), "%i", &ret);
				if (len == 0)
					return false;
				if (str[0] == '0' && len > 1)
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
				else if (str[len - 1] == 'h')
				{
					str.erase(len - 1, 1);
					ss << std::hex << str;
					ss >> ret;
				}
				else
				{
					ss << std::dec << str;
					ss >> ret;
				}
				break;
			case 2:
				//sscanf(str.c_str(), "%x", &ret);
				ss << std::hex << str;
				ss >> ret;
				break;
			case 1:
				//sscanf(str.c_str(), "%d", &ret);
				ss << std::dec << str;
				ss >> ret;
				break;
			case 0:
				//sscanf(str.c_str(), "%o", &ret);
				ss << std::oct << str;
				ss >> ret;
				break;
		}
	}
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

int opLv[128] = { 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

long long power(int a, int b)
{
	long long ret = 1;
	for (int i = 0; i < b; i++)
		ret *= a;
	return ret;
}

int calcStr(std::string str, long long &ret)
{
	std::string num;
	std::list<long long> numStack;
	std::list<char> opStack;
	std::string::const_iterator p, end = str.cend();
	long long temp;
	for (p = str.cbegin(); p != end; p++)
	{
		if (opLv[*p] != 0)
		{
			if (!canBeNum(num))
				return -1;
			numStack.push_back(toNum(num));
			num = "";
			if (opStack.empty())
			{
				opStack.push_back(*p);
				continue;
			}
			while ((!opStack.empty()) && opLv[*p] <= opLv[opStack.back()])
			{
				switch (opStack.back())
				{
					case '+':
						temp = numStack.back();
						numStack.pop_back();
						temp += numStack.back();
						numStack.pop_back();
						numStack.push_back(temp);
						break;
					case '-':
						temp = numStack.back();
						numStack.pop_back();
						temp = numStack.back() - temp;
						numStack.pop_back();
						numStack.push_back(temp);
						break;
					case '*':
						temp = numStack.back();
						numStack.pop_back();
						temp *= numStack.back();
						numStack.pop_back();
						numStack.push_back(temp);
						break;
					case '/':
						temp = numStack.back();
						numStack.pop_back();
						temp = numStack.back() / temp;
						numStack.pop_back();
						numStack.push_back(temp);
						break;
					case '^':
						temp = numStack.back();
						numStack.pop_back();
						temp = power((int)(numStack.back()), (int)(temp));
						numStack.pop_back();
						numStack.push_back(temp);
						break;
					default:
						return -1;
				}
				opStack.pop_back();
			}
			opStack.push_back(*p);
		}
		else
			num.push_back(*p);
	}
	if (!canBeNum(num))
		return -1;
	numStack.push_back(toNum(num));
	while (!opStack.empty())
	{
		switch (opStack.back())
		{
			case '+':
				temp = numStack.back();
				numStack.pop_back();
				temp += numStack.back();
				numStack.pop_back();
				numStack.push_back(temp);
				break;
			case '-':
				temp = numStack.back();
				numStack.pop_back();
				temp = numStack.back() - temp;
				numStack.pop_back();
				numStack.push_back(temp);
				break;
			case '*':
				temp = numStack.back();
				numStack.pop_back();
				temp *= numStack.back();
				numStack.pop_back();
				numStack.push_back(temp);
				break;
			case '/':
				temp = numStack.back();
				numStack.pop_back();
				temp = numStack.back() / temp;
				numStack.pop_back();
				numStack.push_back(temp);
				break;
			case '^':
				temp = numStack.back();
				numStack.pop_back();
				temp = power((int)(numStack.back()), (int)(temp));
				numStack.pop_back();
				numStack.push_back(temp);
				break;
			default:
				return -1;
		}
		opStack.pop_back();
	}
	ret = numStack.back();
	return 0;
}

#endif