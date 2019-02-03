#define _CRT_SECURE_NO_WARNINGS
#include "windows.h"

char dllName[2048];
const char *appName = "Traktor Touch";

void getTraktor(LPSTR lpCmdLine, HANDLE &hProcess, HANDLE &hThread)
{
	/* Try to find a running instance of Traktor */
	HWND hTraktor = FindWindow(NULL, "Traktor");
	if (hTraktor) {
		DWORD pid;
		GetWindowThreadProcessId(hTraktor, &pid);
		hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | SYNCHRONIZE, false, pid);
		if (!hProcess) {
			MessageBox(0, "Failed to access the running Traktor process for DLL injection.", appName, MB_ICONEXCLAMATION);
			ExitProcess(1);
		}
		hThread = NULL;
	}
	else {
		/* Launch Traktor ourselves */
		PROCESS_INFORMATION pi;
		STARTUPINFOA si;

		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		si.cb = sizeof(si);
		if (!CreateProcess("Traktor.exe", lpCmdLine, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
			MessageBox(0, "Failed to start Traktor.exe!\n\nMake sure traktor_touch.exe and traktor_touch.dll are in the same directory as Traktor.exe", appName, MB_ICONEXCLAMATION);
			ExitProcess(1);
		}

		hProcess = pi.hProcess;
		hThread = pi.hThread;			 
	}
}

/**
	@brief Call a Windows API function in the context of another process
	@param[in]  hProcess Handle of the process to inject the API call into
	@param[in]  lpModule Module to call the API from
	@param[in]  lpApi    Name of the API function to call
	@param[in]  iParam   Function parameter - integer or pointer
	@param[in]  copySize Amount of bytes to be copied into the remote address space if iParam is a pointer, 0 if iParam is an integer
	@param[out] oResult  Return code of the called API if the call was successfully executed
	@return              true if the injection succeeded
*/
intptr_t callApiInRemoteContext(HANDLE hProcess, LPSTR lpModule, LPSTR lpApi, intptr_t iParam, size_t copySize, DWORD &oResult)
{
	void *lpFunction = GetProcAddress(GetModuleHandle(lpModule), lpApi);
	if (lpFunction == NULL)
		return false;

	void *procMem = NULL;
	if (copySize) {
		procMem = VirtualAllocEx(hProcess, 0, copySize, MEM_COMMIT, PAGE_READWRITE);
		if (procMem == NULL || !WriteProcessMemory(hProcess, procMem, (void *)iParam, copySize, NULL))
			return false;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpFunction, copySize ? procMem : (void *)iParam, NULL, NULL);
	if (!hThread)
		return false;

	oResult = 0;
	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, &oResult);
	if (copySize)
		VirtualFree(procMem, 0, MEM_RELEASE);

	return true;
}

DWORD hDLL = 0;

bool loadDLL(HANDLE hProcess)
{
	return callApiInRemoteContext(hProcess, "kernel32.dll", "LoadLibraryA", (intptr_t)dllName, sizeof(dllName), hDLL) && hDLL;
}

bool unloadDLL(HANDLE hProcess)
{
	DWORD rc = 0;
	return callApiInRemoteContext(hProcess, "kernel32.dll", "FreeLibrary", hDLL, 0, rc) && rc;
}

HHOOK hook;
LRESULT CALLBACK MessageHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(hook, nCode, wParam, lParam);
}

char *GetErrorMessage()
{
	static char msgbuf[1024];
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msgbuf, sizeof(msgbuf), NULL);
	return msgbuf;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int dllNameLen = GetModuleFileName(NULL, dllName, sizeof(dllName));
	strcpy(dllName + dllNameLen - 3, "dll");
	HMODULE dll = LoadLibrary(dllName);
	if (!dll) {
		MessageBox(0, "Could not find the companion DLL. Make sure it's in the same directory as the EXE and has the same name.", appName, MB_ICONEXCLAMATION);
		return 1;
	}

	HWND hTraktorWindow;
	while (!(hTraktorWindow = FindWindow(NULL, "Traktor")))
		Sleep(100);

	HOOKPROC hookProc = (HOOKPROC)GetProcAddress(dll, "EntryHook");
	DWORD idTraktorUIThread = GetWindowThreadProcessId(hTraktorWindow, NULL);
	HHOOK hook = SetWindowsHookEx(WH_GETMESSAGE, hookProc, dll, idTraktorUIThread);
	if (!hook) {
		MessageBox(0, "Failed to hook Traktor UI thread", GetErrorMessage(), MB_ICONEXCLAMATION);
		return 1;
	}
	PostMessage(hTraktorWindow, WM_NULL, 0, 0);

	MessageBox(0, "Yup", appName, MB_ICONINFORMATION);

	if (0xBABE != SendMessage(hTraktorWindow, WM_APP, 0xCAFE, 0xDEADBEEF))
		MessageBox(0, "Failed to unhook", "Yikes", MB_ICONEXCLAMATION);
	UnhookWindowsHookEx(hook);

	return 0;


	strcpy(dllName + dllNameLen - 3, "dll");

	HANDLE hProcess, hThread;
	getTraktor(lpCmdLine, hProcess, hThread);

	if (!loadDLL(hProcess)) {
		if (hThread)
			TerminateProcess(hProcess, 1);
		MessageBox(0, "Failed to inject companion DLL into Traktor.", appName, MB_ICONEXCLAMATION);
		return 1;
	}

	if (hThread) {
		ResumeThread(hThread);
	}
	/*
	else {
		if (IDNO == MessageBox(0, "DLL injected - hit Yes to keep it injected or No to unload the DLL.", appName, MB_ICONINFORMATION | MB_YESNO)) {
			if (!unloadDLL(hProcess))
				MessageBox(0, "Failed to unload the DLL", appName, MB_ICONEXCLAMATION);
		}
	}
	*/

	return 0;
}