#pragma once
#include<Windows.h>

static LPVOID fpMultiByteToWideChar = NULL;
static LPVOID fpWideCharToMultiByte = NULL;

static int WINAPI OriginalMultiByteToWideChar(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
	typedef int(WINAPI* Fn)(UINT, DWORD, LPCSTR, int, LPWSTR, int);
	return ((Fn)(DWORD_PTR)fpMultiByteToWideChar)(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}

static int WINAPI OriginalWideCharToMultiByte(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
	typedef int(WINAPI* Fn)(UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, LPBOOL);
	return ((Fn)(DWORD_PTR)fpWideCharToMultiByte)(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte,
		lpDefaultChar, lpUsedDefaultChar);
}