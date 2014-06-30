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

std::list<std::string>* preprocess(std::string path)
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

	std::list<std::string>* err = new std::list < std::string > ;

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
							err->push_back("Preprocesser:Can't open file " + ppArg);
						}
						else
						{
							icFile.close();
							fout << "#file " << ppArg << std::endl;
							finLst.push_back(std::ifstream(ppArg));
							fin = finLst.end();
							fin--;
							fileName.push_back(ppArg);
							dir = ppArg.substr(0, ppArg.find_last_of('/') + 1);
							printLine = false;
						}
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
						err->push_back("Preprocesser:Illegal preprocess command #file");
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
	return err;
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
	std::list<std::string>* err = preprocess(path);	//������Ԥ����
	if (err->empty() == false)
		throw(err);
	std::ifstream file("pp_temp");
	std::list<std::string> fileName;
	std::list<int> lineCountLst;
	std::list<int>::iterator lineCount;
	fileName.push_back(path);
	lineCountLst.push_back(0);
	lineCount = lineCountLst.end();
	lineCount--;
	if (!file.is_open())
		throw(std::string("Can't Open File ") + path);

	char *line = new char[65536];
	int *m = new int[0x200000];
	bool *joined = new bool[0x200000];
	memset(m, -1, sizeof(int) * 0x200000);
	memset(joined, false, sizeof(bool) * 0x200000);

	int retLen = 0;

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
			if (markPos != std::string::npos) //ע��
			{
				if (markPos == 0)
					insline = "";
				else
				{
					insline.erase(markPos - 1);
					trim(insline);
				}
			}
			markPos = insline.find(':'); //��ǩ
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
					err->push_back(fileName.back() + ':' + toStr(*lineCount) + ":Illgeal label:" + lbl);
				}
				else
				{
					lblItr = lblLst.begin();
					lblEnd = lblLst.end();
					bool success = true;
					for (lblItr = lblLst.begin(); lblItr != lblEnd; lblItr++)
					{
						if (lblItr->str == lbl)
						{
							err->push_back(fileName.back() + ':' + toStr(*lineCount) + ":Duplicate label:" + lbl);
							success = false;
						}
					}
					if (success)
						lblLst.push_back(label(lbl, add, add));
				}
			}
			if (insline.length() < 1)
				continue;
			if (insline.front() == '#') //Ԥ����ָ��
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
					err->push_back(fileName.back() + ':' + toStr(*lineCount) + ":Undefined preprocess command");
				}
				insline = "";
				continue;
			}
			markPos = insline.find('$'); //������this�Ķ���
			if (markPos != std::string::npos)
			{
				sysLabel = "$__asm_sys_label_" + toHEX(sysLblCount);
				sysLblCount++;
				lblLst.push_back(label(sysLabel, add, add));
				insline = insline.substr(0, markPos) + sysLabel + insline.substr(markPos + 1);
			}
			if (insline.length() < 1)
				continue;
			defEnd = defLst.end();	//�滻�궨��
			for (defItr = defLst.begin(); defItr != defEnd; defItr++)
			{
				markPos = insline.find(defItr->name);
				while (markPos != std::string::npos)
				{
					if ((markPos == 0 ? true : spacer[insline[markPos - 1]]) &&
						(markPos == insline.length() - defItr->name.length() ? true : spacer[insline[markPos + defItr->name.length()]]))	//��ֹ#define a֮���dat֮�౻�滻��dt
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
					err->push_back(fileName.back() + ':' + toStr(*lineCount) + ":Instruction " + line + " is not supported");
				case _ERR_ASM_ILLEGAL:
				case _ERR_ASM_ILLEGAL_OP:
					err->push_back(fileName.back() + ':' + toStr(*lineCount) + ":Illegal instruction " + line);
				case _ERR_ASM_ILLEGAL_ARG:
					pendLst.push_back(pendItem(insline, add, 0x20, fileName.back(), *lineCount));
					joined[add - addShift] = true;
					//Ϊ����δ��ʶ��ı�ǩ�Ĵ��������ռ�
					add += 0x20;
					pendCount++;
					break;
				case _ERR_ASM_TOO_LONG:
					err->push_back("Instruction is too long");
				default:
					for (i = 0; i < len; i++, add++)
						m[add - addShift] = arg_ret[i];
			}
			insline = "";
		}
		if (add > retSize)
			err->push_back("Instruction is too long");
		if (err->empty() == false)
			throw(err);
		lblLst.sort();	//����ǩ���ȴӴ�С�����Խ������ǩ��̱�ǩ���ݲ����ظ�ʱ����ǩ�������滻������
		lblBeg = lblLst.begin();
		lblEnd = lblLst.end();
		int insLenAll = add;
		//����δ��ʶ��ı�ǩ
		while (!pendLst.empty())
		{
			pendItm = pendLst.front();
			pendLst.pop_front();
			insline = pendItm.str;
			add = pendItm.pos;
			if (add < addShift)
			{
				err->push_back("Assembler Error");
				throw(err);
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
						(markPos == insline.length() - lblItr->str.length() ? true : spacer[insline[markPos + lblItr->str.length()]]))	//�봦��궨��ʱ�ĳ����ж���;һ��
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
			//�����֮���ٽ�ָ����� ��ǩ�������б� �Է�ָֹ��Ȳ���
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
					err->push_back(pendItm.file + ':' + toStr(pendItm.lineN) + ":Instruction " + pendItm.str + " is not supported");
				case _ERR_ASM_ILLEGAL:
				case _ERR_ASM_ILLEGAL_OP:
				case _ERR_ASM_ILLEGAL_ARG:
					err->push_back(pendItm.file + ':' + toStr(pendItm.lineN) + ":Illegal instruction " + pendItm.str);
				case _ERR_ASM_TOO_LONG:
					err->push_back("Instruction is too long");
				default:
					for (i = 0; i < len; i++, add++)
						m[add - addShift] = arg_ret[i];
					//����ָ��ʵ�ʳ��ȱ�Ԥ���ռ�̣��ڸ�ָ�����ı�ǩ��Ҫ��ǰ��
					if (len < pendLen)
					{
						pendLen = pendLen - len;
						for (lblItr = lblBeg; lblItr != lblEnd; lblItr++)
						{
							if (lblItr->originPos > add)
							{
								lblItr->pos -= pendLen;
								//�����б��޸ĵ�ַ�ı�ǩ��ָ�����¼��봦�����������ֵ�滻��ֵ
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
		if (err->empty() == false)
			throw(err);
		if (printLabel && retLblLst != NULL)
		{
			retLblLst->clear();
			for (lblItr = lblBeg; lblItr != lblEnd; lblItr++)
				retLblLst->push_back(label(lblItr->str, lblItr->pos, 0));
		}
		add = wAdd;
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
	}
	catch (std::list<std::string>* err)
	{
		file.close();
		delete[] line;
		delete[] m;
		throw(err);
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
	return retLen;
}

#endif