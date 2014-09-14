#include "stdafx.h"
#include "core/main.h"
using namespace std;

/*
#define const_iterator iterator
#define cbegin begin
#define cend end
//*/

#define INS_RET_LEN 65536

USHORT m_ret[INS_RET_LEN];
string m_arg[INS_RET_LEN];

void doCodeThreadS()
{
	doCodeThread();
	if (doCodeB)
		cout << "Process stopped.Press any key to continue." << endl;
}

#ifdef _P_WIN
DWORD threadID = 0;
DWORD WINAPI doCodeThreadBegin(LPVOID lpParam)
{
	doCodeThreadS();
	return 0;
}
#endif

#ifdef _P_LIN
pthread_attr_t attr;
pthread_t tid;
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
	pthread_create(&tid, &attr, &doCodeThreadS, NULL);
#endif
#ifdef _P_NA
#ifdef __CPP11_thread
	thread em(doCodeThreadS);
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
		add = static_cast<USHORT>(toNum(arg, 3));
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
		add = static_cast<USHORT>(toNum(arg));
	USHORT end = add + 0x40;
	string ins;
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
		add = static_cast<UINT>(toNum(arg));
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
	UINT add = static_cast<UINT>(toNum(args[0]));
	for (int i = 1; i < argc && add < 0x10000; i++)
		mem[add++] = static_cast<USHORT>(toNum(args[i]));
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
		endAt = static_cast<UINT>(toNum(arg, 3));
		haveArg = true;
	}
	string ins;
	do
	{
		if (doCode() < 1)
		{
			cout << "Wrong Instruction or Internal Error occurred" << endl;
			break;
		}
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

void generate(string path, USHORT wAdd = 0, bool printLabel = false)
{
	try
	{
		asmFile(path, mem, 65536, wAdd, printLabel);
	}
	catch (std::list<std::string>* err)
	{
		std::list<std::string>::const_iterator p, pEnd = err->cend();
		for (p = err->cbegin(); p != pEnd; p++)
			std::cout << *p << std::endl;
	}
	catch (std::string err)
	{
		cout << err << endl;
	}
}

void breakpoint(string arg = "")
{
	int add = static_cast<int>(toNum(arg));
	if (add < 0)
	{
		cout << "  ^ Error" << endl;
		return;
	}
	breakPoint[add] = true;
}

fInit initF[65535];

void init()
{
	memset(mem, 0, sizeof(mem));
	memset(breakPoint, false, sizeof(breakPoint));
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
	cout << "assemble\tA [address]" << endl;							//
	cout << "breakpoint\tB address" << endl;							//
	cout << "dump\t\tD [address]" << endl;								//
	cout << "enter\t\tE address [list]" << endl;						//
	cout << "generate\tG [-a address] [--lo true/false]" << endl;		//
	cout << "initialize\tI" << endl;									//
	cout << "load\t\tL address" << endl;								//
	cout << "name\t\tN path" << endl;									//
	cout << "proceed\t\tP" << endl;										//
	cout << "quit\t\tQ" << endl;										//
	cout << "register\tR [register]" << endl;							//
	cout << "trace\t\tT [end]" << endl;									//
	cout << "unassemble\tU [address]" << endl;							//
	cout << "write\t\tW start end" << endl;								//
	return;
}

int mainLoop(char *argv = NULL)
{
	string cmd;
	if (argv != NULL)
		cmd = argv;
	else
	{
		while (true)
		{
			cout << '-';
			getline(cin, cmd, '\n');
			trim(cmd);
			if (cmd.length() < 1)
				cout << "  ^ Error" << endl;
			else
				break;
		}
	}
	string filePath;
	int i, argn;
	while (true)
	{
		if (cmd.front() != 'q')
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
			cout << "  ^ Error" << endl;
		}
		else
		{
			switch (cmd.front())
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
				{
					fstream file;
					char fileDatO = 0;
					USHORT fileDat = 0;
					int filePtr = 0;
					file.open(filePath, ios::in | ios::binary);
					filePtr = static_cast<USHORT>(toNum(m_arg[0]));
					if (!file.is_open())
					{
						cout << "  ^ Error:File not exists" << endl;
						break;
					}
					while (!file.eof())
					{
						file.get(fileDatO);
						fileDat = static_cast<BYTE>(fileDatO);
						file.get(fileDatO);
						fileDat = (fileDat << 8) + static_cast<BYTE>(fileDatO);
						mem[filePtr++] = fileDat;
					}
					file.close();
					break;
				}
				case 'w':
				{
					fstream file;
					char fileDatO = 0;
					USHORT fileDat = 0;
					int filePtr = 0, fileEndPtr = 0;
					file.open(filePath, ios::out | ios::binary | ios::trunc);
					if (!file.is_open())
					{
						cout << "  ^ Error:Can't Open File" << endl;
						break;
					}
					filePtr = static_cast<USHORT>(toNum(m_arg[0]));
					fileEndPtr = static_cast<USHORT>(toNum(m_arg[1]));
					for (; filePtr <= fileEndPtr; filePtr++)
					{
						file.put((mem[filePtr] & 0xFF00) >> 8);
						file.put(mem[filePtr] & 0xFF);
					}
					file.close();
					break;
				}
				case 'g':
				{
					USHORT add = 0;
					bool labelOut = false;
					string *arg;
					string path = filePath;
					for (i = 0; i < argn; i++)
					{
						arg = &m_arg[i];
						if (arg->front() == '-')
						{
							arg->erase(0, 1);
							if (arg->length() < 1)
							{
								cout << "  ^ Error" << endl;
								break;
							}
							if ((*arg) == "a")
							{
								i++;
								add = static_cast<USHORT>(toNum(m_arg[i]));
							}
							else if ((*arg) == "-lo")
							{
								i++;
								if (m_arg[i] == "true")
									labelOut = true;
								else if (m_arg[i] == "false")
									labelOut = false;
								else
								{
									cout << "  ^ Error" << endl;
									break;
								}
							}
						}
						else
						{
							path = m_arg[i];
						}
					}
					generate(path, add, labelOut);
					break;
				}
				case 'i':
					init();
					break;
				case 'b':
					breakpoint(m_arg[0]);
					break;
				default:
					cout << "  ^ Error" << endl;
					break;
			}
		}
		while (true)
		{
			cout << '-';
			getline(cin, cmd, '\n');
			trim(cmd);
			if (cmd.length() < 1)
				cout << "  ^ Error" << endl;
			else
				break;
		}
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

int main(int argc, char* argv[])
{
	if (!logout.is_open())
	{
		cout << "Can't open log file" << endl;
		return 0;
	}
	int ret = 0;
	try
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
#ifdef _P_WIN
			HMODULE plugin = LoadLibrary(filename.c_str());
			if (plugin != NULL)
			{
				logout << "Loaded plugin " << filename << endl;
				FARPROC hd = GetProcAddress(plugin, "init");
				if (hd != NULL)
				{
					int initRes = (reinterpret_cast<fInit>(hd))();
					if (initRes != 0)
					{
						logout << "Failed to initialize plugin " << filename << " with error code 0x" << toHEX(initRes) << endl;
						FreeLibrary(plugin);
						continue;
					}
					initF[hwn] = reinterpret_cast<fInit>(hd);
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
					int hwCount = (reinterpret_cast<fGetHWCount>(funcAdd[0]))();
					hd = funcAdd[1];
					for (i = 0; i < hwCount; i++)
						hwt[hwn++] = (reinterpret_cast<fGetInfo>(hd))(i);
					(reinterpret_cast<fSetHandle>(*funcAdd[2]))(
						reinterpret_cast<void *>(&setMem),
						reinterpret_cast<void *>(&getMem),
						reinterpret_cast<void *>(&setReg),
						reinterpret_cast<void *>(&getReg),
						reinterpret_cast<void *>(&additr));
				}
			}
			else
				logout << "Failed to load plugin " << filename << ":LoadLibrary Failed" << endl;
#endif
#ifdef _P_LIN
			void *plugin = NULL;
			plugin = dlopen(filename.c_str(), RTLD_LAZY);
			if (plugin == NULL)
				continue;
			char* pszErr = dlerror();
			if (pszErr != NULL)
				continue;
			logout << "Loaded plugin " << filename << endl;
			fInit initF = reinterpret_cast<fInit>(dlsym(plugin, "init"));
			pszErr = dlerror();
			if (pszErr != NULL)
			{
				int initRes = (*initF)();
				if (initRes != 0)
				{
					logout << "Failed to initialize plugin " << filename << " with error code 0x" << toHEX(initRes) << endl;
					dlclose(plugin);
					continue;
				}
			}
			bool loadSuccess = true;
			voidFunc hd;
			for (i = 0; i < FUNC_COUNT; i++)
			{
				hd = reinterpret_cast<voidFunc>(dlsym(plugin, funcName[i]));
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
				int hwCount = (reinterpret_cast<fGetHWCount>(funcAdd[0]))();
				hd = funcAdd[1];
				for (i = 0; i < hwCount; i++)
					hwt[hwn++] = (reinterpret_cast<fGetInfo>(hd))(i);
				(reinterpret_cast<fSetHandle>(*funcAdd[2]))(
					reinterpret_cast<void *>(&setMem),
					reinterpret_cast<void *>(&getMem),
					reinterpret_cast<void *>(&setReg),
					reinterpret_cast<void *>(&getReg),
					reinterpret_cast<void *>(&additr));
			}
#endif
		}
		memset(mem, 0, sizeof(mem));
		memset(breakPoint, false, sizeof(breakPoint));
		for (int i = 0; i < 8; i++)
			reg[i] = 0;
		pc = 0;
		sp = 0;
		ex = 0;
		ia = 0;
		if (argc == 1)
			ret = mainLoop();
		else if (argc == 2)
			ret = mainLoop(argv[1]);
	}
	catch (const char *ex)
	{
		logout << "Error:" << ex << endl;
	}
	catch (const string &ex)
	{
		logout << "Error:" << ex << endl;
	}
	catch (const exception &ex)
	{
		logout << "Error:" << ex.what() << endl;
	}
	catch (...)
	{
		logout << "An unknown error occured" << endl;
	}
	return ret;
}
