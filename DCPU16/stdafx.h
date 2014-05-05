#pragma once

///*
#include <stdio.h>

#ifdef _MSC_VER
#define scanf scanf_s
#define sscanf sscanf_s
#define fscanf fscanf_s
#define wscanf wscanf_s
#define swscanf swscanf_s
#define fwscanf fwscanf_s
#define printf printf_s
#define sprintf sprintf_s
#define fprintf fprintf_s
#define wprintf wprintf_s
#define swprintf swprintf_s
#define fwprintf fwprintf_s
#endif
//*/

#ifdef _MSC_VER
#if _MSC_VER >= 1700
#ifndef __CPP11_thread
#define __CPP11_thread
#endif
#endif
#endif

#ifdef __GNUC__
#if __GNUC__ > 4 && __GNUC_MINOR__ > 8
#ifndef __CPP11_thread
#define __CPP11_thread
#endif
#endif
#endif

#if __cplusplus >= 201103L || defined(__CPP11_thread)
#include <thread>
#else
#error You need a compiler supports C++11->thread
#endif

#ifdef _WIN32
#define _P_WIN
#else
#ifdef __linux__
#define _P_LIN
#else
#define _P_NA
#endif
#endif

#ifdef _P_WIN
#include <Windows.h>
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")
#endif
#ifdef _P_LIN
#include <dlfcn.h>
#endif
#ifdef _P_NA
#error Your platform is not supported
#endif

#include <iostream>
#include <fstream>
#include <cstring>
#include <list>