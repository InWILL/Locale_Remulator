#pragma once
#include<Windows.h>
#include<detours.h>
#include"LROriginalFunc.h"
#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

extern LRProfile settings;
extern std::ofstream filelog;

void AttachFunctions();
void DetachFunctions();
UINT WINAPI HookGetACP(void);
UINT WINAPI HookGetOEMCP(void);
HWND WINAPI HookCreateWindowExA(
	DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
LRESULT WINAPI HookSendMessageA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//LRESULT WINAPI HookCallWindowProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI HookMultiByteToWideChar(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
int WINAPI HookWideCharToMultiByte(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
UINT WINAPI HookWinExec(
	_In_ LPCSTR lpCmdLine,
	_In_ UINT uCmdShow
);
int WINAPI HookMessageBoxA(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType);

LRESULT WINAPI HookDefWindowProcA(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam);

LRESULT WINAPI HookDefDlgProcA(
	_In_ HWND hDlg,
	_In_ UINT Msg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam);

//Minhook version Code
/*LONG AttachDllFunc(LPCSTR lpszFuncName, LPVOID lpHookAddress, HMODULE hDLL)
{
	LPVOID funcptr = hDLL ? (LPVOID)(DWORD_PTR)GetProcAddress(hDLL, lpszFuncName) : (LPVOID)lpszFuncName;
	LPVOID outputptr;
	return DetourAttach(&funcptr, lpHookAddress);
	// return the original funcaddress !
}

LONG DetachDllFunc(LPCSTR lpszFuncName, LPVOID lpHookAddress, HMODULE hDLL)
{
	LPVOID funcptr = hDLL ? (LPVOID)(DWORD_PTR)GetProcAddress(hDLL, lpszFuncName) : (LPVOID)lpszFuncName;
	LPVOID outputptr;
	return DetourDetach(&funcptr, lpHookAddress);
	// return the original funcaddress !
}*/


inline VOID FreeStringInternal(LPVOID pBuffer/*ecx*/)
{
	HeapFree(GetProcessHeap(), 0, pBuffer);
}

inline LPCWSTR MultiByteToWideCharInternal(LPCSTR lstr)
{
	int lsize = lstrlenA(lstr)/* size without '\0' */, n = 0;
	int wsize = lsize << 1;
	LPWSTR wstr = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, wsize);
	if (wstr) {
		n = MultiByteToWideChar(CP_ACP, 0, lstr, lsize, wstr, wsize);
		wstr[n] = L'\0'; // make tail ! 
	}
	return wstr;
}

inline LPCSTR WideCharToMultiByteInternal(LPCWSTR wstr)
{
	int wsize = lstrlenW(wstr)/* size without '\0' */, n = 0;
	int lsize = wsize;
	LPSTR lstr = (LPSTR)HeapAlloc(GetProcessHeap(), 0, lsize);
	if (lstr) {
		n = WideCharToMultiByte(CP_ACP, 0, wstr, wsize, lstr, lsize, NULL, NULL);
		lstr[n] = '\0'; // make tail ! 
	}
	return lstr;
}