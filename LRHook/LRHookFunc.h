#pragma once
#include<Windows.h>
#include<detours.h>
#include"LROriginalFunc.h"
#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

struct ORIGINAL
{
	HANDLE hHeap;
	UINT CodePage;
	char DllPath[MAX_PATH];
	const char* lpDefaultChar = "";
	BOOL lpUsedDefaultChar = TRUE;
};

extern LRProfile settings;
extern ORIGINAL Original;
extern std::wofstream filelog;

void AttachFunctions();
void DetachFunctions();
UINT WINAPI HookGetACP(void);
UINT WINAPI HookGetOEMCP(void);
BOOL WINAPI HookGetCPInfo(
	UINT       CodePage,
	LPCPINFO  lpCPInfo
);

static LCID WINAPI HookGetLocaleID(void)
{
	return settings.LCID;
}

static LCID WINAPI HookGetThreadLocale(void) { return HookGetLocaleID(); }
static LANGID WINAPI HookGetSystemDefaultUILanguage(void) { return (LANGID)HookGetLocaleID(); }
static LANGID WINAPI HookGetUserDefaultUILanguage(void) { return (LANGID)HookGetLocaleID(); }
static LCID WINAPI HookGetSystemDefaultLCID(void) { return HookGetLocaleID(); }
static LCID WINAPI HookGetUserDefaultLCID(void) { return HookGetLocaleID(); }
static LANGID WINAPI HookGetSystemDefaultLangID(void) { return (LANGID)HookGetLocaleID(); }
static LANGID WINAPI HookGetUserDefaultLangID(void) { return (LANGID)HookGetLocaleID(); }

HWND WINAPI HookCreateWindowExA(
	DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
	int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

LRESULT WINAPI HookSendMessageA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//LRESULT WINAPI HookCallWindowProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI HookMultiByteToWideChar(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
int WINAPI HookWideCharToMultiByte(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
UINT WINAPI HookWinExec(
	_In_ LPSTR lpCmdLine,
	_In_ UINT uCmdShow);
BOOL WINAPI HookCreateProcessA(
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
);

BOOL WINAPI HookCreateProcessW(
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
);

HINSTANCE WINAPI HookShellExecuteA(
	_In_opt_ HWND hwnd,
	_In_opt_ LPSTR lpOperation,
	_In_ LPSTR lpFile,
	_In_opt_ LPSTR lpParameters,
	_In_opt_ LPSTR lpDirectory,
	_In_ INT nShowCmd);

HINSTANCE WINAPI HookShellExecuteW(
	_In_opt_ HWND hwnd,
	_In_opt_ LPWSTR lpOperation,
	_In_ LPWSTR lpFile,
	_In_opt_ LPWSTR lpParameters,
	_In_opt_ LPWSTR lpDirectory,
	_In_ INT nShowCmd
);

BOOL WINAPI HookShellExecuteExA(
_Inout_ SHELLEXECUTEINFOA* pExecInfo
);

BOOL WINAPI HookShellExecuteExW(
_Inout_ SHELLEXECUTEINFOW* pExecInfo
);

int WINAPI HookMessageBoxA(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType
);

BOOL WINAPI HookSetWindowTextA(
	_In_ HWND hWnd,
	_In_opt_ LPCSTR lpString
);

int WINAPI HookGetWindowTextA(
	_In_ HWND hWnd,
	_Out_writes_(nMaxCount) LPSTR lpString,
	_In_ int nMaxCount);

LONG WINAPI HookImmGetCompositionStringA(
	HIMC hIMC,
	DWORD dwIndex,
	LPSTR lpBuf,
	DWORD  dwBufLen
);

LONG WINAPI HookImmGetCompositionStringA_WM(
	HIMC hIMC,
	DWORD dwIndex,
	LPSTR lpBuf,
	DWORD  dwBufLen
);

DWORD WINAPI HookImmGetCandidateListA(
	HIMC            hIMC,
	DWORD           deIndex,
	LPCANDIDATELIST lpCandList,
	DWORD           dwBufLen
);

DWORD WINAPI HookImmGetCandidateListA_WM(
	HIMC            hIMC,
	DWORD           deIndex,
	LPCANDIDATELIST lpCandList,
	DWORD           dwBufLen
);

HFONT WINAPI HookCreateFontA(
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
);

HFONT WINAPI HookCreateFontW(
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
);

HFONT WINAPI HookCreateFontIndirectA(
	LOGFONTA* lplf
);

HFONT WINAPI HookCreateFontIndirectW(
	LOGFONTW* lplf
);

HFONT WINAPI HookCreateFontIndirectExA(
	ENUMLOGFONTEXDVA*
);

HFONT WINAPI HookCreateFontIndirectExW(
	ENUMLOGFONTEXDVW*
);

BOOL WINAPI HookTextOutA(
	HDC    hdc,
	int    x,
	int    y,
	LPSTR lpString,
	int    c
);

int WINAPI HookDrawTextExA(
	_In_ HDC hdc,
	LPSTR lpchText,
	_In_ int cchText,
	_Inout_ LPRECT lprc,
	_In_ UINT format,
	_In_opt_ LPDRAWTEXTPARAMS lpdtp
);

HANDLE WINAPI HookGetClipboardData(
	UINT uFormat
);

HANDLE WINAPI HookSetClipboardData(
	UINT uFormat,
	HANDLE hMem
);

HRESULT WINAPI HookDirectSoundEnumerateA(
	_In_ LPDSENUMCALLBACKA pDSEnumCallback,
	_In_opt_ LPVOID pContext
);

LPSTR WINAPI HookCharPrevExA(
	_In_ WORD CodePage,
	_In_ LPCSTR lpStart,
	_In_ LPCSTR lpCurrentChar,
	_In_ DWORD dwFlags
);

LPSTR WINAPI HookCharNextExA(
	_In_ WORD CodePage,
	_In_ LPCSTR lpCurrentChar,
	_In_ DWORD dwFlags
);

BOOL WINAPI HookIsDBCSLeadByteEx(
	_In_ UINT  CodePage,
	_In_ BYTE  TestChar
);

INT_PTR WINAPI HookDialogBoxParamA(
	_In_opt_ HINSTANCE hInstance,
	_In_ LPCSTR lpTemplateName,
	_In_opt_ HWND hWndParent,
	_In_opt_ DLGPROC lpDialogFunc,
	_In_ LPARAM dwInitParam
);

HWND WINAPI HookCreateDialogIndirectParamA(
	_In_opt_ HINSTANCE hInstance,
	_In_ LPCDLGTEMPLATEA lpTemplate,
	_In_opt_ HWND hWndParent,
	_In_opt_ DLGPROC lpDialogFunc,
	_In_ LPARAM dwInitParam
);

BOOL WINAPI HookVerQueryValueA(
	LPCVOID pBlock,
	LPCSTR lpSubBlock,
	LPVOID* lplpBuffer,
	PUINT puLen
);

DWORD WINAPI HookGetModuleFileNameA(
	HMODULE hModule,
	LPSTR lpFilename,
	DWORD nSize
);

HMODULE WINAPI HookLoadLibraryExA(
	_In_ LPCSTR lpLibFileName,
	_Reserved_ HANDLE hFile,
	_In_ DWORD dwFlags
);

DWORD WINAPI HookGetFileVersionInfoSizeA(
	_In_ LPCSTR lpwstrFilename, 
	_Out_ LPDWORD lpdwHandle
);

BOOL WINAPI HookGetFileVersionInfoA(
	_In_                LPCSTR lptstrFilename, /* Filename of version stamped file */
	_Reserved_          DWORD dwHandle,          /* Information from GetFileVersionSize */
	_In_                DWORD dwLen,             /* Length of buffer for info */
	_Out_writes_bytes_(dwLen) LPVOID lpData            /* Buffer to place the data structure */
);

BOOL WINAPI HookPathRenameExtensionA(
	LPSTR pszPath, 
	LPCSTR pszExt
);

ATOM WINAPI HookRegisterClassA(
	_In_ CONST WNDCLASSA* lpWndClass
);

ATOM WINAPI HookRegisterClassExA(
	_In_ CONST WNDCLASSEXA* lpWndClass
);

LRESULT CALLBACK HookDefWindowProcA(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);

DWORD WINAPI HookGetTimeZoneInformation(
	_Out_ LPTIME_ZONE_INFORMATION lpTimeZoneInformation
);

BOOL WINAPI HookCreateDirectoryA(
	_In_ LPCSTR lpPathName,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes
);

HANDLE WINAPI HookCreateFileA(
	_In_ LPCSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
);

int WINAPI HookGetLocaleInfoA(
	_In_ LCID Locale,
	_In_ LCTYPE LCType,
	_Out_writes_opt_(cchData) LPSTR lpLCData,
	_In_ int cchData
);

int WINAPI HookGetLocaleInfoW(
	_In_ LCID Locale,
	_In_ LCTYPE LCType,
	_Out_writes_opt_(cchData) LPWSTR lpLCData,
	_In_ int cchData
);

//Minhook version Code
/*inline LONG AttachDllFunc(LPCSTR lpszFuncName, LPVOID lpHookAddress, LPCSTR DllName)
{
	HMODULE hDLL = LoadLibraryA(DllName);
	LPVOID funcptr = hDLL ? (LPVOID)(DWORD_PTR)GetProcAddress(hDLL, lpszFuncName) : (LPVOID)lpszFuncName;
	LPVOID outputptr;
	return DetourAttach(&funcptr, lpHookAddress);
	// return the original funcaddress !
}

inline LONG DetachDllFunc(LPCSTR lpszFuncName, LPVOID lpHookAddress, LPCSTR DllName)
{
	HMODULE hDLL = LoadLibraryA(DllName);
	LPVOID funcptr = hDLL ? (LPVOID)(DWORD_PTR)GetProcAddress(hDLL, lpszFuncName) : (LPVOID)lpszFuncName;
	LPVOID outputptr;
	return DetourDetach(&funcptr, lpHookAddress);
	// return the original funcaddress !
}*/

inline LPVOID AllocateZeroedMemory(SIZE_T size/*eax*/) {
	return HeapAlloc(Original.hHeap, HEAP_ZERO_MEMORY, size);
}

inline VOID FreeStringInternal(LPVOID pBuffer/*ecx*/)
{
	HeapFree(Original.hHeap, 0, pBuffer);
}

inline LPWSTR MultiByteToWideCharInternal(LPCSTR lstr, UINT CodePage = CP_ACP)
{
	int lsize = lstrlenA(lstr)/* size without '\0' */, n = 0;
	int wsize = (lsize + 1) << 1;
	LPWSTR wstr = (LPWSTR)HeapAlloc(Original.hHeap, 0, wsize);
	if (wstr) {
		if (CodePage)
			n = OriginalMultiByteToWideChar(CodePage, 0, lstr, lsize, wstr, wsize);
		else
			n = MultiByteToWideChar(CodePage, 0, lstr, lsize, wstr, wsize);
		wstr[n] = L'\0'; // make tail ! 
	}
	return wstr;
}

inline LPSTR WideCharToMultiByteInternal(LPCWSTR wstr, UINT CodePage = CP_ACP)
{
	int wsize = lstrlenW(wstr)/* size without '\0' */, n = 0;
	int lsize = (wsize + 1) << 1;
	LPSTR lstr = (LPSTR)HeapAlloc(Original.hHeap, 0, lsize);
	if (lstr) {
		if (CodePage)
			n = OriginalWideCharToMultiByte(CodePage, 0, wstr, wsize, lstr, lsize, NULL, NULL);
		else
			n = WideCharToMultiByte(CodePage, 0, wstr, wsize, lstr, lsize, NULL, NULL);
		lstr[n] = '\0'; // make tail ! 
	}
	return lstr;
}
