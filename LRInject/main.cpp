#include <iostream>
#include <Windows.h>
#include"../LRCommonLibrary/LRCommonLibrary.h"

#pragma comment(lib, "LRCommonLibrary.lib")

bool inject(DWORD pId, char* dllToInject);
DWORD startup(char* cmdline);

using namespace std;

int main(int argc, char* argv[])
{
	LRConfigFileMap filemap;
	filemap.WrtieConfigFileMap();
	DWORD pid = startup(argv[1]);
	inject(pid, argv[2]);
	Sleep(100);
	filemap.FreeConfigFileMap();
	return 0;
}

DWORD startup(char* cmdline)
{
	int size = MultiByteToWideChar(CP_ACP, 0, cmdline, -1, NULL, 0);
	LPWSTR wCmdline = new WCHAR[size];
	MultiByteToWideChar(CP_ACP, 0, cmdline, -1, wCmdline, size);

	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	CreateProcess(NULL,   // the path
		wCmdline,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return pi.dwProcessId;
}

bool inject(DWORD pId, char* dllToInject)
{
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pId);
	if (processHandle)
	{
		/*
		Retrieves the address of an exported function or variable from the specified dynamic-link library (DLL).
		If the function succeeds, the return value is the address of the exported function or variable.
		If the function fails, the return value is NULL. To get extended error information, call GetLastError.
		*/
		//LPVOID loadLibAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

		/*
		Reserves, commits, or changes the state of a region of memory within the virtual address space of a specified process. The function initializes the memory it allocates to zero.
		If the function succeeds, the return value is the base address of the allocated region of pages.
		If the function fails, the return value is NULL.To get extended error information, call GetLastError.
		*/
		LPVOID baseAddress = VirtualAllocEx(processHandle, NULL, strlen(dllToInject), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		/*
		Writes data to an area of memory in a specified process. The entire area to be written to must be accessible or the operation fails.
		*/
		WriteProcessMemory(processHandle, baseAddress, dllToInject, strlen(dllToInject), NULL);

		/*
		Creates a thread that runs in the virtual address space of another process.
		If the function succeeds, the return value is a handle to the new thread.
		If the function fails, the return value is NULL.To get extended error information, call GetLastError.
		*/
		HANDLE remoteThreadHandle = CreateRemoteThread(processHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, baseAddress, 0, NULL);

		/*
		Waits until the specified object is in the signaled state or the time-out interval elapses.
		*/
		WaitForSingleObject(remoteThreadHandle, INFINITE);

		/*
		Cleanup
		*/
		//VirtualFreeEx(processHandle, baseAddress, strlen(dllToInject), MEM_RELEASE);
		CloseHandle(remoteThreadHandle);
		//CloseHandle(processHandle);

		return true;
	}
	return false;
}