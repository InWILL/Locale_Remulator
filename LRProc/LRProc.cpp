#include<iostream>
#include<Windows.h>
#include<detours.h>
#include<cstdio>
#include<cstring>
#include<vcclr.h>

#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

using namespace System::Reflection;
using namespace System::Runtime::InteropServices;

#ifdef _DEBUG
#using <..\\LRSubMenu\\bin\\Debug\\LRSubMenus.dll>
#else
#using <..\\LRSubMenu\\bin\\Release\\LRSubMenus.dll>
#endif

const int GuidLength = 37;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow)
{
	LRCSharpLibrary::LRConfig::CheckConfigFile();

	if (__argc < 3)
	{
		MessageBox(
			NULL,
			TEXT(R"(
Welcome to Locale Remulator x64 command line tool.

Usage: LRProc.exe GUID Path Args

GUID	Guid of the target profile (in LRConfig.xml).
Path	Full path of the target application.
Args	Additional arguments will be passed to the application.

You can also run LREditor to use this applicaction.
				)"
			),
			TEXT("Locale Remulator x64"),
			NULL
		);
		return 0;
	}

	System::String^ Guid = gcnew System::String(__wargv[1]);
	System::String^ CurrentPath = LRCSharpLibrary::LRConfig::CurrentPath;
	System::String^ DllPath = System::IO::Path::Combine(CurrentPath, "LRHookx64.dll");

	LPSTR dllpath = (LPSTR)(void*)Marshal::StringToHGlobalAnsi(DllPath);

	lpCmdLine = wcsstr(lpCmdLine, __wargv[1]);
	LPWSTR CommandLine = lpCmdLine + GuidLength;

	LRCSharpLibrary::LRProfile^ alpha = LRCSharpLibrary::LRConfig::GetProfile(Guid);

	LRProfile beta;
	beta.CodePage = alpha->CodePage;
	beta.LCID = alpha->LCID;
	beta.Bias = alpha->Bias; // Bias will become negative in HookGetTimeZoneInformation
	beta.HookIME = alpha->HookIME;
	beta.HookLCID = alpha->HookLCID;

	LRConfigFileMap filemap;
	filemap.WrtieConfigFileMap(&beta);

	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	//std::cout << beta.CodePage;

	DetourCreateProcessWithDllExW(NULL, CommandLine, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi, dllpath, NULL);
	return 0;
}
