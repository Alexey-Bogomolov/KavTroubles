// KavTroubles.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Win32Sockets.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Win32Sockets testObject;
	testObject.CheckKAV();

	return 0;
}

