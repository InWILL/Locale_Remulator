#include <iostream>
#include<fstream>
#include <assert.h>
#include "MinHook.h"

#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif

#include "../LRHookFunc/LRHookFunc.h"
#include"../LRCommonLibrary/LRCommonLibrary.h"

#pragma comment(lib, "LRHookFunc.lib")
#pragma comment(lib, "LRCommonLibrary.lib")


//typedef int (WINAPI* MESSAGEBOXA)(HWND, LPCSTR, LPCSTR, UINT);
//MESSAGEBOXA fpMessageBoxA = NULL;

std::ofstream filelog;

extern LPVOID HookDllFunc(LPCSTR lpszFuncName, LPVOID lpHookAddress, HMODULE hDLL)
{
	LPVOID funcptr = hDLL ? (LPVOID)(DWORD_PTR)GetProcAddress(hDLL, lpszFuncName) : (LPVOID)lpszFuncName;
	LPVOID outputptr;
	return MH_CreateHook(funcptr, lpHookAddress, &outputptr) == MH_OK ? (MH_EnableHook(funcptr), outputptr) : NULL;
	// return the original funcaddress !
}

/*extern LPVOID HookFunc(LPVOID lpszFuncName, LPVOID lpHookAddress)
{
	LPVOID outputptr;
	return MH_CreateHook(lpszFuncName, lpHookAddress, &outputptr) == MH_OK ? (MH_EnableHook(lpszFuncName), outputptr) : NULL;
}*/
// Hook shit

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	LRConfigFileMap filemap;
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		std::cout << "DLL_PROCESS_ATTACH\n";
		filemap.ReadConfigFileMap();
		if (MH_Initialize() != MH_OK)
		{
			return 1;
		}
		HookFunctions();
		//filelog.open("test.log");
		break;
	case DLL_THREAD_ATTACH:
		std::cout << "DLL_THREAD_ATTACH\n";
		break;
	case DLL_THREAD_DETACH:
		std::cout << "DLL_THREAD_DETACH\n";
		break;
	case DLL_PROCESS_DETACH:
		std::cout << "DLL_PROCESS_DETACH\n";
		filemap.FreeConfigFileMap();
		MH_Uninitialize();
		break;
	}
	return true;
}
