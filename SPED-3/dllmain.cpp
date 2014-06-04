// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "defines.h"
#include "3DDisplay.h"
#include "export.h"

HANDLE mainThread = NULL;
DWORD threadID = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		info[0].a = 0xbf3c;
		info[0].b = 0x42ba;
		info[0].c = 0x0003;
		info[0].x = 0x7e91;
		info[0].y = 0x1eb3;
		info[0].hwi = &intrpt;
		timeSetEvent(20, 1, &ClockMain, 0, TIME_PERIODIC);
		mainThread = CreateThread(NULL, 0, &MainThread, NULL, 0, &threadID);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		done = TRUE;
		break;
	}
	return TRUE;
}

