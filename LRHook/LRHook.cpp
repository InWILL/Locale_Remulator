#include<Windows.h>
#include <iostream>
#include <assert.h>
#include <detours/detours.h>

#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

#include "../LRHookFunc/LRHookFunc.h"
#pragma comment(lib, "LRHookFunc.lib")



//typedef int (WINAPI* MESSAGEBOXA)(HWND, LPCSTR, LPCSTR, UINT);
//MESSAGEBOXA fpMessageBoxA = NULL;


LRProfile settings;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	if (DetourIsHelperProcess()) {
		return TRUE;
	}
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		std::cout << "DLL_PROCESS_ATTACH\n";
		LRConfigFileMap filemap;
		
		filemap.ReadConfigFileMap(&settings);
		filemap.FreeConfigFileMap();
		
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		AttachFunctions();

		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
		std::cout << "DLL_THREAD_ATTACH\n";
		break;
	case DLL_THREAD_DETACH:
		std::cout << "DLL_THREAD_DETACH\n";
		break;
	case DLL_PROCESS_DETACH:
		std::cout << "DLL_PROCESS_DETACH\n";
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetachFunctions();

		DetourTransactionCommit();
		break;
	}
	return true;
}
