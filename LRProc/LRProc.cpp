#include <iostream>
#include <Windows.h>
#include <detours.h>

#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

#ifdef _DEBUG
#using <../LRSubMenu/bin/Debug/LRSubMenus.dll>
#else
#using <../LRSubMenu/bin/Release/LRSubMenus.dll>
#endif

//extern "C" __declspec(dllexport) int LRInject(char* filepath, char* dllpath, UINT CodePage)
int _tmain(int argc,_TCHAR *argv[])
{
	_TCHAR* filepath = argv[1];
	char* dllpath32 = "LRHook32.dll";
	char* dllpath64 = "LRHook64.dll";
	System::String^ Guid = gcnew System::String(argv[2]);
	LRCSharpLibrary::LRProfile^ alpha = LRCSharpLibrary::LRConfig::GetProfile(Guid);

	LRProfile beta;
	beta.CodePage = alpha->CodePage;

	LRConfigFileMap filemap;
	filemap.WrtieConfigFileMap(&beta);
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);

	DWORD type = 0;
	GetBinaryType(filepath, &type);
	if (type == SCS_64BIT_BINARY)
		DetourCreateProcessWithDllEx(NULL, filepath, NULL,
			NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
			&si, &pi, dllpath64, NULL);
	else
		DetourCreateProcessWithDllEx(NULL, filepath, NULL,
			NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
			&si, &pi, dllpath32, NULL);

	Sleep(100);
	filemap.FreeConfigFileMap();
	return 0;
}
