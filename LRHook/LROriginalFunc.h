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

static int (WINAPI* OriginalGetUserDefaultLocaleName)(
	LPWSTR lpLocalName,
	int cchLocalName) = GetUserDefaultLocaleName;

static int (WINAPI* OriginalGetSystemDefaultLocaleName)(
	LPWSTR lpLocalName,
	int cchLocalName) = GetSystemDefaultLocaleName;

static UINT(WINAPI* OriginalGetACP)(void) = GetACP;
static UINT(WINAPI* OriginalGetOEMCP)(void) = GetOEMCP;
static BOOL(WINAPI* OriginalGetCPInfo)(
	UINT       CodePage,
	LPCPINFO  lpCPInfo
	) = GetCPInfo;

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

static LRESULT(WINAPI* OriginalSendMessageA)(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_Pre_maybenull_ _Post_valid_ WPARAM wParam,
	_Pre_maybenull_ _Post_valid_ LPARAM lParam
	) = SendMessageA;

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

static int(WINAPI* OriginalMessageBoxA)(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType
	) = MessageBoxA;

static BOOL(WINAPI* OriginalSetWindowTextA)(
	_In_ HWND hWnd,
	_In_opt_ LPCSTR lpString
	) = SetWindowTextA;

static int(WINAPI* OriginalGetWindowTextA)(
	_In_ HWND hWnd,
	_Out_writes_(nMaxCount) LPSTR lpString,
	_In_ int nMaxCount
	) = GetWindowTextA;

static LONG(WINAPI* OriginalImmGetCompositionStringA)(
	HIMC hIMC,
	DWORD dwIndex,
	LPVOID lpBuf,
	DWORD  dwBufLen
	) = ImmGetCompositionStringA;

static DWORD(WINAPI* OriginalImmGetCandidateListA)(
	HIMC            hIMC,
	DWORD           deIndex,
	LPCANDIDATELIST lpCandList,
	DWORD           dwBufLen
	) = ImmGetCandidateListA;

static HFONT(WINAPI* OriginalCreateFontA)(
	_In_ int cHeight,
	_In_ int cWidth,
	_In_ int cEscapement,
	_In_ int cOrientation,
	_In_ int cWeight,
	_In_ DWORD bItalic,
	_In_ DWORD bUnderline,
	_In_ DWORD bStrikeOut,
	_In_ DWORD iCharSet,
	_In_ DWORD iOutPrecision,
	_In_ DWORD iClipPrecision,
	_In_ DWORD iQuality,
	_In_ DWORD iPitchAndFamily,
	_In_opt_ LPCSTR pszFaceName
	) = CreateFontA;

static HFONT(WINAPI* OriginalCreateFontW)(
	_In_ int cHeight,
	_In_ int cWidth,
	_In_ int cEscapement,
	_In_ int cOrientation,
	_In_ int cWeight,
	_In_ DWORD bItalic,
	_In_ DWORD bUnderline,
	_In_ DWORD bStrikeOut,
	_In_ DWORD iCharSet,
	_In_ DWORD iOutPrecision,
	_In_ DWORD iClipPrecision,
	_In_ DWORD iQuality,
	_In_ DWORD iPitchAndFamily,
	_In_opt_ LPCWSTR pszFaceName
	) = CreateFontW;

static HFONT(WINAPI* OriginalCreateFontIndirectA)(
	const LOGFONTA* lplf
	) = CreateFontIndirectA;

static HFONT(WINAPI* OriginalCreateFontIndirectW)(
	_In_ CONST LOGFONTW* lplf
	) = CreateFontIndirectW;

static HFONT(WINAPI* OriginalCreateFontIndirectExA)(
	_In_ CONST ENUMLOGFONTEXDVA*
	) = CreateFontIndirectExA;

static HFONT(WINAPI* OriginalCreateFontIndirectExW)(
	_In_ CONST ENUMLOGFONTEXDVW*
	) = CreateFontIndirectExW;

static BOOL(WINAPI* OriginalTextOutA)(
	HDC    hdc,
	int    x,
	int    y,
	LPCSTR lpString,
	int    c
	) = TextOutA;

static int(WINAPI* OriginalDrawTextExA)(
	_In_ HDC hdc,
	LPSTR lpchText,
	_In_ int cchText,
	_Inout_ LPRECT lprc,
	_In_ UINT format,
	_In_opt_ LPDRAWTEXTPARAMS lpdtp
	) = DrawTextExA;

static HANDLE(WINAPI* OriginalGetClipboardData)(
	_In_ UINT uFormat
	) = GetClipboardData;

static HANDLE(WINAPI* OriginalSetClipboardData)(
	_In_ UINT uFormat,
	_In_opt_ HANDLE hMem
	) = SetClipboardData;

static HRESULT(WINAPI* OriginalDirectSoundEnumerateA)(
	_In_ LPDSENUMCALLBACKA pDSEnumCallback,
	_In_opt_ LPVOID pContext
	) = DirectSoundEnumerateA;

static LPSTR(WINAPI* OriginalCharPrevExA)(
	_In_ WORD CodePage,
	_In_ LPCSTR lpStart,
	_In_ LPCSTR lpCurrentChar,
	_In_ DWORD dwFlags
	) = CharPrevExA;

static LPSTR(WINAPI* OriginalCharNextExA)(
	_In_ WORD CodePage,
	_In_ LPCSTR lpCurrentChar,
	_In_ DWORD dwFlags
	) = CharNextExA;

static BOOL(WINAPI* OriginalIsDBCSLeadByteEx)(
	_In_ UINT  CodePage,
	_In_ BYTE  TestChar
	) = IsDBCSLeadByteEx;

static INT_PTR(WINAPI* OriginalDialogBoxParamA)(
	_In_opt_ HINSTANCE hInstance,
	_In_ LPCSTR lpTemplateName,
	_In_opt_ HWND hWndParent,
	_In_opt_ DLGPROC lpDialogFunc,
	_In_ LPARAM dwInitParam
	) = DialogBoxParamA;

static HWND(WINAPI* OriginalCreateDialogIndirectParamA)(
	_In_opt_ HINSTANCE hInstance,
	_In_ LPCDLGTEMPLATEA lpTemplate,
	_In_opt_ HWND hWndParent,
	_In_opt_ DLGPROC lpDialogFunc,
	_In_ LPARAM dwInitParam
	) = CreateDialogIndirectParamA;

static BOOL(WINAPI* OriginalVerQueryValueA)(
	LPCVOID pBlock,
	LPCSTR lpSubBlock,
	LPVOID* lplpBuffer,
	PUINT puLen
) = VerQueryValueA;

static DWORD(WINAPI* OriginalGetModuleFileNameA)(
	HMODULE hModule,
	LPSTR lpFilename,
	DWORD nSize
	) = GetModuleFileNameA;

static HMODULE(WINAPI* OriginalLoadLibraryExA)(
	_In_ LPCSTR lpLibFileName,
	_Reserved_ HANDLE hFile,
	_In_ DWORD dwFlags
	) = LoadLibraryExA;

static DWORD(WINAPI* OriginalGetFileVersionInfoSizeA)(
	_In_ LPCSTR lpwstrFilename,
	_Out_ LPDWORD lpdwHandle
	) = GetFileVersionInfoSizeA;

static BOOL(WINAPI* OriginalGetFileVersionInfoA)(
	_In_                LPCSTR lptstrFilename, /* Filename of version stamped file */
	_Reserved_          DWORD dwHandle,          /* Information from GetFileVersionSize */
	_In_                DWORD dwLen,             /* Length of buffer for info */
	_Out_writes_bytes_(dwLen) LPVOID lpData            /* Buffer to place the data structure */
	) = GetFileVersionInfoA;

static BOOL(WINAPI* OriginalPathRenameExtensionA)(
	LPSTR pszPath,
	LPCSTR pszExt
	) = PathRenameExtensionA;

static ATOM(WINAPI* OriginalRegisterClassA)(
	_In_ CONST WNDCLASSA* lpWndClass
	) = RegisterClassA;

static ATOM(WINAPI* OriginalRegisterClassExA)(
	_In_ CONST WNDCLASSEXA* lpWndClass
	) = RegisterClassExA;

static LRESULT(WINAPI* OriginalDefWindowProcA)(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	) = DefWindowProcA;

static LCID(WINAPI* OriginalGetThreadLocale)() = GetThreadLocale;
static LANGID(WINAPI* OriginalGetSystemDefaultUILanguage)() = GetSystemDefaultUILanguage;
static LANGID(WINAPI* OriginalGetUserDefaultUILanguage)() = GetUserDefaultUILanguage;
static LCID(WINAPI* OriginalGetSystemDefaultLCID)() = GetSystemDefaultLCID;
static LCID(WINAPI* OriginalGetUserDefaultLCID)() = GetUserDefaultLCID;
static LANGID(WINAPI* OriginalGetSystemDefaultLangID)() = GetSystemDefaultLangID;
static LANGID(WINAPI* OriginalGetUserDefaultLangID)() = GetUserDefaultLangID;

static DWORD(WINAPI* OriginalGetTimeZoneInformation)(
	_Out_ LPTIME_ZONE_INFORMATION lpTimeZoneInformation
	) = GetTimeZoneInformation;

static BOOL(WINAPI* OriginalCreateDirectoryA)(
	_In_ LPCSTR lpPathName,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes
) = CreateDirectoryA;

static HANDLE(WINAPI* OriginalCreateFileA)(
	_In_ LPCSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
	) = CreateFileA;

static int(WINAPI* OriginalGetLocaleInfoA)(
	_In_ LCID Locale,
	_In_ LCTYPE LCType,
	_Out_writes_opt_(cchData) LPSTR lpLCData,
	_In_ int cchData
	) = GetLocaleInfoA;

static int(WINAPI* OriginalGetLocaleInfoW)(
	_In_ LCID Locale,
	_In_ LCTYPE LCType,
	_Out_writes_opt_(cchData) LPWSTR lpLCData,
	_In_ int cchData
) = GetLocaleInfoW;