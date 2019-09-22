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
#include <shellapi.h>

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
static NOTIFYICONDATA nid = { 0 };

enum CommandID
{
	ID_RESTORE = 1000,
	ID_CLOSE
};

static LRESULT CALLBACK TrayIconProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if((uMsg == WM_USER + 1337 && LOWORD(lParam) == NIN_SELECT) || uMsg == WM_COMMAND)
	{
		// Restore main window
		ShowWindow(mainHwnd, SW_SHOW);
		Shell_NotifyIcon(NIM_DELETE, &nid);
		if (uMsg == WM_COMMAND && wParam == ID_CLOSE)
			SendMessage(mainHwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
		else
			SetForegroundWindow(mainHwnd);
	}
	else if (uMsg == WM_USER + 1337 && LOWORD(lParam) == WM_CONTEXTMENU)
	{
		SetForegroundWindow(hwnd);
		POINT pt;
		GetCursorPos(&pt);
		HMENU hMenu = CreatePopupMenu();
		AppendMenu(hMenu, MF_STRING, ID_RESTORE, _T("&Restore Thunderbird"));
		AppendMenu(hMenu, MF_STRING, ID_CLOSE, _T("&Close Thunderbird"));
		TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
		DestroyMenu(hMenu);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MessageHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	static HWND trayHwnd = nullptr;
	MSG &msg = *(MSG *)lParam;

	if(mainHwnd == nullptr)
	{
		/*
		 * The main UI thread handles several windows and we need to subclass the right one,
		 * so check the window title until we see the proper Thunderbird window pass by.
		 */
		static TCHAR winTitle[256];
		GetWindowText(msg.hwnd, winTitle, _countof(winTitle));
		if(_tcsstr(winTitle, L"- Mozilla Thunderbird"))
		{
			mainHwnd = msg.hwnd;

			static const TCHAR windowClassName[] = _T("ThunderbirdMinToTrayIcon");
			WNDCLASSEX wndClass;
			wndClass.cbSize = sizeof(WNDCLASSEX);
			wndClass.style = 0;
			wndClass.lpfnWndProc = TrayIconProc;
			wndClass.cbClsExtra = 0;
			wndClass.cbWndExtra = 0;
			wndClass.hInstance = GetModuleHandle(nullptr);
			wndClass.hIcon = nullptr;
			wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wndClass.hbrBackground = nullptr;
			wndClass.lpszMenuName = nullptr;
			wndClass.lpszClassName = windowClassName;
			wndClass.hIconSm = nullptr;
			RegisterClassEx(&wndClass);

			trayHwnd = CreateWindow(
				windowClassName,
				_T("Thunderbird Tray Icon"),
				WS_POPUP | WS_VISIBLE,
				CW_USEDEFAULT, CW_USEDEFAULT,
				1, 1,
				HWND_MESSAGE,
				nullptr,
				GetModuleHandle(nullptr),
				nullptr);
		}

	}

	if(msg.hwnd == mainHwnd &&
		(
		(msg.message == WM_NCLBUTTONDOWN && (msg.wParam == HTCLOSE || msg.wParam == HTMINBUTTON))
		||
		(msg.message == WM_SYSCOMMAND && (msg.wParam == SC_CLOSE || msg.wParam == SC_MINIMIZE))
		))
	{
		ShowWindow(msg.hwnd, SW_HIDE);
		// Show tray icon
		nid.cbSize = sizeof(nid);
		nid.hWnd = trayHwnd;
		nid.uID = 1337;
		nid.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP | NIF_MESSAGE;
		nid.uCallbackMessage = WM_USER + 1337;
		nid.hIcon = (HICON)GetClassLongPtr(mainHwnd, GCLP_HICON);
		lstrcpy(nid.szTip, _T("Mozilla Thunderbird"));
		nid.dwState = 0;
		nid.dwStateMask = 0;
		lstrcpy(nid.szInfo, _T(""));
		nid.uVersion = NOTIFYICON_VERSION_4;
		lstrcpy(nid.szInfoTitle, _T(""));
		nid.dwInfoFlags = 0;
		nid.guidItem = {};
		nid.hBalloonIcon = nid.hIcon;
		Shell_NotifyIcon(NIM_ADD, &nid);
		Shell_NotifyIcon(NIM_SETVERSION, &nid);
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
