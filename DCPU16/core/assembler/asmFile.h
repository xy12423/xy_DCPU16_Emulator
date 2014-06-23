#pragma once

#ifndef _H_ASM_AF
#define _H_ASM_AF

#include "assembler.h"
#include <list>
#include <fstream>

USHORT arg_ret[65536];

struct pendItem
{
	std::string str;
	USHORT pos;
	int len;
	std::string file;
	int lineN;
	pendItem(){};
	pendItem(std::string _str, USHORT _pos, int _len, std::string _file, int _lineN)
	{
		str = _str;
		pos = _pos;
		len = _len;
		file = _file;
		lineN = _lineN;
	}
};
typedef std::list<pendItem> pendList;

struct label
{
	std::string str;
	USHORT pos, originPos;
	pendList used;
	label(){};
	label(std::string _str, USHORT _pos, USHORT _originPos)
	{
		str = _str;
		pos = _pos;
		originPos = _originPos;
	}
	bool operator <(const label &n)
	{
		return str.length() > n.str.length();
	}
};
typedef std::list<label> labelList;

struct defItem
{
	std::string name, val;
	defItem(){};
	defItem(std::string _name, std::string _val)
	{
		name = _name;
		val = _val;
	}
	bool operator <(const defItem &n)
	{
		return name.length() > n.name.length();
	}
	bool operator >(const defItem &n)
	{
		return name.length() < n.name.length();
	}
};
typedef std::list<defItem> defList;

bool preprocess(std::string path)
{
	std::list<std::ifstream> finLst;
	std::list<std::ifstream>::iterator fin;
	{
		int mkPos = path.find('\\');
		while (mkPos != std::string::npos)
		{
			path[mkPos] = '/';
			mkPos = path.find('\\');
		}
	}
	finLst.push_back(std::ifstream(path));
	std::list<std::string> fileName;
	fin = finLst.end();
	fin--;
	if (!fin->is_open())
	{
		throw(std::string("Can't Open File ") + path);
	}
	std::ofstream fout("pp_temp");

	char *line = new char[65536];
	int lineCount = 0;
	std::string insline;
	std::string dir = path.substr(0, path.find_last_of('/') + 1);
	std::string ppCmd, ppArg;
	bool printLine;

	defList defLst;
	defList::iterator defItr, defEnd;
	int layer = 1, skipping = 0;
	std::list<bool> elseRes;

	while (!finLst.empty())
	{
		if (skipping > 0)
			printLine = false;
		else
			printLine = true;
		fin->getline(line, 65536, '\n');
		insline = line;
		trim(insline);
		if (insline.length() > 0)
		{
			if (insline.front() == '#')
			{
				insline.erase(0, 1);
				int markPos = insline.find(' ');
				if (markPos == std::string::npos)
				{
					ppCmd = insline;
					ppArg = "";
				}
				else
				{
					ppCmd = insline.substr(0, markPos);
					ppArg = insline.substr(markPos + 1);
				}
				trim(ppCmd);
				trim(ppArg);
				if (ppCmd == "ifdef")
					layer++;
				else if (ppCmd == "ifndef")
					layer++;
				else if (ppCmd == "endif")
					layer--;
				else if (ppCmd == "else")
					layer--;
				if (skipping != 0 && layer < skipping)
					skipping = 0;
				if (skipping == 0)
				{
					if (ppCmd == "include")
					{
						int mkPos = 0;
						mkPos = ppArg.find('\\');
						while (mkPos != std::string::npos)
						{
							ppArg[mkPos] = '/';
							mkPos = ppArg.find('\\');
						}
						if (ppArg.front() == '"' && ppArg.back() == '"')
						{
							ppArg.erase(0, 1);
							ppArg.pop_back();
						}
						ppArg = dir + ppArg;
						std::ifstream icFile(ppArg);
						if (!icFile.is_open())
						{
							fin->close();
							finLst.pop_back();
							while (!finLst.empty())
							{
								fin--;
								fin->close();
								finLst.pop_back();
							}
							fout.close();
							throw(std::string("Preprocesser:Can't open file ") + ppArg);
						}
						icFile.close();
						fout << "#file " << ppArg << std::endl;
						finLst.push_back(std::ifstream(ppArg));
						fin = finLst.end();
						fin--;
						fileName.push_back(ppArg);
						dir = ppArg.substr(0, ppArg.find_last_of('/') + 1);
						printLine = false;
					}
					else if (ppCmd == "define")
					{
						markPos = ppArg.find(' ');
						if (markPos == std::string::npos)
						{
							ppCmd = ppArg;
							ppArg = "";
						}
						else
						{
							ppCmd = ppArg.substr(0, markPos);
							ppArg = ppArg.substr(markPos + 1);
						}
						defItem newDefItm(ppCmd, ppArg);
						defEnd = defLst.end();
						for (defItr = defLst.begin(); defItr != defEnd; defItr++)
							if ((*defItr) > newDefItm)
								break;
						defLst.insert(defItr, newDefItm);
					}
					else if (ppCmd == "undef")
					{
						defEnd = defLst.end();
						for (defItr = defLst.begin(); defItr != defEnd; defItr++)
						{
							if (defItr->name == ppArg)
							{
								defLst.erase(defItr);
								break;
							}
						}
					}
					else if (ppCmd == "ifdef")
					{
						bool nFound = true;
						defEnd = defLst.end();
						for (defItr = defLst.begin(); defItr != defEnd; defItr++)
						{
							if (defItr->name == ppArg)
							{
								nFound = false;
								break;
							}
						}
						printLine = false;
						if (nFound)
							skipping = layer;
						elseRes.push_back(!nFound);
					}
					else if (ppCmd == "ifndef")
					{
						bool found = false;
						defEnd = defLst.end();
						for (defItr = defLst.begin(); defItr != defEnd; defItr++)
						{
							if (defItr->name == ppArg)
							{
								found = true;
								break;
							}
						}
						printLine = false;
						if (found)
							skipping = layer;
						elseRes.push_back(!found);
					}
					else if (ppCmd == "else")
					{
						if (elseRes.back())
							skipping = layer;
						printLine = false;
					}
					else if (ppCmd == "endif")
					{
						printLine = false;
						elseRes.pop_back();
					}
					else if (ppCmd == "file" || ppCmd == "/file")
					{
						fin->close();
						finLst.pop_back();
						while (!finLst.empty())
						{
							fin--;
							fin->close();
							finLst.pop_back();
						}
						fout.close();
						throw(std::string("Preprocesser:Illegal preprocess command #file"));
					}
				}
				if (ppCmd == "else")
					layer++;
			}
		}
		if (printLine)
			fout << line;
		fout << std::endl;
		if (fin->eof())
		{
			fin->close();
			finLst.pop_back();
			fin = finLst.end();
			if (!finLst.empty())
				fin--;
			if (!fileName.empty())
			{
				std::string parentFilename = fileName.back();
				fout << "#/file " << parentFilename << std::endl;
				dir = parentFilename.substr(0, parentFilename.find_last_of('/') + 1);
				fileName.pop_back();
			}
		}
	}

	while (!finLst.empty())
	{
		finLst.end()->close();
		finLst.pop_back();
	}
	fout.close();
	return false;
}

bool spacer[] = {
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
	true, false, false, false, false, false, false, false,
	false, false, true, true, true, true, false, true,
	false, false, false, false, false, false, false, false,
	false, false, true, true, false, false, false, false,
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
	false, false, false, true, false, true, false, false,
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,
};

int asmFile(std::string path, USHORT retMem[], int retSize, USHORT wAdd = 0, bool printLabel = false, labelList* retLblLst = NULL)
{
	if (preprocess(path))	//处理部分预处理
		return -1;
	std::ifstream file("pp_temp");
	std::list<std::string> fileName;
	std::list<int> lineCountLst;
	std::list<int>::iterator lineCount;
	fileName.push_back(path);
	lineCountLst.push_back(0);
	lineCount = lineCountLst.end();
	lineCount--;
	if (!file.is_open())
	{
		throw(std::string("Can't Open File ") + path);
	}

	char *line = new char[65536];
	int *m = new int[0x200000];
	bool *joined = new bool[0x200000];
	memset(m, -1, sizeof(int) * 0x200000);
	memset(joined, false, sizeof(bool) * 0x200000);

	try
	{
		std::string insline;
		int markPos = std::string::npos;
		USHORT sysLblCount = 0;
		std::string sysLabel;

		labelList lblLst;
		pendList pendLst;
		std::string lbl;
		int pendCount = 0;
		labelList::iterator lblBeg, lblItr, lblEnd;
		std::list<labelList::iterator> lblUsedLst;
		std::list<labelList::iterator>::const_iterator usedItr, usedEnd;
		pendItem pendItm;

		std::string ppCmd, ppArg;
		defList defLst;
		defList::iterator defItr, defEnd;

		USHORT add = wAdd, addShift = wAdd;
		int len = 0, pendLen = 3, i;

		while (!file.eof())
		{
			(*lineCount)++;
			file.getline(line, 65536, '\n');
			insline = insline + line;
			trim(insline);
			if (insline.length() < 1)
				continue;
			markPos = insline.find(';');
			if (markPos != std::string::npos) //注释
			{
				if (markPos == 0)
					insline = "";
				else
				{
					insline.erase(markPos - 1);
					trim(insline);
				}
			}
			markPos = insline.find(':'); //标签
			if (markPos == 0)
			{
				markPos = insline.find(' ');
				if (markPos == std::string::npos)
				{
					lbl = insline.substr(1);
					insline = "";
				}
				else
				{
					lbl = insline.substr(1, markPos - 1);
					insline.erase(0, markPos + 1);
				}
				if (lbl.find('$') != std::string::npos)
				{
					throw(std::string(fileName.back() + ':' + toStr(*lineCount) + ":Illgeal label:" + lbl));
				}
				lblItr = lblLst.begin();
				lblEnd = lblLst.end();
				for (lblItr = lblLst.begin(); lblItr != lblEnd; lblItr++)
				{
					if (lblItr->str == lbl)
					{
						throw(std::string(fileName.back() + ':' + toStr(*lineCount) + ":Duplicate label:" + lbl));
					}
				}
				lblLst.push_back(label(lbl, add, add));
			}
			if (insline.length() < 1)
				continue;
			if (insline.front() == '#') //预处理指令
			{
				insline.erase(0, 1);
				markPos = insline.find(' ');
				if (markPos == std::string::npos)
				{
					ppCmd = insline;
					ppArg = "";
				}
				else
				{
					ppCmd = insline.substr(0, markPos);
					ppArg = insline.substr(markPos + 1);
				}
				trim(ppCmd);
				lcase(ppCmd);
				trim(ppArg);
				if (ppCmd == "define")
				{
					markPos = ppArg.find(' ');
					if (markPos == std::string::npos)
					{
						ppCmd = ppArg;
						ppArg = "";
					}
					else
					{
						ppCmd = ppArg.substr(0, markPos);
						ppArg = ppArg.substr(markPos + 1);
					}
					defItem newDefItm(ppCmd, ppArg);
					defEnd = defLst.end();
					for (defItr = defLst.begin(); defItr != defEnd; defItr++)
						if ((*defItr) > newDefItm)
							break;
					defLst.insert(defItr, newDefItm);
				}
				else if (ppCmd == "undef")
				{
					defEnd = defLst.end();
					for (defItr = defLst.begin(); defItr != defEnd; defItr++)
						if (defItr->name == ppArg)
						{
						defLst.erase(defItr);
						break;
						}
				}
				else if (ppCmd == "file")
				{
					fileName.push_back(ppArg);
					lineCountLst.push_back(0);
					lineCount = lineCountLst.end();
					lineCount--;
				}
				else if (ppCmd == "/file")
				{
					fileName.pop_back();
					lineCountLst.pop_back();
					lineCount = lineCountLst.end();
					if (!lineCountLst.empty())
						lineCount--;
				}
				else
				{
					throw(std::string(fileName.back() + ':' + toStr(*lineCount) + ":Undefined preprocess command"));
				}
				insline = "";
				continue;
			}
			markPos = insline.find('$'); //类似于this的东西
			if (markPos != std::string::npos)
			{
				sysLabel = "$__asm_sys_label_" + toHEX(sysLblCount);
				sysLblCount++;
				lblLst.push_back(label(sysLabel, add, add));
				insline = insline.substr(0, markPos) + sysLabel + insline.substr(markPos + 1);
			}
			if (insline.length() < 1)
				continue;
			defEnd = defLst.end();	//替换宏定义
			for (defItr = defLst.begin(); defItr != defEnd; defItr++)
			{
				markPos = insline.find(defItr->name);
				while (markPos != std::string::npos)
				{
					if ((markPos == 0 ? true : spacer[insline[markPos - 1]]) &&
						(markPos == insline.length() - defItr->name.length() ? true : spacer[insline[markPos + defItr->name.length()]]))	//防止#define a之类后dat之类被替换成dt
						insline = insline.substr(0, markPos) + defItr->val + insline.substr(markPos + defItr->name.length());
					markPos = insline.find(defItr->name, min(markPos + 1, (int)(insline.length() - 1)));
				}
			}
			if (insline.back() == ',')
				continue;
			len = assembler(insline, arg_ret, 65536);
			switch (len)
			{
				case _ERR_ASM_NOT_SUPPORTED:
					throw(std::string(fileName.back() + ':' + toStr(*lineCount) + ":Instruction " + line + " is not supported"));
				case _ERR_ASM_ILLEGAL:
				case _ERR_ASM_ILLEGAL_OP:
					throw(std::string(fileName.back() + ':' + toStr(*lineCount) + ":Illegal instruction " + line));
				case _ERR_ASM_ILLEGAL_ARG:
					pendLst.push_back(pendItem(insline, add, 0x20, fileName.back(), *lineCount));
					joined[add - addShift] = true;
					//为含有未能识别的标签的代码留出空间
					add += 0x20;
					pendCount++;
					break;
				default:
					for (i = 0; i < len; i++, add++)
						m[add - addShift] = arg_ret[i];
			}
			insline = "";
		}
		lblLst.sort();	//按标签长度从大到小排序以解决长标签与短标签内容部分重复时长标签被部分替换的问题
		lblBeg = lblLst.begin();
		lblEnd = lblLst.end();
		if (add > retSize)
		{
			throw(std::string("Out of memory"));
		}
		int insLenAll = add;
		//处理未被识别的标签
		while (!pendLst.empty())
		{
			pendItm = pendLst.front();
			pendLst.pop_front();
			insline = pendItm.str;
			add = pendItm.pos;
			if (add < addShift)
			{
				throw(std::string("Assembler Error"));
			}
			joined[add - addShift] = false;
			pendLen = pendItm.len;
			for (i = 0; i < pendLen; i++)
				m[add + i - addShift] = -1;
			lblUsedLst.clear();
			for (lblItr = lblBeg; lblItr != lblEnd; lblItr++)
			{
				markPos = insline.find(lblItr->str);
				bool used = false;
				while (markPos != std::string::npos)
				{
					if ((markPos == 0 ? true : spacer[insline[markPos - 1]]) &&
						(markPos == insline.length() - lblItr->str.length() ? true : spacer[insline[markPos + lblItr->str.length()]]))	//与处理宏定义时的超长判断用途一致
					{
						used = true;
						insline = insline.substr(0, markPos) + "0x" + toHEX(lblItr->pos) + insline.substr(markPos + lblItr->str.length());
					}
					markPos = insline.find(lblItr->str, min(markPos + 1, (int)(insline.length() - 1)));
				}
				if (used)
					lblUsedLst.push_back(lblItr);
			}
			len = assembler(insline, arg_ret, 65536);
			pendItm.len = len;
			usedEnd = lblUsedLst.cend();
			//汇编完之后再将指令加入 标签被引用列表 以防止指令长度不对
			for (usedItr = lblUsedLst.cbegin(); usedItr != usedEnd; usedItr++)
			{
				lblItr = *usedItr;
				pendList *usedList = &(lblItr->used);
				pendList::const_iterator itr, itrEnd = usedList->cend();
				for (itr = usedList->cbegin(); itr != itrEnd; itr++)
				{
					if (itr->pos == add)
					{
						usedList->erase(itr);
						break;
					}
				}
				usedList->push_back(pendItm);
			}
			switch (len)
			{
				case _ERR_ASM_NOT_SUPPORTED:
					throw(std::string(pendItm.file + ':' + toStr(pendItm.lineN) + ":Instruction " + pendItm.str + " is not supported"));
				case _ERR_ASM_ILLEGAL:
				case _ERR_ASM_ILLEGAL_OP:
				case _ERR_ASM_ILLEGAL_ARG:
					throw(std::string(pendItm.file + ':' + toStr(pendItm.lineN) + ":Illegal instruction " + pendItm.str));
				default:
					for (i = 0; i < len; i++, add++)
						m[add - addShift] = arg_ret[i];
					//由于指令实际长度比预留空间短，在该指令后面的标签需要往前凑
					if (len < pendLen)
					{
						pendLen = pendLen - len;
						for (lblItr = lblBeg; lblItr != lblEnd; lblItr++)
						{
							if (lblItr->originPos > add)
							{
								lblItr->pos -= pendLen;
								//将含有被修改地址的标签的指令重新加入处理队列以用新值替换旧值
								pendList *usedList = &(lblItr->used);
								pendList::const_iterator itr, itrEnd = usedList->cend();
								for (itr = usedList->cbegin(); itr != itrEnd; itr++)
								{
									if (!joined[itr->pos])
									{
										pendLst.push_back(*itr);
										joined[itr->pos] = true;
									}
								}
							}
						}
					}
			}
		}
		if (printLabel)
		{
			retLblLst->clear();
			for (lblItr = lblBeg; lblItr != lblEnd; lblItr++)
				retLblLst->push_back(label(lblItr->str, lblItr->pos, 0));
		}
		add = wAdd;
		int retLen = 0;
		int emptyCount = 0;
		for (i = 0; add < insLenAll && i < 0x200000; i++)
		{
			if (m[i] < 0)
			{
				continue;
				emptyCount++;
			}
			retLen++;
			retMem[add++] = m[i];
		}
		return retLen;
	}
	catch (std::string err)
	{
		file.close();
		delete[] line;
		delete[] m;
		throw(err);
	}
	file.close();
	delete[] line;
	delete[] m;
	return 0;
}

#endif