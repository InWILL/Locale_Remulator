#include<iostream>
#include<Windows.h>
#include<detours.h>
#include<cstdio>
#include<cstring>

#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

using namespace System::Reflection;
using namespace System::Runtime::InteropServices;

#ifdef _DEBUG
#using <../LRSubMenu/bin/Debug/LRSubMenus.dll>
#else
#using <../LRSubMenu/bin/Release/LRSubMenus.dll>
#endif

//extern "C" __declspec(dllexport) int LRInject(char* filepath, char* dllpath, UINT CodePage)
//int main(int argc,char* argv[])
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	if (__argc < 3)
	{
		MessageBox(NULL, TEXT("Please use LREditor to run the application."), TEXT("LRProc"), NULL);
		return 0;
	}
	char* DllPath = __argv[1];
	lpCmdLine = strstr(lpCmdLine, __argv[2]);
	char* CommandLine = strstr(lpCmdLine, __argv[3]);
	char* Font;
	System::String^ Guid = gcnew System::String(__argv[2]);
	LRCSharpLibrary::LRProfile^ alpha = LRCSharpLibrary::LRConfig::GetProfile(Guid);
	Font = (char*)(void*)Marshal::StringToHGlobalAnsi(alpha->Font);
	
	LRProfile beta;
	beta.CodePage = alpha->CodePage;
	beta.HookIME = alpha->HookIME;
    strcpy(beta.lfFaceName, Font);

	LRConfigFileMap filemap;
	filemap.WrtieConfigFileMap(&beta);
	
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
    //std::cout << beta.CodePage;

	DetourCreateProcessWithDllExA(NULL, CommandLine, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi, DllPath, NULL);
    
	//Sleep(30000);
	//WaitForSingleObject(pi.hProcess, INFINITE);
	
	//filemap.FreeConfigFileMap();
	return 0;
}
