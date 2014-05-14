#include "stdafx.h"
#include "core/main.h"
using namespace std;

#define INS_RET_LEN 65536

USHORT m_ret[INS_RET_LEN];
string m_arg[INS_RET_LEN];

#ifdef _P_WIN
DWORD threadID = 0;
DWORD WINAPI doCodeThreadBegin(LPVOID lpParam)
{
	doCodeThread();
	return 0;
}
#endif

void doCodeThreadStarter()
{
	doCodeB = true;
#ifdef _P_WIN
	CreateThread(NULL, 0, &doCodeThreadBegin, NULL, 0, &threadID);
#endif
#ifdef _P_LIN
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, &doCodeThread, NULL);
#endif
#ifdef _P_NA
#ifdef __CPP11_thread
	thread em(doCodeThread);
	em.detach();
#endif
#endif
}

#ifdef _P_WIN
DWORD_PTR zero = 0;
MMRESULT timerH = 0;
void CALLBACK timer(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	cycleAll += 100;
	if (!doCodeB)
		timeKillEvent(timerH);
}
#endif
#ifdef _P_LIN
pthread_attr_t attr;
pthread_t tid;
void timer()
{
	while (doCodeB)
	{
		cycleAll += 100;
		usleep(1000);
	}
}
#endif
#ifdef _P_NA
#ifdef __CPP11_thread

#endif
#include <ctime>
#if CLOCKS_PER_SEC < 1000
#error You need a better clock
#endif
void timer()
{
	clock_t start;
	while (doCodeB)
	{
		start = clock();
		cycleAll += 100;
		while (clock() - start < 1);
	}
}
#endif

void clockStarter()
{
#ifdef _P_WIN
	timerH = timeSetEvent(1, 1, timer, zero, TIME_PERIODIC);
#endif
#ifdef _P_LIN
	pthread_create(&tid, &attr, &timer, NULL);
#endif
#ifdef _P_NA
	thread tmr(timer);
	tmr.detach();
#endif
}

void assm(string arg)
{
	static USHORT add = 0;
	char in[50];
	string ins;
	if (arg != "")
		add = toNum(arg, 3);
	while (true)
	{
		printf("%04X:", add);
		cin.getline(in, 50, '\n');
		ins = in;
		if (ins.length() == 0)
			break;
		int len = assembler(ins, m_ret, INS_RET_LEN);
		switch (len)
		{
			case _ERR_ASM_NOT_SUPPORTED:
				cout << "Instruction is not supported" << endl;
				break;
			case _ERR_ASM_ILLEGAL:
			case _ERR_ASM_ILLEGAL_OP:
			case _ERR_ASM_ILLEGAL_ARG:
				cout << "Illegal instruction" << endl;
				break;
			default:
				for (int i = 0; i < len; i++, add++)
					mem[add] = m_ret[i];
		}
	}
	return;
}

void unasm(string arg)
{
	static USHORT add = 0;
	if (arg != "")
		add = toNum(arg);
	USHORT end = add + 0x40;
	string ins("");
	for (; add < end; )
	{
		printf("%04X:", add);
		add += unassembler(mem[add], mem[add + 1], mem[add + 2], ins);
		cout << ins << endl;
		if (mem[add] == 0)
			break;
	}
}

void dump(string arg)
{
	static UINT add = 0;
	if (arg != "")
		add = toNum(arg);
	int i, j;
	for (i = 0; i < 8; i++)
	{
		printf("%04X:", add);
		for (j = 0; j < 8; j++)
		{
			printf("%04X ", mem[add]);
			add++;
			if (add >= 0x10000)
			{
				add = 0;
				cout << endl;
				return;
			}
		}
		cout << endl;
	}
	return;
}

void enter(int argc, string args[])
{
	if (argc < 1)
	{
		cout << "  ^ Error" << endl;
		return;
	}
	UINT add = toNum(args[0]);
	for (int i = 1; i < argc && add < 0x10000; i++)
		mem[add++] = toNum(args[i]);
	return;
}

void regist(string arg = "")
{
	if (arg == "")
	{
		printf("A=%04X\tB=%04X\tC=%04X\tX=%04X\tY=%04X\tZ=%04X\tI=%04X\tJ=%04X\t", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5], reg[6], reg[7]);
		cout << endl;
		printf("PC=%04X\tSP=%04X\tEX=%04X\tIA=%04X\t", pc, sp, ex, ia);
		cout << endl;
	}
	else
	{
		USHORT regt = 0;
		int ret = retGRegNum(arg, regt);
		if (ret == _ERR_ASM_NOERR)
		{
			ucase(arg);
			cout << arg << "=";
			printf("%04X", reg[regt]);
			cout << endl << ':';
			scanf("%x", &reg[regt]);
		}
		else 
		{
			if (arg == "pc")
			{
				cout << "PC" << "=";
				printf("%04X", pc);
				cout << endl << ':';
				scanf("%x", &pc);
			}
			else if (arg == "sp")
			{
				cout << "SP" << "=";
				printf("%04X", sp);
				cout << endl << ':';
				scanf("%x", &sp);
			}
			else if (arg == "ex")
			{
				cout << "EX" << "=";
				printf("%04X", ex);
				cout << endl << ':';
				scanf("%x", &ex);
			}
			else if (arg == "ia")
			{
				cout << "IA" << "=";
				printf("%04X", ia);
				cout << endl << ':';
				scanf("%x", &ia);
			}
			else
				return;
			//cout << endl;
		}
		cin.get();
	}
}

void trace(string arg = "")
{
	bool haveArg = false;
	UINT endAt = pc;
	if (arg != "")
	{
		endAt = toNum(arg, 3);
		haveArg = true;
	}
	string ins;
	do
	{
		doCode();
		regist();
		unassembler(mem[pc], mem[pc + 1], mem[pc + 2], ins);
		cout << "Next:" << ins << endl;
	}
	while (haveArg && pc < endAt);
}

void proceed()
{
	doCodeThreadStarter();
	clockStarter();
	getchar();
	doCodeB = false;
}

struct pendItem
{
	std::string str;
	USHORT pos;
	int len, lineN;
	pendItem(){};
	pendItem(std::string _str, USHORT _pos, int _len, int _lineN)
	{
		str = _str;
		pos = _pos;
		len = _len;
		lineN = _lineN;
	}
};
typedef list<pendItem> pendList;

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
typedef list<label> labelList;

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
typedef list<defItem> defList;

defList defLst;

bool preprocesser(string path)
{
	ifstream mfin(path);
	ofstream fout("pp_" + path);

	return false;
}

int generate(string path, USHORT wAdd = 0)
{
	if (preprocesser(path))
		return -1;
	ifstream file("pp_" + path);
	if (!file.is_open())
	{
		cout << "  ^ Error" << endl;
		return -1;
	}

	int result = 0;

	char *line = new char[65536];
	int lineCount = 0;
	string insline;
	int markPos = string::npos;
	USHORT sysLblCount = 0;
	string sysLabel;

	labelList lblLst;
	pendList pendLst;
	string lbl;
	int pendCount = 0;
	labelList::iterator lblBeg, lblItr, lblEnd;
	list<labelList::iterator> lblUsedLst;
	list<labelList::iterator>::const_iterator usedItr, usedEnd;
	pendItem pendItm;

	string ppCmd, ppArg;
	defList::iterator defItr, defEnd;

	USHORT add = wAdd, addShift = wAdd;
	int len = 0, pendLen = 3, i;
	int *m = new int[0x200000];
	bool *joined = new bool[0x200000];

	memset(m, -1, sizeof(int) * 0x200000);
	memset(joined, false, sizeof(bool)* 0x200000);
	while (!file.eof())
	{
		lineCount++;
		file.getline(line, 65536, '\n');
		insline = line;
		trim(insline);
		if (insline.length() < 1)
			continue;
		if (insline.front() == '#')
		{
			insline.erase(0, 1);
			markPos = insline.find(' ');
			if (markPos == string::npos)
			{
				ppCmd = insline;
				ppArg = "";
			}
			else
			{
				ppCmd = insline.substr(0, markPos);
				ppArg = insline.substr(markPos + 1);
			}
			lcase(ppCmd);
			if (ppCmd == "define")
			{
				markPos = ppArg.find(' ');
				if (markPos == string::npos)
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
			else if (ppCmd == "include")
			{
				
			}
			else
			{
				cout << path << ':' << lineCount << ":Undefined preprocess command" << endl;
				result = -1;
				goto _g_end;
			}
			continue;
		}
		markPos = insline.find(';');
		if (markPos != string::npos)
		{
			if (markPos == 0)
				insline = "";
			else
			{
				insline.erase(markPos - 1);
				trim(insline);
			}
		}
		markPos = insline.find('$');
		if (markPos != string::npos)
		{
			sysLabel = "__asm_sys_label_" + toHEX(sysLblCount);
			lblLst.push_back(label(sysLabel, add, add));
			insline = insline.substr(0, markPos) + sysLabel + insline.substr(markPos + 1);
		}
		markPos = insline.find(':');
		if (markPos != string::npos)
		{
			if (markPos == 0)
			{
				markPos = insline.find(' ');
				if (markPos == string::npos)
				{
					lbl = insline.substr(1);
					insline = "";
				}
				else
				{
					lbl = insline.substr(1, markPos - 1);
					insline.erase(0, markPos + 1);
				}
				lblLst.push_back(label(lbl, add, add));
			}
			else
			{
				lbl = insline.substr(0, markPos);
				insline.erase(0, markPos + 1);
				lblLst.push_back(label(lbl, add, add));
			}
		}
		if (insline.length() < 1)
			continue;
		defEnd = defLst.end();
		for (defItr = defLst.begin(); defItr != defEnd; defItr++)
		{
			markPos = insline.find(defItr->name);
			while (markPos != string::npos)
			{
				insline = insline.substr(0, markPos) + defItr->val + insline.substr(markPos + defItr->name.length());
				markPos = insline.find(defItr->name);
			}
		}
		len = assembler(insline, m_ret, INS_RET_LEN);
		switch (len)
		{
			case _ERR_ASM_NOT_SUPPORTED:
				cout << path << ':' << lineCount << ":Instruction " << line << " is not supported" << endl;
				result = -1;
				goto _g_end;
			case _ERR_ASM_ILLEGAL:
			case _ERR_ASM_ILLEGAL_OP:
				cout << path << ':' << lineCount << ":Illegal instruction " << line << endl;
				result = -1;
				goto _g_end;
			case _ERR_ASM_ILLEGAL_ARG:
				pendLst.push_back(pendItem(insline, add, 0x20, lineCount));
				joined[add - addShift] = true;
				//为含有未能识别的标签的代码留出空间
				add += 0x20;
				pendCount++;
				break;
			default:
				for (i = 0; i < len; i++, add++)
					m[add - addShift] = m_ret[i];
		}
	}
	lblLst.sort();	//按标签长度从大到小排序以解决长标签与短标签内容部分重复时长标签被部分替换的问题
	lblBeg = lblLst.begin();
	lblEnd = lblLst.end();
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
			cout << "Assembler Error" << endl;
			cout << "Please send your program to the developer" << endl;
			goto _g_end;
		}
		joined[add - addShift] = false;
		pendLen = pendItm.len;
		for (i = 0; i < pendLen; i++)
			m[add + i - addShift] = -1;
		lblUsedLst.clear();
		for (lblItr = lblBeg; lblItr != lblEnd; lblItr++)
		{
			markPos = insline.find(lblItr->str);
			if (markPos != string::npos)
			{
				lblUsedLst.push_back(lblItr);
				while (markPos != string::npos)
				{
					insline = insline.substr(0, markPos) + "0x" + toHEX(lblItr->pos) + insline.substr(markPos + lblItr->str.length());
					markPos = insline.find(lblItr->str);
				}
			}
		}
		len = assembler(insline, m_ret, INS_RET_LEN);
		pendItm.len = len;
		usedEnd = lblUsedLst.cend();
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
				cout << path << ':' << pendItm.lineN << ":Instruction " << pendItm.str << " is not supported" << endl;
				result = -1;
				goto _g_end;
			case _ERR_ASM_ILLEGAL:
			case _ERR_ASM_ILLEGAL_OP:
			case _ERR_ASM_ILLEGAL_ARG:
				cout << path << ':' << pendItm.lineN << ":Illegal instruction " << pendItm.str << endl;
				result = -1;
				goto _g_end;
			default:
				for (i = 0; i < len; i++, add++)
					m[add - addShift] = m_ret[i];
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
		mem[add++] = m[i];
	}
	result = retLen;
_g_end:file.close();
	delete[] line;
	delete[] m;
	return result;
}

fInit initF[65535];

void init()
{
	memset(mem, 0, sizeof(mem));
	for (int i = 0; i < 8; i++)
		reg[i] = 0;
	pc = 0;
	sp = 0;
	ex = 0;
	ia = 0;
	for (UINT i = 0; i < hwn; i++)
		if (initF[i] != NULL)
			(*initF[i])();
}

void printUsage()
{
	cout << "assemble\tA [address]" << endl;		//
	cout << "dump\t\tD [address]" << endl;			//
	cout << "enter\t\tE address [list]" << endl;	//
	cout << "generate\tG [address]" << endl;		//
	cout << "initialize\tI" << endl;				//
	cout << "load\t\tL address" << endl;			//
	cout << "name\t\tN path" << endl;				//
	cout << "proceed\t\tP" << endl;					//
	cout << "quit\t\tQ" << endl;					//
	cout << "register\tR [register]" << endl;		//
	cout << "trace\t\tT [end]" << endl;				//
	cout << "unassemble\tU [address]" << endl;		//
	cout << "write\t\tW start end" << endl;			//
	return;
}

int mainLoop()
{
	char _cmd[100];
	string cmd;
	string filePath;
	fstream file;
	char fileDatO = 0;
	USHORT fileDat = 0;
	int filePtr = 0, fileEndPtr = 0;
	int i, argn;
	cout << '-';
	while (true)
	{
		cin.getline(_cmd, 100, '\n');
		cmd = _cmd;
		trim(cmd);
		if (cmd.length() < 1)
		{
			cout << "  ^ Error" << endl << "-";
			continue;
		}
		if (cmd[0] != 'q')
		{
			argn = 0;
			m_arg[0] = "";
			int pos = cmd.find(' ');
			if (pos != string::npos)
			{
				argn++;
				m_arg[0] = cmd.substr(pos + 1);
				ltrim(m_arg[0]);
				cmd.erase(pos);
				for (i = 1; i < INS_RET_LEN; i++)
				{
					m_arg[i] = "";
					pos = m_arg[i - 1].find(' ');
					if (pos == string::npos)
						break;
					argn++;
					m_arg[i] = m_arg[i - 1].substr(pos + 1);
					ltrim(m_arg[i]);
					m_arg[i - 1].erase(pos);
				}
			}
		}
		if (cmd.length() != 1)
		{
			cout << "  ^ Error" << endl << "-";
			continue;
		}
		switch (cmd[0])
		{
			case 'q':
				return 0;
			case '?':
				printUsage();
				break;
			case 'd':
				dump(m_arg[0]);
				break;
			case 'e':
				enter(argn, m_arg);
				break;
			case 'r':
				regist(m_arg[0]);
				break;
			case 'a':
				assm(m_arg[0]);
				break;
			case 'u':
				unasm(m_arg[0]);
				break;
			case 't':
				trace(m_arg[0]);
				break;
			case 'p':
				proceed();
				break;
			case 'n':
				filePath = m_arg[0];
				break;
			case 'l':
				if (file.is_open())
					file.close();
				file.open(filePath, ios::in | ios::binary);
				filePtr = (USHORT)(toNum(m_arg[0]));
				if (!file.is_open())
				{
					cout << "  ^ Error:File not exists" << endl;
					break;
				}
				while (!file.eof())
				{
					file.get(fileDatO);
					fileDat = (BYTE)(fileDatO);
					file.get(fileDatO);
					fileDat = (fileDat << 8) + (BYTE)(fileDatO);
					mem[filePtr++] = fileDat;
				}
				file.close();
				break;
			case 'w':
				if (file.is_open())
					file.close();
				file.open(filePath, ios::out | ios::binary | ios::trunc);
				filePtr = (USHORT)(toNum(m_arg[0]));
				fileEndPtr = (USHORT)(toNum(m_arg[1]));
				for (; filePtr <= fileEndPtr; filePtr++)
				{
					file.put((mem[filePtr] & 0xFF00) >> 8);
					file.put(mem[filePtr] & 0xFF);
				}
				file.close();
				break;
			case 'g':
				if (m_arg[0] == "")
					generate(filePath, 0);
				else
					generate(filePath, toNum(m_arg[0]));
				break;
			case 'i':
				init();
				break;
			default:
				cout << "  ^ Error" << endl;
				break;
		}
		cout << '-';
	}
	return 0;
}

ifstream fin;
ofstream logout("log.log");

#define FUNC_COUNT 3
char *funcName[FUNC_COUNT] = {
	"getHWCount",
	"getInfo",
	"setHandle"
};
#ifdef _P_WIN
FARPROC funcAdd[FUNC_COUNT];
#endif
#ifdef _P_LIN
typedef void (*voidFunc)();
voidFunc funcAdd[FUNC_COUNT];
#endif

int main(int argc, char* argv[], char* envp[])
{
	fin.open("plugins.txt");
	if (!fin.is_open())
	{
		ofstream crtfile("plugins.txt");
		crtfile.close();
		fin.open("plugins.txt");
	}
	string filename("");
	char *fname = NULL;
	int len = 0, i;
	while (!fin.eof())
	{
		fin >> filename;
		len = filename.length();
		if (len < 1)
			continue;
		fname = new char[len + 1];
		for (i = 0; i < len; i++)
			fname[i] = filename[i];
		fname[len] = '\0';
#ifdef _P_WIN
		HMODULE plugin = LoadLibrary(filename.c_str());
		if (plugin != NULL)
		{
			logout << "Loaded plugin " << filename << endl;
			FARPROC hd = GetProcAddress(plugin, "init");
			if (hd != NULL)
			{
				int initRes = ((fInit)(*hd))();
				if (initRes != 0)
				{
					logout << "Failed to initialize plugin " << filename << " with error code 0x" << toHEX(initRes) << endl;
					FreeLibrary(plugin);
					delete fname;
					continue;
				}
			}
			bool loadSuccess = true;
			for (i = 0; i < FUNC_COUNT; i++)
			{
				hd = GetProcAddress(plugin, funcName[i]);
				if (hd == NULL)
				{
					logout << "Failed to find function entry point " << funcName[i] << " in plugin " << filename << endl;
					FreeLibrary(plugin);
					loadSuccess = false;
					break;
				}
				funcAdd[i] = hd;
			}
			if (loadSuccess)
			{
				int hwCount = ((fGetHWCount)(*funcAdd[0]))();
				hd = funcAdd[1];
				for (i = 0; i < hwCount; i++)
					hwt[hwn++] = ((fGetInfo)(*hd))(i);
				((fSetHandle)(*funcAdd[2]))(&setMem, &getMem, &setReg, &getReg, &additr);
			}
		}
		else
			logout << "Failed to load plugin " << filename << endl;
#endif
#ifdef _P_LIN
		void *plugin = NULL;
		plugin = dlopen(fname, RTLD_LAZY);
		if (plugin == NULL)
			continue;
		char* pszErr = dlerror();
		if (pszErr != NULL)
			continue;
		logout << "Loaded plugin " << filename << endl;
		fInit initF = dlsym(plugin, "init");
		pszErr = dlerror();
		if (pszErr != NULL)
		{
			int initRes = (*initF)();
			if (initRes != 0)
			{
				logout << "Failed to initialize plugin " << filename << " with error code 0x" << toHEX(initRes) << endl;
				dlclose(plugin);
				delete fname;
				continue;
			}
		}
		bool loadSuccess = true;
		voidFunc hd;
		for (i = 0; i < FUNC_COUNT; i++)
		{
			hd = dlsym(plugin, funcName[i]);
			pszErr = dlerror();
			if (pszErr != NULL)
			{
				logout << "Failed to find function entry point " << funcName[i] << " in plugin " << filename << endl;
				dlclose(plugin);
				loadSuccess = false;
				break;
			}
			funcAdd[i] = hd;
		}
		if (loadSuccess)
		{
			int hwCount = ((fGetHWCount)(*funcAdd[0]))();
			hd = funcAdd[1];
			for (i = 0; i < hwCount; i++)
				hwt[hwn++] = ((fGetInfo)(*hd))(i);
			((fSetHandle)(*funcAdd[2]))(&setMem, &getMem, &setReg, &getReg, &additr);
		}
#endif
		delete fname;
	}
	return mainLoop();
}
