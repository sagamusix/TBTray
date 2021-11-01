/*
 * TBTray companion DLL by Johannes Schultz
 *
 * Based on Traktouch by Joachim Fenkes:
 *
 * Copyright (c) 2019 by Joachim Fenkes <github@dojoe.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES			1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT	1
 // Windows Header Files:

#include <windows.h>
#include <tchar.h>

#include "guicon.h"
#include <stdlib.h>

#ifdef _DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif

/* Our GetMessage hook */
HHOOK hMessageHook;

static HWND mainHwnd = nullptr;

LRESULT CALLBACK MessageHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSG &msg = *(MSG *)lParam;

	if(mainHwnd == nullptr && (GetWindowLong(msg.hwnd, GWL_STYLE) & WS_SIZEBOX))
	{
		/*
		 * The main UI thread handles several windows and we need to subclass the right one,
		 * so check the window title until we see the proper Thunderbird window pass by.
		 */
		static TCHAR winTitle[256];
		GetWindowText(msg.hwnd, winTitle, _countof(winTitle));
		if(_tcsstr(winTitle, L"- Mozilla Thunderbird"))
			mainHwnd = msg.hwnd;
	}

	if(msg.hwnd == mainHwnd &&
		(
		(msg.message == WM_NCLBUTTONDOWN && msg.wParam == HTCLOSE)
		||
		(msg.message == WM_SYSCOMMAND && msg.wParam == SC_CLOSE)
		))
	{
		PostMessage(msg.hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		// Ignore this message
		msg.message = WM_NULL;
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

HMODULE hDLL;

/*
 * This is a temporary WindowProc hook that serves as the initial entry point to the DLL.
 * The loader installs this hook into the Thunderbird main UI thread, then sends a message to
 * make sure this function runs.
 *
 * This installs the actual hook from the context of the running Thunderbird, and also pins the
 * DLL inside Thunderbird to make sure it's not being unloaded. It runs inside the main UI thread
 * so we're safe against concurrency issues at this point.
 */
extern "C" __declspec(dllexport) LRESULT CALLBACK EntryHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	static bool firstTime = true;
	if (firstTime)
	{
#ifdef _DEBUG
		RedirectIOToConsole();
#else
		static TCHAR dllName[2048];
		// Prevent the DLL from unloading by incrementing its refcount.
		//In debug mode the loader's hook will keep the DLL loaded and we want
		//to be able to unload, so we don't do this in debug mode.
		GetModuleFileName(hDLL, dllName, _countof(dllName));
		LoadLibrary(dllName);
#endif
		hMessageHook = SetWindowsHookEx(WH_GETMESSAGE, MessageHook, NULL, GetCurrentThreadId());
		firstTime = false;
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hDLL = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
