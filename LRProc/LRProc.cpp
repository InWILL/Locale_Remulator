#include<iostream>
#include<Windows.h>
#include<detours.h>

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
	char* filepath = __argv[1];
	char* dllpath = __argv[3];
	System::String^ Guid = gcnew System::String(__argv[2]);
	LRCSharpLibrary::LRProfile^ alpha = LRCSharpLibrary::LRConfig::GetProfile(Guid);

	LRProfile beta;
	beta.CodePage = alpha->CodePage;
    strcpy(beta.lfFaceName, (char*)(void*)Marshal::StringToHGlobalAnsi(alpha->Font));
	beta.HookIME = alpha->HookIME;
	strcpy(beta.DllPath, dllpath);
	LRConfigFileMap filemap;
	filemap.WrtieConfigFileMap(&beta);
	
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
    //std::cout << beta.CodePage;

	DetourCreateProcessWithDllExA(NULL, filepath, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi, dllpath, NULL);
    
	WaitForSingleObject(pi.hProcess, INFINITE);
    Sleep(60000);
	filemap.FreeConfigFileMap();
	return 0;
}
