#pragma once

#ifndef _H_CK
#define _H_CK

#include "stdafx.h"
#include "defines.h"

int state = 0;
MMRESULT res;
USHORT count = 0, itrpt = 0;

void CALLBACK timer(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	count++;
	if (itrpt != 0)
		(*additr)(itrpt);
}

#endif