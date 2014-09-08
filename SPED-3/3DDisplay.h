#pragma once

#ifndef _H_MAIN
#define _H_MAIN

#include "defines.h"
#include "stdafx.h"

char szTitle[] = "3D Vector Display";
char szWindowClass[] = "3D Vector Display";

#define WINDOW_TOP 100
#define WINDOW_LEFT 100
#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 300

HDC			hDC = NULL;
HGLRC		hRC = NULL;
HWND		hWnd = NULL;
HINSTANCE	hInstance;

volatile BOOL done = FALSE;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

USHORT dpState, dpError;

struct line
{
	GLdouble x1, y1, z1;
	GLdouble x2, y2, z2;
	GLfloat r, g, b;
};

line mem[128];
volatile USHORT mvStart = 0, mvEnd = 0, mvCount = 0;
volatile USHORT rotAngle = 0;

BOOL DrawGLScene(GLvoid)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -0.3f, -0.3f);
	line t;
	for (int i = 0; i < mvCount; i++)
	{
		t = mem[i];
		glColor3f(t.r, t.g, t.b);
		glBegin(GL_LINES);
		glVertex3d(t.x1, t.y1, 1.0f - t.z1);
		glVertex3d(t.x2, t.y2, 1.0f - t.z2);
		glEnd();
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
	if (height == 0)
		height = 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	DrawGLScene();
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
	WindowRect.left = (LONG)0;
	WindowRect.right = (LONG)WINDOW_WIDTH;
	WindowRect.top = (LONG)0;
	WindowRect.bottom = (LONG)WINDOW_HEIGHT;

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
	dwStyle = WS_OVERLAPPEDWINDOW;

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
		KillGLWindow();
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	static	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		bits,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16, 
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	if (!(hDC = GetDC(hWnd)))
	{
		KillGLWindow();
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
	{
		KillGLWindow();
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))
	{
		KillGLWindow();
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(hRC = wglCreateContext(hDC)))
	{
		KillGLWindow();
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!wglMakeCurrent(hDC, hRC))
	{
		KillGLWindow();
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	ReSizeGLScene(WINDOW_WIDTH, WINDOW_HEIGHT);

	if (!InitGL())
	{
		KillGLWindow();
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
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

GLfloat mcr[] = {
	0.0001f, 0.5f, 0.0f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f
};
GLfloat mcg[] = {
	0.0001f, 0.0f, 0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f
};
GLfloat mcb[] = {
	0.0001f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f
};

void CALLBACK ClockMain(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	int i, j;
	if (mvCount == 0)
		return;
	UINT cell;
	USHORT data;
	BYTE color;
	line t;
	(*getMem)(mvEnd - 2, &data);
	cell = data;
	(*getMem)(mvEnd - 1, &data);
	cell = (cell << 16) + data;
	t.y1 = (GLdouble)(cell >> 24) / (GLdouble)(0xFF);
	t.x1 = (GLdouble)((cell >> 16) & 0xFF) / (GLdouble)(0xFF);
	t.z1 = (GLdouble)(cell & 0xFF) / (GLdouble)(0xFF);
	color = (BYTE)(cell >> 8);
	t.r = mcr[color];
	t.g = mcg[color];
	t.b = mcb[color];
	for (i = mvStart, j = 0; i < mvEnd; i += 2, j++)
	{
		(*getMem)(i, &data);
		cell = data;
		(*getMem)(i + 1, &data);
		cell = (cell << 16) + data;
		t.y2 = (GLdouble)(cell >> 24) / (GLdouble)(0xFF);
		t.x2 = (GLdouble)((cell >> 16) & 0xFF) / (GLdouble)(0xFF);
		t.z2 = (GLdouble)(cell & 0xFF) / (GLdouble)(0xFF);
		mem[j] = t;
		t.x1 = t.x2;
		t.y1 = t.y2;
		t.z1 = t.z2;
		color = (BYTE)(cell >> 8);
		t.r = mcr[color];
		t.g = mcg[color];
		t.b = mcb[color];
	}
	PostMessage(hWnd, 0xFFFF, 0, 0);
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