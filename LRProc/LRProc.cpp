#include <iostream>
#include <Windows.h>
#include<detours/detours.h>

#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

#ifdef _DEBUG
#using <../LRSubMenu/bin/Debug/LRSubMenus.dll>
#else
#using <../LRSubMenu/bin/Release/LRSubMenus.dll>
#endif

//extern "C" __declspec(dllexport) int LRInject(char* filepath, char* dllpath, UINT CodePage)
int main(int argc,char *argv[])
{
	char* filepath = argv[1];
	char* dllpath = argv[2];
	System::String^ Guid = gcnew System::String(argv[3]);
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

	DetourCreateProcessWithDllEx(NULL, filepath, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi, dllpath, NULL);

	Sleep(100);
	filemap.FreeConfigFileMap();
	return 0;
}
