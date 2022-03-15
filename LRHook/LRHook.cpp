#include<Windows.h>
#include<detours.h>

#include "LRHookFunc.h"

#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")


//typedef int (WINAPI* MESSAGEBOXA)(HWND, LPCSTR, LPCSTR, UINT);
//MESSAGEBOXA fpMessageBoxA = NULL;


LRProfile settings;
//std::wofstream filelog;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	if (DetourIsHelperProcess()) {
		return TRUE;
	}
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		std::cout << "DLL_PROCESS_ATTACH\n";
		//filelog.open("test.log", std::ios::out);

		LRConfigFileMap filemap;
		filemap.ReadConfigFileMap(&settings);
		filemap.FreeConfigFileMap();
		GetModuleFileNameA(hModule, Original.DllPath, MAX_PATH);
		//settings.hHeap = HeapCreate(0, 0, 0);
		//filelog << settings.CodePage << std::endl;
		Original.hHeap = GetProcessHeap();

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
		
		//filelog.close();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetachFunctions();

		DetourTransactionCommit();
		break;
	}
	return true;
}
