// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "defines.h"
#include "main.h"
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
		info[0].a = 0xF615;
		info[0].b = 0x7349;
		info[0].c = 0x1802;
		info[0].x = 0x8B36;
		info[0].y = 0x1C6C;
		info[0].hwi = &intrptMonitor;
		info[1].a = 0x7406;
		info[1].b = 0x30CF;
		info[1].c = 0x0001;
		info[1].x = 0x2EBC;
		info[1].y = 0x6F78;
		info[1].hwi = &intrptKeyb;
		for (int i = 0; i < 128; i++)
			for (int j = 0; j < 96; j++)
			{
				mem[i][j].r = 0.0f;
				mem[i][j].g = 0.0f;
				mem[i][j].b = 0.0f;
			}
		timeSetEvent(20, 1, &ClockMain, 0, TIME_PERIODIC);
		mainThread = CreateThread(NULL, 0, &MainThread, NULL, 0, &threadID);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		PostMessage(hWnd, WM_QUIT, 0, 0);
		break;
	}
	return TRUE;
}

