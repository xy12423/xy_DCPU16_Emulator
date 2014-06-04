// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <Windows.h>
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")


// TODO:  在此处引用程序需要的其他头文件

#include <gl\gl.h>
#include <gl\glu.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#include <ctime>
#include <list>