#include <iostream>
#include <Windows.h>
#include <detours.h>

#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

using namespace System::Reflection;

#ifdef _DEBUG
#using <../LRSubMenu/bin/Debug/LRSubMenus.dll>
#else
#using <../LRSubMenu/bin/Release/LRSubMenus.dll>
#endif

//extern "C" __declspec(dllexport) int LRInject(char* filepath, char* dllpath, UINT CodePage)
int main(int argc,char *argv[])
{
	char* filepath = argv[1];
	char* dllpath = argv[3];
	System::String^ Guid = gcnew System::String(argv[2]);
	System::String^ ConfigPath = gcnew System::String(argv[4]);
	LRCSharpLibrary::LRProfile^ alpha = LRCSharpLibrary::LRConfig::GetProfile(Guid, ConfigPath);

	LRProfile beta;
	beta.CodePage = alpha->CodePage;
	strcpy(beta.DllPath, dllpath);
	LRConfigFileMap filemap;
	filemap.WrtieConfigFileMap(&beta);
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);

	DWORD type = 0;
	GetBinaryType(filepath, &type);
	
	DetourCreateProcessWithDllEx(NULL, filepath, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi, dllpath, NULL);

	WaitForSingleObject(pi.hProcess, INFINITE);
	filemap.FreeConfigFileMap();
	return 0;
}
