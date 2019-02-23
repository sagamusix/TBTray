/*
 * Traktouch loader
 *
 * Finds the active Traktor window, or starts Traktor if there is no window yet,
 * then loads the companion DLL into Traktor via a Windows hook.
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
#include <Shlwapi.h>

char dllName[2048];
const char *appName = "Traktouch";

char *GetErrorMessage()
{
	static char msgbuf[1024];
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msgbuf, sizeof(msgbuf), NULL);
	return msgbuf;
}

/*
 * Find the active Traktor window, or launch Traktor if no window is found.
 */
HWND getTraktor(LPSTR lpCmdLine)
{
	/* Try to find a running instance of Traktor */
	HWND hTraktor = FindWindow(NULL, "Traktor");
	if (!hTraktor) {
		/* Launch Traktor ourselves */
		PROCESS_INFORMATION pi;
		STARTUPINFOA si;
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		si.cb = sizeof(si);

		/* Construct the name of Traktor.exe in the same directory as the DLL */
		PathRemoveFileSpec(dllName);
		lstrcat(dllName, "\\Traktor.exe");

		/* Try the constructed path first; if that doesn't work try the current directory */
		if (!CreateProcess(dllName, lpCmdLine, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi) && 
			!CreateProcess("Traktor.exe", lpCmdLine, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi)) {
			MessageBox(0, "Failed to start Traktor.exe!\n\nMake sure traktouch.exe and traktouch.dll are in the same directory as Traktor.exe", appName, MB_ICONEXCLAMATION);
			ExitProcess(1);
		}
	}

	/* Wait up to 10s for Traktor to open */
	for (int timeout = 0; timeout < 100; timeout++) {
		hTraktor = FindWindow(NULL, "Traktor");
		if (hTraktor)
			return hTraktor;
		Sleep(100);
	}

	MessageBox(0, "Traktor main window did not open within 10s, giving up.", appName, MB_ICONEXCLAMATION);
	ExitProcess(1);
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	/* Construct the DLL filename from our own filename, then attempt to load the DLL and find the entry hook */
	int dllNameLen = GetModuleFileName(NULL, dllName, sizeof(dllName));
	strcpy(dllName + dllNameLen - 3, "dll");
	HMODULE dll = LoadLibrary(dllName);
	HOOKPROC hookProc = (HOOKPROC)GetProcAddress(dll, "EntryHook");
	if (!dll || !hookProc) {
		MessageBox(0, "Could not find the companion DLL. Make sure it's in the same directory as the EXE and has the same name.", appName, MB_ICONEXCLAMATION);
		return 1;
	}

	/* Grab Traktor and install the entry hook */
	HWND hTraktorWindow = getTraktor(lpCmdLine);
	DWORD idTraktorUIThread  = GetWindowThreadProcessId(hTraktorWindow, NULL);
	HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, hookProc, dll, idTraktorUIThread);
	if (!hook) {
		MessageBox(0, "Failed to hook Traktor UI thread", GetErrorMessage(), MB_ICONEXCLAMATION);
		return 1;
	}
	FreeLibrary(dll);

	/* Send a message to the hooked WindowProc to make sure the DLL install code ran before we exit */
	SendMessage(hTraktorWindow, WM_NULL, 0, 0);

#ifdef _DEBUG
	MessageBox(0, "DLL injected - hit OK to unload", appName, MB_ICONINFORMATION);

	/* Tell the DLL to unhook and unload itself */
	if (0xBABE != SendMessage(hTraktorWindow, WM_APP, 0xCAFE, 0xDEADBEEF))
		MessageBox(0, "Failed to unhook", "Yikes", MB_ICONEXCLAMATION);
	UnhookWindowsHookEx(hook);
#endif

	return 0;
}