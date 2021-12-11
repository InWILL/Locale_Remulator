#pragma once
#include<Windows.h>
#include"LROriginalFunc.h"
#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

extern LRProfile settings;

void HookFunctions();
extern LPVOID HookDllFunc(LPCSTR lpszFuncName, LPVOID lpHookAddress, HMODULE hDLL);
//extern LPVOID HookFunc(LPVOID lpszFuncName, LPVOID lpHookAddress);
UINT WINAPI HookGdiGetCodePage(HDC hdc);
UINT WINAPI HookGetACP(void);
int WINAPI HookMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
HWND WINAPI HookCreateWindowExA(
	DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
LRESULT WINAPI HookSendMessageA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//LRESULT WINAPI HookCallWindowProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI HookMultiByteToWideChar(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
int WINAPI HookWideCharToMultiByte(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);


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