#pragma once
#include<Windows.h>

static int (WINAPI* OriginalMultiByteToWideChar)(
	UINT CodePage, 
	DWORD dwFlags,
	LPCSTR lpMultiByteStr, 
	int cbMultiByte, 
	LPWSTR lpWideCharStr, 
	int cchWideChar
	) = MultiByteToWideChar;


static int (WINAPI* OriginalWideCharToMultiByte)(
	UINT CodePage,
	DWORD dwFlags,
	LPCWSTR lpWideCharStr,
	int cchWideChar,
	LPSTR lpMultiByteStr,
	int cbMultiByte,
	LPCSTR lpDefaultChar,
	LPBOOL lpUsedDefaultChar
	) = WideCharToMultiByte;

static UINT(WINAPI* OriginalGetACP)(void) = GetACP;
static UINT(WINAPI* OriginalGetOEMCP)(void) = GetOEMCP;

static LRESULT(WINAPI* OriginalSendMessageA)(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_Pre_maybenull_ _Post_valid_ WPARAM wParam,
	_Pre_maybenull_ _Post_valid_ LPARAM lParam) = SendMessageA;

static UINT(WINAPI* OriginalWinExec)(
	_In_ LPCSTR lpCmdLine,
	_In_ UINT uCmdShow
	) = WinExec;

static int(WINAPI* OriginalMessageBoxA)(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType) = MessageBoxA;

static LRESULT(WINAPI* OriginalDefWindowProcA)(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam) = DefWindowProcA;

static LRESULT(WINAPI* OriginalDefDlgProcA)(
	_In_ HWND hDlg,
	_In_ UINT Msg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam) = DefDlgProcA;