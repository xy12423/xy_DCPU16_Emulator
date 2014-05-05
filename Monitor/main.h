#pragma once

#ifndef _H_MAIN
#define _H_MAIN

#define unitLen 3

#include "defines.h"
#include "stdafx.h"

char szTitle[] = "Monitor";
char szWindowClass[] = "Monitor";

#define WINDOW_TOP 200
#define WINDOW_LEFT 400
#define WINDOW_WIDTH (128 * unitLen)
#define WINDOW_HEIGHT (96 * unitLen)

HDC			hDC = NULL;
HGLRC		hRC = NULL;
HWND		hWnd = NULL;
HINSTANCE	hInstance;

bool	keys[256];
BOOL	active = TRUE;
volatile BOOL done = FALSE;

//Keyboard

std::list<BYTE> keyList;
USHORT keyItr = 0;

//Monitor

struct color
{
	GLfloat r, g, b;
};

color mem[128][96];

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL DrawGLScene(GLvoid)
{
	int i, j;
	glLoadIdentity();
	glOrtho(0, 128, -96, 0, -10.0f, 10.0f);
	for (i = 0; i < 128; i++)
		for (j = 0; j < 96; j++)
		{
			glColor3f(mem[i][j].r, mem[i][j].g, mem[i][j].b);
			glRectf((GLfloat)(i), (GLfloat)(-j), (GLfloat)(i + 1), (GLfloat)(-j - 1));
		}
	glFlush();
	SwapBuffers(hDC);
	return TRUE;
}

BOOL InitGL(GLvoid)
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	return TRUE;
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
{
	glOrtho(0, 128, -96, 0, -10.0f, 10.0f);
}

GLvoid KillGLWindow(GLvoid)
{
	if (hRC)
	{
		if (!wglMakeCurrent(NULL, NULL))
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		if (!wglDeleteContext(hRC))
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hRC = NULL;
	}

	if (hDC && !ReleaseDC(hWnd, hDC))
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;
	}

	if (hWnd && !DestroyWindow(hWnd))
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;
	}

	if (!UnregisterClass(szWindowClass, hInstance))
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;
	}
}

BOOL CreateGLWindow(char* title, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;
	WNDCLASS	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	RECT		WindowRect;
	WindowRect.left = (long)0;
	WindowRect.right = (long)WINDOW_WIDTH;
	WindowRect.top = (long)0;
	WindowRect.bottom = (long)WINDOW_HEIGHT;

	hInstance = GetModuleHandle(NULL);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szWindowClass;

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_DLGFRAME;

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	if (!(hWnd = CreateWindowEx(dwExStyle,
		szWindowClass,
		title,
		dwStyle |
		WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN,
		WINDOW_LEFT, WINDOW_TOP,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL)))
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(WINDOW_WIDTH, WINDOW_HEIGHT);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

bool bigLetter = false;
USHORT numShift[10] = { 
	0x29,
	0x21,
	0x40,
	0x23,
	0x24,
	0x25,
	0x5E,
	0x26,
	0x2A,
	0x28 
};

USHORT getDCKeyCode(UINT nativeKC)
{
	if (0x41 <= nativeKC && nativeKC <= 0x5A)
	{
		if (bigLetter)
			return nativeKC;
		else
			return nativeKC + 0x20;
	}
	else if (0x30 <= nativeKC && nativeKC <= 0x39)
	{
		if (keys[0x90])
			return numShift[nativeKC - 0x30];
		else
			return nativeKC;
	}
	else if (0x60 <= nativeKC && nativeKC <= 0x69)
	{
		return nativeKC - 0x30;
	}
	switch (nativeKC)
	{
		case 0x8:
			return 0x10;
		case 0xD:
			return 0x11;
		case 0x2D:
			return 0x12;
		case 0x2E:
			return 0x13;
		case 0x26:
			return 0x80;
		case 0x28:
			return 0x81;
		case 0x25:
			return 0x82;
		case 0x27:
			return 0x83;
		case 0x10:
			return 0x90;
		case 0x11:
			return 0x91;
		case 0x20:
			return 0x20;

		case 0xC0:
			if (keys[0x90])
				return 0x7E;
			else
				return 0x60;
		case 0xBD:
			if (keys[0x90])
				return 0x5F;
			else
				return 0x2D;
		case 0xBB:
			if (keys[0x90])
				return 0x2B;
			else
				return 0x3D;

		case 0xDB:
			if (keys[0x90])
				return 0x7B;
			else
				return 0x5B;
		case 0xDD:
			if (keys[0x90])
				return 0x7D;
			else
				return 0x5D;
		case 0xDC:
			if (keys[0x90])
				return 0x7C;
			else
				return 0x5C;

		case 0xBA:
			if (keys[0x90])
				return 0x3A;
			else
				return 0x3B;
		case 0xDE:
			if (keys[0x90])
				return 0x22;
			else
				return 0x27;

		case 0xBC:
			if (keys[0x90])
				return 0x3C;
			else
				return 0x2C;
		case 0xBE:
			if (keys[0x90])
				return 0x3E;
			else
				return 0x2E;
		case 0xBF:
			if (keys[0x90])
				return 0x3F;
			else
				return 0x2F;

		case 0x6A:
			return 0x2A;
		case 0x6B:
			return 0x2B;
		case 0x6D:
			return 0x2D;
		case 0x6E:
			return 0x2E;
		case 0x6F:
			return 0x2F;
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool oldBigLetter;
	switch (uMsg)
	{
		case WM_ACTIVATE:
			if (!HIWORD(wParam))
				active = TRUE;
			else
				active = FALSE;
			return 0;
		case WM_SYSCOMMAND:
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					return 0;
			}
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			oldBigLetter = bigLetter;
			if (wParam == 0x14)
				bigLetter = !bigLetter;
			if (wParam == 0x10)
				bigLetter = !bigLetter;
			if (oldBigLetter != bigLetter)
			{
				if (oldBigLetter)
				{
					for (int i = 0x41; i <= 0x5A; i++)
					{
						if (keys[i])
						{
							keys[i] = false;
							keys[i + 0x20] = true;
						}
					}
				}
				else
				{
					for (int i = 0x61; i <= 0x7A; i++)
					{
						if (keys[i])
						{
							keys[i] = false;
							keys[i - 0x20] = true;
						}
					}
				}
			}
			wParam = getDCKeyCode(wParam);
			if (wParam == 0)
				return 0;
			keys[wParam] = true;
			keyList.push_back((BYTE)(wParam));
			if (keyItr != 0)
				(*additr)(keyItr);
			return 0;
		case WM_KEYUP:
			oldBigLetter = bigLetter;
			if (wParam == 0x10)
				bigLetter = !bigLetter;
			if (oldBigLetter != bigLetter)
			{
				if (oldBigLetter)
				{
					for (int i = 0x41; i <= 0x5A; i++)
					{
						if (keys[i])
						{
							keys[i] = false;
							keys[i + 0x20] = true;
						}
					}
				}
				else
				{
					for (int i = 0x61; i <= 0x7A; i++)
					{
						if (keys[i])
						{
							keys[i] = false;
							keys[i - 0x20] = true;
						}
					}
				}
			}
			wParam = getDCKeyCode(wParam);
			if (wParam == 0)
				return 0;
			keys[wParam] = false;
			if (keyItr != 0)
				(*additr)(keyItr);
			return 0;
		case WM_SIZE:
			ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
			return 0;
		case 0xFFFF:
			DrawGLScene();
			return 0;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

USHORT mf[256];
color mp[16];
USHORT mfDef[256] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x7F41, 0x7F00, 0x427F, 0x4000, 0x7949, 0x4F00, 0x4949, 0x7F00,
	0x0F08, 0x7F00, 0x4F49, 0x7900, 0x7F49, 0x7900, 0x0101, 0x7F00,
	0x7F49, 0x7F00, 0x4F49, 0x7F00, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x7817, 0x7800, 0x7F49, 0x3600, 0x3E41, 0x4100,
	0x7F41, 0x3E00, 0x7F49, 0x4900, 0x7F09, 0x0900, 0x3E41, 0x7100,
	0x7F08, 0x7F00, 0x417F, 0x4100, 0x417F, 0x0100, 0x7F18, 0x6700,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
},
//Font is made by SuperSuperSea(Bobby825)
mpDef[16] = {
	0x0000, 0x0800, 0x0080, 0x0008, 0x0880, 0x0808, 0x0088, 0x0CCC,
	0x0888, 0x0F00, 0x00F0, 0x000F, 0x0FF0, 0x0F0F, 0x00FF, 0x0FFF,
};
volatile USHORT mvStart = 0, mfStart = 0, mpStart = 0;
volatile bool usrMF = false, usrMP = false;
volatile int mOn = 0;
volatile int blinkCount = 0;
volatile bool blinkClock = false;
volatile int monitorBootCount = 0;

void CALLBACK ClockMain(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	int i, x, y;
	if (mOn == 0)
		return;
	else if (mOn == 1)
	{
		monitorBootCount++;
		if (monitorBootCount >= 50)
			mOn = 2;
		return;
	}
	blinkCount++;
	if (blinkCount >= 1000)
	{
		blinkClock = !blinkClock;
		blinkCount = 0;
	}
	if (usrMF)
	{
		for (i = 0; i < 256; i++)
			(*getMem)((USHORT)(i + mfStart), mf + i);
	}
	else
	{
		for (i = 0; i < 256; i++)
			mf[i] = mfDef[i];
	}
	if (usrMP)
	{
		USHORT cl = 0;
		for (i = 0; i < 16; i++)
		{
			(*getMem)((USHORT)(i + mpStart), &cl);
			mp[i].r = (GLfloat)((cl & 0x0F00) >> 8) / 15.0f;
			mp[i].g = (GLfloat)((cl & 0x00F0) >> 4) / 15.0f;
			mp[i].b = (GLfloat)(cl & 0x000F) / 15.0f;
		}
	}
	else
	{
		USHORT cl = 0;
		for (i = 0; i < 16; i++)
		{
			cl = mpDef[i];
			mp[i].r = (GLfloat)((cl & 0x0F00) >> 8) / 15.0f;
			mp[i].g = (GLfloat)((cl & 0x00F0) >> 4) / 15.0f;
			mp[i].b = (GLfloat)(cl & 0x000F) / 15.0f;
		}
	}
	USHORT cel = 0;
	color fore, back;
	BYTE chr;
	bool blink;
	UINT font;
	int l, n;
	i = 0;
	for (y = 0; y < 96; y += 8)
	{
		for (x = 0; x < 128; x += 4)
		{
			(*getMem)((USHORT)(i + mvStart), &cel);
			fore = mp[(cel & 0xF000) >> 12];
			back = mp[(cel & 0x0F00) >> 8];
			if (cel & 0x0080)
				blink = true;
			else
				blink = false;
			if (blink && blinkClock)
			{
				back.r = 1.0f - back.r;
				back.g = 1.0f - back.g;
				back.b = 1.0f - back.b;
			}
			chr = cel & 0x007F;
			font = (mf[chr * 2] << 16) + mf[chr * 2 + 1];
			for (l = x + 3; l >= x; l--)
				for (n = y; n < y + 8; n++)
				{
					if (font & 0x0001)
						mem[l][n] = fore;
					else
						mem[l][n] = back;
					font = font >> 1;
				}
			i++;
		}
	}
	PostMessage(hWnd, 0xFFFF, 0, 0);
	return;
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
	MSG msg;

	if (!CreateGLWindow(szTitle, 16, false))
		return 0;

	DrawGLScene();

	while (!done)
	{
		if (GetMessage(&msg, NULL, 0, 0))	// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)
				done = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	KillGLWindow();
	return (msg.wParam);
}

#endif