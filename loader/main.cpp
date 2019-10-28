/*
 * TBTray Loader by Johannes Schultz
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

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>


TCHAR *GetErrorMessage()
{
	static TCHAR msgbuf[1024];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msgbuf, _countof(msgbuf), nullptr);
	return msgbuf;
}


static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	if(hwnd)
	{
		TCHAR name[256] = {};
		if(::GetClassName(hwnd, name, 256) > 0)
		{
			if(!lstrcmp(name, _T("MozillaWindowClass")))
			{
				GetWindowText(hwnd, name, 256);
				if(_tcsstr(name, _T("- Mozilla Thunderbird")))
				{
					*reinterpret_cast<HWND *>(lParam) = hwnd;
					return FALSE; // done
				}
			}
		}
	}
	return TRUE; // continue
}

static void CALLBACK TimerProc(HWND, UINT, UINT_PTR idTimer, DWORD)
{
	static HWND hwnd = nullptr;
	if(IsWindow(hwnd))
		return; // We already hooked it

	hwnd = nullptr;
	::EnumWindows(&EnumWindowsProc, reinterpret_cast<LPARAM>(&hwnd));
	if(hwnd == nullptr)
		return; // Thunderbird is not running

	// Construct the DLL filename from our own filename, then attempt to load the DLL and find the entry hook
	TCHAR dllName[1024];
	int dllNameLen = GetModuleFileName(nullptr, dllName, _countof(dllName));
	lstrcpy(dllName + dllNameLen - 3, _T("dll"));
	HMODULE dll = LoadLibrary(dllName);
	HOOKPROC hookProc = (HOOKPROC)GetProcAddress(dll, "EntryHook");
	if(!dll || !hookProc)
	{
		MessageBox(nullptr, _T("Could not find the companion DLL. Make sure it's in the same directory as the EXE and has the same name."), _T("TBTray"), MB_ICONEXCLAMATION);
		hwnd = nullptr;
		return;
	}

	DWORD threadID = GetWindowThreadProcessId(hwnd, nullptr);
	HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, hookProc, dll, threadID);
	if(!hook)
	{
		MessageBox(nullptr, GetErrorMessage(), _T("Failed to hook Thunderbird UI thread"), MB_ICONEXCLAMATION);
		hwnd = nullptr;
		return;
	}
	FreeLibrary(dll);

	// Send a message to the hooked WindowProc to make sure the DLL install code ran before we exit
	SendMessage(hwnd, WM_NULL, 0, 0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int argc = 0;
	auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argc > 1 && !wcscmp(argv[1], L"register"))
	{
		TCHAR path[1024];
		auto len = GetModuleFileName(nullptr, path, _countof(path));

		HKEY hkey = nullptr;
		RegCreateKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
		bool ok = false;
		if (hkey)
		{
			ok = RegSetValueEx(hkey, _T("TBTray"), 0, REG_SZ, (BYTE *)path, (len + 1) * 2) == ERROR_SUCCESS;
			RegCloseKey(hkey);
			MessageBox(nullptr, ok ? _T("TBTray has been successfully registered.") : _T("An error occurred while registering TBTray."), _T("TBTray"), MB_OK | MB_ICONINFORMATION);
		}
		return ok ? 0 : 1;
	}
	else if (argc > 1 && !wcscmp(argv[1], L"unregister"))
	{
		HKEY hkey = nullptr;
		bool ok = false;
		RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
		if (hkey)
		{
			ok = RegDeleteValue(hkey, _T("TBTray")) == ERROR_SUCCESS;
			RegCloseKey(hkey);
		}
		MessageBox(nullptr, ok ? _T("TBTray has been successfully unregistered.") : _T("An error occurred while unregistering TBTray. Maybe it was not registered?"), _T("TBTray"), MB_OK | MB_ICONINFORMATION);
		return ok ? 0 : 1;
	}


	SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
	auto timerID = SetTimer(nullptr, 1, 1000, TimerProc);

	MSG msg;
	while(::GetMessage(&msg, nullptr, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

#ifdef _DEBUG
	MessageBox(nullptr, _T("DLL injected - hit OK to unload"), _T("TBTray"), MB_ICONINFORMATION);

	/* Tell the DLL to unhook and unload itself */
	/*if (0xBABE != SendMessage(hTraktorWindow, WM_APP + 9999, 0xCAFE, 0xDEADBEEF))
		MessageBox(0, "Failed to unhook", "Yikes", MB_ICONEXCLAMATION);
	UnhookWindowsHookEx(hook);*/
#endif

	return 0;
}
