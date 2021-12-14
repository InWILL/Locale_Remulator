#include <iostream>
#include <Windows.h>
#include<detours/detours.h>

#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

bool inject(DWORD pId, char* dllToInject);
DWORD startup(char* cmdline);

extern "C" __declspec(dllexport) int LRInject(char* filepath, char* dllpath, UINT CodePage)
{
	LRProfile profile;
	profile.CodePage = CodePage;
	LRConfigFileMap filemap;
	filemap.WrtieConfigFileMap(&profile);
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);

	DetourCreateProcessWithDllEx(NULL, filepath, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi, dllpath, NULL);

	Sleep(100);
	filemap.FreeConfigFileMap();
	return 0;
}
