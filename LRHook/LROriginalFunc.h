#pragma once
#include<Windows.h>
#include<imm.h>
#include<DSound.h>
#include<shlwapi.h>
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Dsound.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Shlwapi.lib")

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
static BOOL(WINAPI* OriginalGetCPInfo)(
	UINT       CodePage,
	LPCPINFO  lpCPInfo
	) = GetCPInfo;

static UINT(WINAPI* OriginalWinExec)(
	_In_ LPCSTR lpCmdLine,
	_In_ UINT uCmdShow
	) = WinExec;

static BOOL(WINAPI* OriginalCreateProcessA)(
	_In_opt_ LPCSTR lpApplicationName,
	_Inout_opt_ LPSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOA lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
	) = CreateProcessA;

static BOOL(WINAPI* OriginalCreateProcessW)(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
	) = CreateProcessW;

static HINSTANCE(WINAPI* OriginalShellExecuteA)(
	_In_opt_ HWND hwnd, 
	_In_opt_ LPCSTR lpOperation, 
	_In_ LPCSTR lpFile, 
	_In_opt_ LPCSTR lpParameters,
	_In_opt_ LPCSTR lpDirectory, 
	_In_ INT nShowCmd
	)= ShellExecuteA;

static HINSTANCE(WINAPI* OriginalShellExecuteW)(
	_In_opt_ HWND hwnd,
	_In_opt_ LPCWSTR lpOperation,
	_In_ LPCWSTR lpFile,
	_In_opt_ LPCWSTR lpParameters,
	_In_opt_ LPCWSTR lpDirectory,
	_In_ INT nShowCmd
	) = ShellExecuteW;

static BOOL(WINAPI* OriginalShellExecuteExA)(
	_Inout_ SHELLEXECUTEINFOA* pExecInfo
	) = ShellExecuteExA;

static BOOL(WINAPI* OriginalShellExecuteExW)(
	_Inout_ SHELLEXECUTEINFOW* pExecInfo
	) = ShellExecuteExW;

static LCID(WINAPI* OriginalGetThreadLocale)() = GetThreadLocale;
static LANGID(WINAPI* OriginalGetSystemDefaultUILanguage)() = GetSystemDefaultUILanguage;
static LANGID(WINAPI* OriginalGetUserDefaultUILanguage)() = GetUserDefaultUILanguage;
static LCID(WINAPI* OriginalGetSystemDefaultLCID)() = GetSystemDefaultLCID;
static LCID(WINAPI* OriginalGetUserDefaultLCID)() = GetUserDefaultLCID;
static LANGID(WINAPI* OriginalGetSystemDefaultLangID)() = GetSystemDefaultLangID;
static LANGID(WINAPI* OriginalGetUserDefaultLangID)() = GetUserDefaultLangID;

static HWND(WINAPI* OriginalCreateWindowExA)(
	_In_ DWORD dwExStyle,
	_In_opt_ LPCSTR lpClassName,
	_In_opt_ LPCSTR lpWindowName,
	_In_ DWORD dwStyle,
	_In_ int X,
	_In_ int Y,
	_In_ int nWidth,
	_In_ int nHeight,
	_In_opt_ HWND hWndParent,
	_In_opt_ HMENU hMenu,
	_In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID lpParam
	) = CreateWindowExA;