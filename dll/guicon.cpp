/*
* guicon.cpp
*
* This creates a Win32 console window and redirects std* to/from it.
* Code is Copyright 1997 by Andrew Tucker.
*/

#include <Windows.h>
#include "guicon.h"
#include "stdafx.h"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

/*#ifndef _USE_OLD_IOSTREAMS
using namespace std;
#endif*/

// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

#ifdef _DEBUG

extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow (void);

void RedirectIOToConsole()
{
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	
	// allocate a console for this app
	
	AllocConsole();
	
	// set the screen buffer to be big enough to let us scroll text
	
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
		&coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
		coninfo.dwSize);
	
	// redirect unbuffered STDOUT to the console
	
	freopen_s(&fp, "conout$", "w+t", stdout);
	setvbuf( stdout, NULL, _IONBF, 0 );
	
	// redirect unbuffered STDIN to the console
	
	freopen_s(&fp, "conin$", "r+t", stdin);
	setvbuf( stdin, NULL, _IONBF, 0 );
	
	// redirect unbuffered STDERR to the console

	freopen_s(&fp, "conout$", "w+t", stdout);
	setvbuf( stderr, NULL, _IONBF, 0 );
	
	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	
//	ios::sync_with_stdio();
	
}

void PutConsoleToFront()
{
	BringWindowToTop(GetConsoleWindow());
}

void CloseConsole()
{
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
	FreeConsole();
}

#endif

//End of File
