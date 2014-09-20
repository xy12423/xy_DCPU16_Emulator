// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "defines.hpp"
#include "floppy.hpp"
#include "export.hpp"

MMRESULT timerHandle = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		info[0].a = 0x24C5;
		info[0].b = 0x4FD5;
		info[0].c = 0x000B;
		info[0].x = 0x7E91;
		info[0].y = 0x1EB3;
		info[0].hwi = &intrpt;
		timerHandle = timeSetEvent(10, 1, &FloppyTimer, NULL, TIME_PERIODIC);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		timeKillEvent(timerHandle);
		break;
	}
	return TRUE;
}

