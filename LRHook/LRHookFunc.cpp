#include<Windows.h>
#include<detours.h>

#include"LROriginalFunc.h"
#include"LRHookFunc.h"
#include"User32Hook.h"
#include"KernelbaseHook.h"

ORIGINAL Original = { NULL };

//OriginalNtUserCreateWindowEx = AttachDllFunc("NtUserCreateWindowEx", HookNtUserCreateWindowEx, "user32.dll");

static CreateProcessInternalWFn OriginalCreateProcessInternalW = (CreateProcessInternalWFn)DetourFindFunction("kernelbase.dll", "CreateProcessInternalW");

BOOL WINAPI LRCreateProcessInternalW(
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
)
{
	return OriginalCreateProcessInternalW(
		NULL,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation,
		NULL
	);
}

BOOL WINAPI HookCreateProcessInternalW(
	HANDLE hUserToken,
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation,
	OPTIONAL PHANDLE hRestrictedUserToken
)
{
	if (!OriginalCreateProcessInternalW(
		hUserToken,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags | CREATE_SUSPENDED,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation,
		hRestrictedUserToken))
	{
		return FALSE;
	}
	//MessageBoxW(NULL, lpApplicationName, L"HookCreateProcessInternalW", NULL);
	LPCSTR sz = Original.DllPath;
	DetourUpdateProcessWithDll(lpProcessInformation->hProcess, &sz, 1);
	DetourProcessViaHelperW(lpProcessInformation->dwProcessId, Original.DllPath, LRCreateProcessInternalW);
	if (!(dwCreationFlags & CREATE_SUSPENDED)) {
		ResumeThread(lpProcessInformation->hThread);
	}

	return TRUE;
}

LPVOID AllocateZeroedMemory(SIZE_T size/*eax*/) {
	return HeapAlloc(Original.hHeap, HEAP_ZERO_MEMORY, size);
}

VOID FreeStringInternal(LPVOID pBuffer/*ecx*/)
{
	HeapFree(Original.hHeap, 0, pBuffer);
}

LPWSTR MultiByteToWideCharInternal(LPCSTR lstr, UINT CodePage)
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

LPSTR WideCharToMultiByteInternal(LPCWSTR wstr, UINT CodePage)
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


void AttachFunctions() 
{
	DetourAttach(&(PVOID&)OriginalGetACP, HookGetACP);
	DetourAttach(&(PVOID&)OriginalGetOEMCP, HookGetOEMCP);
	DetourAttach(&(PVOID&)OriginalGetCPInfo, HookGetCPInfo);
	DetourAttach(&(PVOID&)OriginalGetThreadLocale, HookGetThreadLocale);
	DetourAttach(&(PVOID&)OriginalGetSystemDefaultUILanguage, HookGetSystemDefaultUILanguage);
	DetourAttach(&(PVOID&)OriginalGetUserDefaultUILanguage, HookGetUserDefaultUILanguage);
	DetourAttach(&(PVOID&)OriginalGetSystemDefaultLCID, HookGetSystemDefaultLCID);
	DetourAttach(&(PVOID&)OriginalGetUserDefaultLCID, HookGetUserDefaultLCID);
	DetourAttach(&(PVOID&)OriginalGetSystemDefaultLangID, HookGetSystemDefaultLangID);
	DetourAttach(&(PVOID&)OriginalGetUserDefaultLangID, HookGetUserDefaultLangID);
	DetourAttach(&(PVOID&)OriginalMultiByteToWideChar, HookMultiByteToWideChar);
	DetourAttach(&(PVOID&)OriginalWideCharToMultiByte, HookWideCharToMultiByte);

	DetourAttach(&(PVOID&)OriginalCreateWindowExA, HookCreateWindowExA);
	//DetourAttach(&(PVOID&)OriginalDefWindowProcA, HookDefWindowProcA);
	DetourAttach(&(PVOID&)OriginalMessageBoxA, HookMessageBoxA);

	DetourAttach(&(PVOID&)OriginalCharPrevExA, HookCharPrevExA);
	DetourAttach(&(PVOID&)OriginalCharNextExA, HookCharNextExA);
	DetourAttach(&(PVOID&)OriginalIsDBCSLeadByteEx, HookIsDBCSLeadByteEx);
	DetourAttach(&(PVOID&)OriginalSendMessageA, HookSendMessageA);
	
	//DetourAttach(&(PVOID&)OriginalNtCreateUserProcess, HookNtCreateUserProcess);
#ifdef _WIN64
	DetourAttach(&(PVOID&)OriginalCreateProcessA, HookCreateProcessA);
	DetourAttach(&(PVOID&)OriginalCreateProcessW, HookCreateProcessW);
#else
	DetourAttach(&(PVOID&)OriginalCreateProcessInternalW, HookCreateProcessInternalW);
#endif
	
	DetourAttach(&(PVOID&)OriginalWinExec, HookWinExec);
	//DetourAttach(&(PVOID&)OriginalShellExecuteA, HookShellExecuteA);
	//DetourAttach(&(PVOID&)OriginalShellExecuteW, HookShellExecuteW);
	//DetourAttach(&(PVOID&)OriginalShellExecuteExA, HookShellExecuteExA);
	//DetourAttach(&(PVOID&)OriginalShellExecuteExW, HookShellExecuteExW);
	
	
	//DetourAttach(&(PVOID&)OriginalSetWindowTextA, HookSetWindowTextA);
	//DetourAttach(&(PVOID&)OriginalGetWindowTextA, HookGetWindowTextA);
	DetourAttach(&(PVOID&)OriginalDirectSoundEnumerateA, HookDirectSoundEnumerateA);
	DetourAttach(&(PVOID&)OriginalCreateFontA, HookCreateFontA);
	DetourAttach(&(PVOID&)OriginalCreateFontW, HookCreateFontW);
	DetourAttach(&(PVOID&)OriginalCreateFontIndirectA, HookCreateFontIndirectA);
	DetourAttach(&(PVOID&)OriginalCreateFontIndirectW, HookCreateFontIndirectW);
	DetourAttach(&(PVOID&)OriginalCreateFontIndirectExA, HookCreateFontIndirectExA);
	DetourAttach(&(PVOID&)OriginalCreateFontIndirectExW, HookCreateFontIndirectExW);
	//DetourAttach(&(PVOID&)OriginalTextOutA, HookTextOutA);
	DetourAttach(&(PVOID&)OriginalDrawTextExA, HookDrawTextExA);
	DetourAttach(&(PVOID&)OriginalGetClipboardData, HookGetClipboardData);
	DetourAttach(&(PVOID&)OriginalSetClipboardData, HookSetClipboardData);

	DetourAttach(&(PVOID&)OriginalDialogBoxParamA, HookDialogBoxParamA);
	DetourAttach(&(PVOID&)OriginalCreateDialogIndirectParamA, HookCreateDialogIndirectParamA);
	//DetourAttach(&(PVOID&)OriginalVerQueryValueA, HookVerQueryValueA);
	/*DetourAttach(&(PVOID&)OriginalGetModuleFileNameA, HookGetModuleFileNameA);
	DetourAttach(&(PVOID&)OriginalLoadLibraryExA, HookLoadLibraryExA);
	DetourAttach(&(PVOID&)OriginalGetFileVersionInfoSizeA, HookGetFileVersionInfoSizeA);
	DetourAttach(&(PVOID&)OriginalGetFileVersionInfoA, HookGetFileVersionInfoA);
	DetourAttach(&(PVOID&)OriginalPathRenameExtensionA, HookPathRenameExtensionA);*/

	DetourAttach(&(PVOID&)OriginalGetTimeZoneInformation, HookGetTimeZoneInformation);
	DetourAttach(&(PVOID&)OriginalCreateDirectoryA, HookCreateDirectoryA);
	DetourAttach(&(PVOID&)OriginalCreateFileA, HookCreateFileA);
	
	DetourAttach(&(PVOID&)OriginalGetLocaleInfoA, HookGetLocaleInfoA);
	DetourAttach(&(PVOID&)OriginalGetLocaleInfoW, HookGetLocaleInfoW);

	if (settings.HookLCID)
	{
		/*DetourAttach(&(PVOID&)OriginalRegisterClassA, HookRegisterClassA);
		DetourAttach(&(PVOID&)OriginalRegisterClassExA, HookRegisterClassExA);*/
	}
	
	Original.CodePage = OriginalGetACP();
	if (settings.HookIME)
	{
		if (Original.CodePage == 936 && (settings.CodePage == 932 || settings.CodePage == 950))
		{
			DetourAttach(&(PVOID&)OriginalImmGetCompositionStringA, HookImmGetCompositionStringA);
			DetourAttach(&(PVOID&)OriginalImmGetCandidateListA, HookImmGetCandidateListA);
		} 
		else
		{
			DetourAttach(&(PVOID&)OriginalImmGetCompositionStringA, HookImmGetCompositionStringA_WM);
			//DetourAttach(&(PVOID&)OriginalImmGetCandidateListA, HookImmGetCandidateListA_WM);
		}
	}
}

void DetachFunctions() 
{
	DetourDetach(&(PVOID&)OriginalGetACP, HookGetACP);
	DetourDetach(&(PVOID&)OriginalGetOEMCP, HookGetOEMCP);
	DetourDetach(&(PVOID&)OriginalGetCPInfo, HookGetCPInfo);
	DetourDetach(&(PVOID&)OriginalGetThreadLocale, HookGetThreadLocale);
	DetourDetach(&(PVOID&)OriginalGetSystemDefaultUILanguage, HookGetSystemDefaultUILanguage);
	DetourDetach(&(PVOID&)OriginalGetUserDefaultUILanguage, HookGetUserDefaultUILanguage);
	DetourDetach(&(PVOID&)OriginalGetSystemDefaultLCID, HookGetSystemDefaultLCID);
	DetourDetach(&(PVOID&)OriginalGetUserDefaultLCID, HookGetUserDefaultLCID);
	DetourDetach(&(PVOID&)OriginalGetSystemDefaultLangID, HookGetSystemDefaultLangID);
	DetourDetach(&(PVOID&)OriginalGetUserDefaultLangID, HookGetUserDefaultLangID);
	DetourDetach(&(PVOID&)OriginalMultiByteToWideChar, HookMultiByteToWideChar);
	DetourDetach(&(PVOID&)OriginalWideCharToMultiByte, HookWideCharToMultiByte);

	DetourDetach(&(PVOID&)OriginalCreateWindowExA, HookCreateWindowExA);
	DetourDetach(&(PVOID&)OriginalDefWindowProcA, HookDefWindowProcA);
	DetourDetach(&(PVOID&)OriginalMessageBoxA, HookMessageBoxA);
	DetourDetach(&(PVOID&)OriginalCharPrevExA, HookCharPrevExA);
	DetourDetach(&(PVOID&)OriginalCharNextExA, HookCharNextExA);
	DetourDetach(&(PVOID&)OriginalIsDBCSLeadByteEx, HookIsDBCSLeadByteEx);
	DetourDetach(&(PVOID&)OriginalSendMessageA, HookSendMessageA);
	
	DetourDetach(&(PVOID&)OriginalWinExec, HookWinExec);
	DetourDetach(&(PVOID&)OriginalCreateProcessA, HookCreateProcessA);
	DetourDetach(&(PVOID&)OriginalCreateProcessW, HookCreateProcessW);
	//DetourDetach(&(PVOID&)OriginalShellExecuteA, HookShellExecuteA);
	//DetourDetach(&(PVOID&)OriginalShellExecuteW, HookShellExecuteW);
	//DetourDetach(&(PVOID&)OriginalShellExecuteExA, HookShellExecuteExA);
	//DetourDetach(&(PVOID&)OriginalShellExecuteExW, HookShellExecuteExW);

	DetourDetach(&(PVOID&)OriginalSetWindowTextA, HookSetWindowTextA);
	//DetourDetach(&(PVOID&)OriginalGetWindowTextA, HookGetWindowTextA);
	DetourDetach(&(PVOID&)OriginalDirectSoundEnumerateA, HookDirectSoundEnumerateA);
	DetourDetach(&(PVOID&)OriginalCreateFontA, HookCreateFontA);
	DetourDetach(&(PVOID&)OriginalCreateFontW, HookCreateFontW);
	DetourDetach(&(PVOID&)OriginalCreateFontIndirectA, HookCreateFontIndirectA);
	DetourDetach(&(PVOID&)OriginalCreateFontIndirectW, HookCreateFontIndirectW);
	DetourDetach(&(PVOID&)OriginalCreateFontIndirectExA, HookCreateFontIndirectExA);
	DetourDetach(&(PVOID&)OriginalCreateFontIndirectExW, HookCreateFontIndirectExW);
	DetourDetach(&(PVOID&)OriginalTextOutA, HookTextOutA);
	DetourDetach(&(PVOID&)OriginalDrawTextExA, HookDrawTextExA);
	DetourDetach(&(PVOID&)OriginalGetClipboardData, HookGetClipboardData);
	DetourDetach(&(PVOID&)OriginalSetClipboardData, HookSetClipboardData);

	DetourDetach(&(PVOID&)OriginalDialogBoxParamA, HookDialogBoxParamA);
	DetourDetach(&(PVOID&)OriginalCreateDialogIndirectParamA, HookCreateDialogIndirectParamA);
	DetourDetach(&(PVOID&)OriginalVerQueryValueA, HookVerQueryValueA);

	DetourDetach(&(PVOID&)OriginalGetTimeZoneInformation, HookGetTimeZoneInformation);
	DetourDetach(&(PVOID&)OriginalCreateDirectoryA, HookCreateDirectoryA);
	DetourDetach(&(PVOID&)OriginalCreateFileA, HookCreateFileA);

	DetourDetach(&(PVOID&)OriginalGetLocaleInfoA, HookGetLocaleInfoA);
	DetourDetach(&(PVOID&)OriginalGetLocaleInfoW, HookGetLocaleInfoW);

	if (settings.HookLCID)
	{
		DetourDetach(&(PVOID&)OriginalRegisterClassA, HookRegisterClassA);
		DetourDetach(&(PVOID&)OriginalRegisterClassExA, HookRegisterClassExA);
	}

	if (settings.HookIME)
	{
		if (Original.CodePage == 936 && (settings.CodePage == 932 || settings.CodePage == 950))
		{
			DetourDetach(&(PVOID&)OriginalImmGetCompositionStringA, HookImmGetCompositionStringA);
			DetourDetach(&(PVOID&)OriginalImmGetCandidateListA, HookImmGetCandidateListA);
		}
		else
		{
			DetourDetach(&(PVOID&)OriginalImmGetCompositionStringA, HookImmGetCompositionStringA_WM);
			DetourDetach(&(PVOID&)OriginalImmGetCandidateListA, HookImmGetCandidateListA_WM);
		}
	}
}

HWND WINAPI HookCreateWindowExA(
	DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
	int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	LPCWSTR wstrlpClassName = lpClassName ? MultiByteToWideCharInternal(lpClassName) : NULL;
	LPCWSTR wstrlpWindowName = lpWindowName ? MultiByteToWideCharInternal(lpWindowName) : NULL;
	HWND ret = CreateWindowExW(
		dwExStyle,
		wstrlpClassName,
		wstrlpWindowName,
		dwStyle,
		X,
		Y,
		nWidth,
		nHeight,
		hWndParent,
		hMenu,
		hInstance,
		lpParam
	);
	if (wstrlpClassName)
	{
		FreeStringInternal((LPVOID)wstrlpClassName);
	}
	if (wstrlpWindowName)
	{
		FreeStringInternal((LPVOID)wstrlpWindowName);
	}
	return ret;
}

int WINAPI HookMessageBoxA(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType)
{
	LPCWSTR wlpText = MultiByteToWideCharInternal(lpText);
	LPCWSTR wlpCaption = MultiByteToWideCharInternal(lpCaption);
	int ret = MessageBoxW(hWnd, wlpText, wlpCaption, uType);
	if (wlpText)
	{
		FreeStringInternal((LPVOID)wlpText);
	}
	if (wlpCaption)
	{
		FreeStringInternal((LPVOID)wlpCaption);
	}
	return ret;
}

UINT WINAPI HookGetACP(void)
{
	return settings.CodePage;
}
UINT WINAPI HookGetOEMCP(void)
{
	return settings.CodePage;
}

BOOL WINAPI HookGetCPInfo(
	UINT       CodePage,
	LPCPINFO  lpCPInfo
)
{
	CodePage = settings.CodePage;
	return OriginalGetCPInfo(CodePage, lpCPInfo);
}

static int CheckWindowStyle(HWND hWnd, DWORD type/*ebx*/) {

	LONG_PTR n = GetWindowLongPtrW(hWnd, GWL_STYLE);
	// window no needs conversion ?? 
	if (n == 0) {
		return (0);
	}
	else if (n == /*0x84C820E4*/(WS_POPUP | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_SYSMENU |
		WS_EX_RTLREADING | WS_EX_TOOLWINDOW | WS_EX_MDICHILD | WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY)) {
		return (0);
	}
	else if (!(n & (WS_EX_ACCEPTFILES | WS_EX_TRANSPARENT))) {
		return (0);
	}
	else if (!type && (n & WS_EX_CLIENTEDGE)) {
		return (0);
	}
	else if (n & WS_EX_MDICHILD) {
		return (0);
	}
	// other case : 
	return (-1); // xor ebx, ebx !
}

inline LRESULT CallWindowSendMessage(LPVOID lpProcAddress, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	DWORD_PTR Param1/*ecx*/, DWORD_PTR Param2/*ecx*/, DWORD_PTR Param3/*ecx*/, int FunctionType)
{
	switch (FunctionType) {
	case 0:
	default:
		return ((LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM))(DWORD_PTR)lpProcAddress)
			(hWnd, uMsg, wParam, lParam);
	case 1:
		return ((LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM, DWORD_PTR, DWORD_PTR))(DWORD_PTR)lpProcAddress)
			(hWnd, uMsg, wParam, lParam, Param1, Param2);
	case 2:
		return ((LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM, DWORD_PTR, DWORD_PTR, DWORD_PTR))(DWORD_PTR)lpProcAddress)
			(hWnd, uMsg, wParam, lParam, Param1, Param2, Param3);
	case 3:
		return ((LRESULT(WINAPI*)(DWORD_PTR, HWND, UINT, WPARAM, LPARAM))(DWORD_PTR)lpProcAddress)
			(Param1, hWnd, uMsg, wParam, lParam);
	}
}

static LRESULT SendUnicodeMessage(LPVOID lpProcAddress, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	DWORD_PTR Param1/*ecx*/, DWORD_PTR Param2/*ecx*/, DWORD_PTR Param3/*ecx*/, int FunctionType)
{
	LPCWSTR lpUnicodeWindowName = NULL, lpUnicodeClassName = NULL;
	WCHAR CharBuffer[2];
	int type = 0;

	//char classname[256]; GetClassNameA(hWnd, classname, sizeof(classname));
	//if (lstrcmpiA(classname, "TListBox") == 0) {
	//ntprintfA(256, 1, "%s: proc-%p hwnd=%p, msg=%04x, wParam=%d, lParam=%d\n", __FUNCTION__, lpProcAddress, hWnd, uMsg, wParam, lParam);
	//}
	switch (uMsg) {
	case WM_SETTEXT:
	case WM_SETTINGCHANGE:
	case EM_REPLACESEL:
	case WM_DEVMODECHANGE:
	case CB_DIR:
	case LB_DIR:
	case LB_ADDFILE:
	case CB_ADDSTRING:
	case CB_INSERTSTRING:
	case CB_FINDSTRING:
	case CB_SELECTSTRING:
	case CB_FINDSTRINGEXACT:
	case LB_ADDSTRING:
	case LB_INSERTSTRING:
	case LB_SELECTSTRING:
	case LB_FINDSTRING:
	case LB_FINDSTRINGEXACT:
	case 0x01AA:
	case 0x01AB:
	case 0x01AC:
	{
		return ANSI_INSTRINGNULL(hWnd, uMsg, wParam, lParam);
	}	break;
	case WM_IME_CHAR: // LN309
	case WM_CHAR: // LN309
	{
		if ((wchar_t)wParam > 0x7F) { // is multibyte ... 
			// here we exchange the order : 
			//	char t = *((char*)&wParam + 0);
			//	*((char*)&wParam + 0) = *((char*)&wParam + 1);
			//	*((char*)&wParam + 1) = t;
			wParam = ((wParam & 0xFF) << 8) | ((wParam & 0xFF00) >> 8);
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)&wParam, -1, CharBuffer, 2);
			//	*((wchar_t*)&wParam) = CharBuffer[0];
			wParam = CharBuffer[0];
		}
	}	break;
	case WM_GETTEXTLENGTH: // LN327
	{
		LRESULT len = CallWindowSendMessage(lpProcAddress, hWnd, WM_GETTEXTLENGTH, 0, 0, Param1, Param2, Param3, FunctionType);
		if (len > 0) {
			LPWSTR lParamW = (LPWSTR)AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
			CallWindowSendMessage(lpProcAddress, hWnd, WM_GETTEXT, (len + 1) * sizeof(wchar_t), (LPARAM)lParamW,
				Param1, Param2, Param3, FunctionType);
			len = WideCharToMultiByte(CP_ACP, 0, lParamW, -1, NULL, 0, NULL, NULL) - 1; // required
			// LN793
			if (lParamW) FreeStringInternal(lParamW);
		}
		return len;
	}	break;
	case WM_GETTEXT: // LN310
	{
		if (IsBadWritePtr((LPVOID)lParam, 1)) {
			return (0);
		}
		else {
			// L311
			int len = (int)CallWindowSendMessage(lpProcAddress, hWnd, WM_GETTEXTLENGTH, 0, 0, Param1, Param2, Param3, FunctionType);
			// no needs check len == 0 ?? 
			LPWSTR lParamW = (LPWSTR)AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
			len = (int)CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, (LPARAM)lParamW, Param1, Param2, Param3, FunctionType);
			len = WideCharToMultiByte(CP_ACP, 0, lParamW, -1, (LPSTR)lParam, len, NULL, NULL) - 1;
			if (len > 0) {
				// LN793
				if (lParamW) FreeStringInternal(lParamW);
			}
			else {
				// L316
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					*((LPSTR)lParam + wParam - 1) = '\0';
				}
				else {
					// L317
					*((LPSTR)lParam) = '\0';
				}
			}
			return len;
		}
	}	break;
	// ----------- common controls end ---------------
	default: // LN301
		break;
	}
	// --------- 
	return CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, lParam, Param1, Param2, Param3, FunctionType);
}

LRESULT WINAPI HookSendMessageA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return SendUnicodeMessage((LPVOID)(DWORD_PTR)SendMessageW, hWnd, uMsg, wParam, lParam, 0, 0, 0, 0);
}

int WINAPI HookMultiByteToWideChar(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
	CodePage = (CodePage >= CP_UTF7) ? CodePage : settings.CodePage;
	return OriginalMultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}

int WINAPI HookWideCharToMultiByte(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
	CodePage = (CodePage >= CP_UTF7) ? CodePage : settings.CodePage;
	return OriginalWideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
}

UINT WINAPI HookWinExec(
	_In_ LPSTR lpCmdLine,
	_In_ UINT uCmdShow
)
{
	//LRConfigFileMap filemap;
	//filemap.WrtieConfigFileMap(&settings);

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFOA));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFOA);
	
	BOOL ret = CreateProcessA(NULL, lpCmdLine, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi);

	//filemap.FreeConfigFileMap();
	if (ret == TRUE)
		return 0x21;
	else return 0;
}

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
)
{
	//MessageBoxA(NULL, lpCommandLine, "HookCreateProcessA", NULL);
	return DetourCreateProcessWithDllExA(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation, 
		Original.DllPath,
		OriginalCreateProcessA);
}

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
)
{
	//MessageBoxW(NULL, lpCommandLine, TEXT("HookCreateProcessW"), NULL);
	if (lpCommandLine && wcsstr(lpCommandLine, L"BlackXchg.aes") != NULL)
	{
		return OriginalCreateProcessW(
			lpApplicationName,
			lpCommandLine,
			lpProcessAttributes,
			lpThreadAttributes,
			bInheritHandles,
			dwCreationFlags,
			lpEnvironment,
			lpCurrentDirectory,
			lpStartupInfo,
			lpProcessInformation
		);
	}
	return DetourCreateProcessWithDllExW(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation,
		Original.DllPath,
		OriginalCreateProcessW);
}

HINSTANCE WINAPI HookShellExecuteA(
	_In_opt_ HWND hwnd,
	_In_opt_ LPSTR lpOperation,
	_In_ LPSTR lpFile,
	_In_opt_ LPSTR lpParameters,
	_In_opt_ LPSTR lpDirectory,
	_In_ INT nShowCmd
)
{
	//MessageBoxA(NULL, lpFile, "ShellExecuteA", NULL);

	//LRConfigFileMap filemap;
	//filemap.WrtieConfigFileMap(&settings);

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFOA));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFOA);

	bool ret = DetourCreateProcessWithDllExA(lpFile, lpParameters, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, lpDirectory,
		&si, &pi, Original.DllPath, OriginalCreateProcessA);

	//filemap.FreeConfigFileMap();

	return (HINSTANCE)pi.hProcess;
}

HINSTANCE WINAPI HookShellExecuteW(
	_In_opt_ HWND hwnd,
	_In_opt_ LPWSTR lpOperation,
	_In_ LPWSTR lpFile,
	_In_opt_ LPWSTR lpParameters,
	_In_opt_ LPWSTR lpDirectory,
	_In_ INT nShowCmd
)
{
	//MessageBoxW(NULL, TEXT("ShellExecuteW"), TEXT("ShellExecuteW"), NULL);

	//LRConfigFileMap filemap;
	//filemap.WrtieConfigFileMap(&settings);

	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFOW));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFOW);

	bool ret = DetourCreateProcessWithDllExW(lpFile, lpParameters, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, lpDirectory,
		&si, &pi, Original.DllPath, OriginalCreateProcessW);

	//filemap.FreeConfigFileMap();

	return (HINSTANCE)pi.hProcess;
}

BOOL WINAPI HookShellExecuteExA(
	_Inout_ SHELLEXECUTEINFOA* pExecInfo
)
{
	//MessageBoxA(NULL, pExecInfo->lpFile, "ShellExecuteExA", NULL);

	return OriginalShellExecuteExA(pExecInfo);
}

BOOL WINAPI HookShellExecuteExW(
	_Inout_ SHELLEXECUTEINFOW* pExecInfo
)
{
	//MessageBoxW(NULL, L"ShellExecuteExW", L"ShellExecuteExW", NULL);

	return OriginalShellExecuteExW(pExecInfo);
}

BOOL WINAPI HookSetWindowTextA(
	_In_ HWND hWnd,
	_In_opt_ LPCSTR lpString
)
{
	LPCWSTR wstr = lpString ? MultiByteToWideCharInternal(lpString) : NULL;
	//LONG_PTR originalWndProc = GetWindowLongPtrW(hWnd, GWLP_WNDPROC);
	//SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)DefWindowProcW);
	BOOL ret = SetWindowTextW(hWnd, wstr);
	//SetWindowLongPtrW(hWnd, GWLP_WNDPROC, originalWndProc);
	if (wstr) {
		FreeStringInternal((LPVOID)wstr);
	}
	return ret;
}

int WINAPI HookGetWindowTextA(_In_ HWND hWnd, _Out_writes_(nMaxCount) LPSTR lpString, _In_ int nMaxCount)
{
	int wlen = GetWindowTextLengthW(hWnd) + 1;
	LPWSTR lpStringW = (LPWSTR)AllocateZeroedMemory(wlen * sizeof(wchar_t));
	int wsize = GetWindowTextW(hWnd, lpStringW, wlen);
	int lsize = wsize ? WideCharToMultiByte(CP_ACP, 0, lpStringW, wsize, lpString, nMaxCount, NULL, NULL) : 0;
	FreeStringInternal(lpStringW);
	return lsize;
}

LONG WINAPI HookImmGetCompositionStringA(
	HIMC hIMC, 
	DWORD dwIndex,
	LPSTR lpBuf,
	DWORD  dwBufLen
)
{
	LONG ret = OriginalImmGetCompositionStringA(hIMC, dwIndex, lpBuf, dwBufLen);
	if (lpBuf)
	{
		LPWSTR wstr = MultiByteToWideCharInternal(lpBuf, Original.CodePage);
		if (wstr)
		{
			int wsize = lstrlenW(wstr), n = 0;
			int lsize = (wsize + 1) << 1;
			n = OriginalWideCharToMultiByte(settings.CodePage, 0, wstr, wsize, lpBuf, lsize, NULL, NULL);
			dwBufLen = lsize;
			lpBuf[n] = '\0';
		}
		FreeStringInternal((LPVOID)wstr);
	}
	return ret;
}

LONG WINAPI HookImmGetCompositionStringA_WM(
	HIMC hIMC,
	DWORD dwIndex,
	LPSTR lpBuf,
	DWORD  dwBufLen
)
{
	LONG wsize = ImmGetCompositionStringW(hIMC, dwIndex, NULL, 0);
	LPWSTR wstr = (LPWSTR)AllocateZeroedMemory(wsize);
	ImmGetCompositionStringW(hIMC, dwIndex, wstr, wsize);
	LONG lsize = (wsize + 1) << 1;
	if (lpBuf)
	{
		lsize = OriginalWideCharToMultiByte(settings.CodePage, 0, wstr, wsize, lpBuf, lsize, Original.lpDefaultChar, &Original.lpUsedDefaultChar);
		lpBuf[lsize] = '\0'; // make tail ! 
	}
	FreeStringInternal(wstr);
	return lsize;
}

DWORD WINAPI HookImmGetCandidateListA(
	HIMC            hIMC,
	DWORD           deIndex,
	LPCANDIDATELIST lpCandList,
	DWORD           dwBufLen
)
{
	DWORD ret= OriginalImmGetCandidateListA(hIMC, deIndex, lpCandList, dwBufLen);
	if (lpCandList)
	{
		for (int i = 0; i < lpCandList->dwCount; i++)
		{
			LPSTR lstr = (LPSTR)lpCandList + lpCandList->dwOffset[i];
			LPWSTR wstr = MultiByteToWideCharInternal(lstr, Original.CodePage);
			if (wstr)
			{
				int wsize = lstrlenW(wstr), n = 0;
				int lsize = (wsize + 1) << 1;
				n = OriginalWideCharToMultiByte(settings.CodePage, 0, wstr, wsize, lstr, lsize, NULL, NULL);
				dwBufLen = lsize;
				lstr[n] = '\0';
				FreeStringInternal(wstr);
			}
		}
	}
	return ret;
}

DWORD WINAPI HookImmGetCandidateListA_WM(
	HIMC            hIMC,
	DWORD           deIndex,
	LPCANDIDATELIST lpCandList,
	DWORD           dwBufLen
)
{
	DWORD ret = OriginalImmGetCandidateListA(hIMC, deIndex, lpCandList, dwBufLen);
	if (lpCandList)
	{
		DWORD dwBufLenW = ImmGetCandidateListW(hIMC, deIndex, NULL, NULL);
		LPCANDIDATELIST lpCandListW = (LPCANDIDATELIST)AllocateZeroedMemory(dwBufLenW);
		ImmGetCandidateListW(hIMC, deIndex, lpCandListW, dwBufLenW);
		for (int i = 0; i < lpCandList->dwCount; i++)
		{
			LPSTR lstr = (LPSTR)lpCandList + lpCandList->dwOffset[i];
			LPWSTR wstr = (LPWSTR)lpCandListW + lpCandListW->dwOffset[i];
			if (lstr)
			{
				int lsize = lstrlenA(lstr);
				int wsize = wcslen(wstr);
				OriginalWideCharToMultiByte(settings.CodePage, 0, wstr, wsize, lstr, lsize, NULL, NULL);
				//filelog << lstr << "###" << lsize << "###" << wstr << "###" <<wsize << std::endl;
			}
		}
		FreeStringInternal(lpCandListW);
	}
	return ret;
}

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
)
{
	LPWSTR pszFaceNameW = MultiByteToWideCharInternal(pszFaceName);
	HFONT ret = CreateFontW(
		cHeight,
		cWidth,
		cEscapement,
		cOrientation,
		cWeight,
		bItalic,
		bUnderline,
		bStrikeOut,
		iCharSet,
		iOutPrecision,
		iClipPrecision,
		iQuality,
		iPitchAndFamily,
		pszFaceNameW
	);
	FreeStringInternal(pszFaceNameW);
	return ret;
}

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
)
{
	//iCharSet = HANGUL_CHARSET;
	return OriginalCreateFontW(
		cHeight,
		cWidth,
		cEscapement,
		cOrientation,
		cWeight,
		bItalic,
		bUnderline,
		bStrikeOut,
		iCharSet,
		iOutPrecision,
		iClipPrecision,
		iQuality,
		iPitchAndFamily,
		pszFaceName
	);
}

//set font characters set
HFONT WINAPI HookCreateFontIndirectA(
	LOGFONTA* lplf
)
{
	//MessageBoxA(NULL, lplf->lfFaceName, "HookCreateFontIndirectA", NULL);
	//lplf->lfCharSet = CHINESEBIG5_CHARSET;
	/*if (strcmp(settings.lfFaceName, "None") != 0)
		strcpy(lplf->lfFaceName, settings.lfFaceName);
	return OriginalCreateFontIndirectA(lplf);*/
	LOGFONTW logfont = { sizeof(LOGFONTW), };
	memcpy(&logfont, lplf, sizeof(LOGFONTW));
	MultiByteToWideChar(settings.CodePage, 0, lplf->lfFaceName, -1, logfont.lfFaceName, LF_FACESIZE);
	return CreateFontIndirectW(&logfont);
}

HFONT WINAPI HookCreateFontIndirectW(
	LOGFONTW* lplf
)
{
	//lplf->lfCharSet = HANGUL_CHARSET;
	return OriginalCreateFontIndirectW(lplf);
}

HFONT WINAPI HookCreateFontIndirectExA(
	ENUMLOGFONTEXDVA* lplf
)
{
	ENUMLOGFONTEXDVW lplfW = { sizeof(ENUMLOGFONTEXDVW), };
	memcpy(&lplfW, lplf, sizeof(ENUMLOGFONTEXDVW));
	MultiByteToWideChar(settings.CodePage, 0, lplf->elfEnumLogfontEx.elfLogFont.lfFaceName, -1, lplfW.elfEnumLogfontEx.elfLogFont.lfFaceName, LF_FACESIZE);
	return OriginalCreateFontIndirectExW(&lplfW);
}

HFONT WINAPI HookCreateFontIndirectExW(
	ENUMLOGFONTEXDVW* lplf
)
{
	//lplf->elfEnumLogfontEx.elfLogFont.lfCharSet = HANGUL_CHARSET;
	return OriginalCreateFontIndirectExW(lplf);
}

BOOL WINAPI HookTextOutA(
	HDC    hdc,
	int    x,
	int    y,
	LPSTR lpString,
	int    c
)
{
	LPWSTR wstr = MultiByteToWideCharInternal(lpString);
	if (wstr)
	{
		bool ret = TextOutW(hdc, x, y, wstr, 1);
		FreeStringInternal(wstr);
		return ret;
	}
	return OriginalTextOutA(hdc, x, y, lpString, c);
}

int WINAPI HookDrawTextExA(
	_In_ HDC hdc,
	LPSTR lpchText,
	_In_ int cchText,
	_Inout_ LPRECT lprc,
	_In_ UINT format,
	_In_opt_ LPDRAWTEXTPARAMS lpdtp
)
{
	LPWSTR wstr = MultiByteToWideCharInternal(lpchText);
	if (wstr)
	{
		int wsize = lstrlenW(wstr);
		int ret = DrawTextExW(hdc, wstr, wsize, lprc, format, lpdtp);
		FreeStringInternal(wstr);
		return ret;
	}
	return OriginalDrawTextExA(
		hdc,
		lpchText,
		cchText,
		lprc,
		format,
		lpdtp
	);
}

HANDLE WINAPI HookGetClipboardData(
	UINT uFormat
)
{
	if (uFormat == CF_TEXT)
	{
		HANDLE hClipMemory = OriginalGetClipboardData(CF_UNICODETEXT);
		HANDLE hGlobalMemory = NULL;
		LPWSTR wstr = (LPWSTR)GlobalLock(hClipMemory);
		if (wstr)
		{
			int wsize = lstrlenW(wstr);
			int lsize = (wsize + 1) << 1;
			hGlobalMemory = GlobalAlloc(GHND, lsize);
			if (hGlobalMemory)
			{
				LPSTR lstr = (LPSTR)GlobalLock(hGlobalMemory);
				if (lstr)
				{
					lsize = OriginalWideCharToMultiByte(settings.CodePage, 0, wstr, wsize, lstr, lsize, NULL, NULL);
					lstr[lsize] = '\0';
				}
				GlobalUnlock(hGlobalMemory);
			}
		}
		GlobalUnlock(hClipMemory);
		if (hGlobalMemory)
			return hGlobalMemory;
	}
	return OriginalGetClipboardData(uFormat);
}

HANDLE WINAPI HookSetClipboardData(
	UINT uFormat,
	HANDLE hMem
)
{
	if (uFormat == CF_TEXT)
	{
		HANDLE hGlobalMemory = NULL;
		LPSTR lstr = (LPSTR)GlobalLock(hMem);
		if (lstr)
		{
			int lsize = lstrlenA(lstr);
			int wsize = (lsize + 1) << 1;
			hGlobalMemory = GlobalAlloc(GHND, wsize);
			if (hGlobalMemory)
			{
				LPWSTR wstr = (LPWSTR)GlobalLock(hGlobalMemory);
				if (wstr)
				{
					wsize = OriginalMultiByteToWideChar(settings.CodePage, 0, lstr, lsize, wstr, wsize);
					wstr[wsize] = L'\0';
				}
				GlobalUnlock(hGlobalMemory);
			}
		}
		GlobalUnlock(hMem);
		if (hGlobalMemory)
			return OriginalSetClipboardData(CF_UNICODETEXT, hGlobalMemory);
	}
	return OriginalSetClipboardData(uFormat, hMem);
}

HRESULT WINAPI HookDirectSoundEnumerateA(
	_In_ LPDSENUMCALLBACKA pDSEnumCallback,
	_In_opt_ LPVOID pContext
)
{
	return DirectSoundEnumerateW((LPDSENUMCALLBACKW)pDSEnumCallback,pContext);
}

LPSTR WINAPI HookCharPrevExA(
	_In_ WORD CodePage,
	_In_ LPCSTR lpStart,
	_In_ LPCSTR lpCurrentChar,
	_In_ DWORD dwFlags
)
{
	CodePage = (CodePage >= CP_UTF7) ? CodePage : settings.CodePage;
	return OriginalCharPrevExA(CodePage, lpStart, lpCurrentChar, dwFlags);
}

LPSTR WINAPI HookCharNextExA(
	_In_ WORD CodePage,
	_In_ LPCSTR lpCurrentChar,
	_In_ DWORD dwFlags
)
{
	CodePage = (CodePage >= CP_UTF7) ? CodePage : settings.CodePage;
	return OriginalCharNextExA(CodePage, lpCurrentChar, dwFlags);
}

BOOL WINAPI HookIsDBCSLeadByteEx(
	_In_ UINT  CodePage,
	_In_ BYTE  TestChar
)
{
	CodePage = (CodePage >= CP_UTF7) ? CodePage : settings.CodePage;
	return OriginalIsDBCSLeadByteEx(CodePage, TestChar);
}

INT_PTR WINAPI HookDialogBoxParamA(
	_In_opt_ HINSTANCE hInstance,
	_In_ LPCSTR lpTemplateName,
	_In_opt_ HWND hWndParent,
	_In_opt_ DLGPROC lpDialogFunc,
	_In_ LPARAM dwInitParam
)
{
	LPWSTR lpTemplateNameW = MultiByteToWideCharInternal(lpTemplateName);
	return DialogBoxParamW(
		hInstance, 
		lpTemplateNameW ? lpTemplateNameW : (LPCWSTR)lpTemplateName, 
		hWndParent, 
		lpDialogFunc, 
		dwInitParam
	);
}

HWND WINAPI HookCreateDialogIndirectParamA(
	_In_opt_ HINSTANCE hInstance,
	_In_ LPCDLGTEMPLATEA lpTemplate,
	_In_opt_ HWND hWndParent,
	_In_opt_ DLGPROC lpDialogFunc,
	_In_ LPARAM dwInitParam
)
{
	return CreateDialogIndirectParamW(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);
}

BOOL WINAPI HookVerQueryValueA(
	LPCVOID pBlock,
	LPCSTR lpSubBlock,
	LPVOID* lplpBuffer,
	PUINT puLen
)
{
	/*
	if (lstrcmpA(lpSubBlock, "\\VarFileInfo\\Translation") == 0)
	{
		// if query location info, we change that value : 
		BOOL ret = OriginalVerQueryValueA(pBlock, lpSubBlock, lplpBuffer, puLen);
		if (ret && *puLen >= sizeof(DWORD))
		{
			LPWORD lpTranslate = (LPWORD)(*lplpBuffer);
			filelog << lpTranslate[0] << " " << lpTranslate[1] << std::endl;
			lpTranslate[0] = (WORD)1041;
			lpTranslate[1] = (WORD)1200; // change the first default one !
			return ret;
		}
	}*/
	if (lstrlenA(lpSubBlock) > 2 && lpSubBlock[0] == '\\' && lpSubBlock[1] == 'S')
	{
		LPWSTR lpSubBlockW = MultiByteToWideCharInternal(lpSubBlock);
		LPWSTR lpBufferW;
		BOOL ret = VerQueryValueW(pBlock, lpSubBlockW, (LPVOID*)&lpBufferW, puLen);
		LPSTR lpBufferA = WideCharToMultiByteInternal(lpBufferW);
		*lplpBuffer = lpBufferA;
		*puLen = lstrlenA(lpBufferA);
		FreeStringInternal(lpSubBlockW);
		return ret;
	}
	return OriginalVerQueryValueA(pBlock, lpSubBlock, lplpBuffer, puLen);
}

DWORD WINAPI HookGetModuleFileNameA(
	HMODULE hModule,
	LPSTR lpFilename,
	DWORD nSize
)
{
	LPWSTR lpFilenameW = (LPWSTR)AllocateZeroedMemory(MAX_PATH);
	DWORD ret = GetModuleFileNameW(hModule, lpFilenameW, nSize);
	OriginalWideCharToMultiByte(settings.CodePage, 0, lpFilenameW, MAX_PATH, lpFilename, MAX_PATH, NULL, NULL);
	FreeStringInternal(lpFilenameW);
	return ret;
}

HMODULE WINAPI HookLoadLibraryExA(
	_In_ LPCSTR lpLibFileName,
	_Reserved_ HANDLE hFile,
	_In_ DWORD dwFlags
)
{
	LPWSTR lpLibFileNameW = MultiByteToWideCharInternal(lpLibFileName);
	HMODULE ret = LoadLibraryExW(lpLibFileNameW, hFile, dwFlags);
	FreeStringInternal(lpLibFileNameW);
	return ret;
}

DWORD WINAPI HookGetFileVersionInfoSizeA(
	_In_ LPCSTR lpwstrFilename,
	_Out_ LPDWORD lpdwHandle
)
{
	LPWSTR lpwstrFilenameW = MultiByteToWideCharInternal(lpwstrFilename);
	DWORD ret = GetFileVersionInfoSizeW(lpwstrFilenameW, lpdwHandle);
	FreeStringInternal(lpwstrFilenameW);
	return ret;
}

BOOL WINAPI HookGetFileVersionInfoA(
	_In_                LPCSTR lptstrFilename, /* Filename of version stamped file */
	_Reserved_          DWORD dwHandle,          /* Information from GetFileVersionSize */
	_In_                DWORD dwLen,             /* Length of buffer for info */
	_Out_writes_bytes_(dwLen) LPVOID lpData            /* Buffer to place the data structure */
)
{
	LPWSTR lptstrFilenameW = MultiByteToWideCharInternal(lptstrFilename);
	BOOL ret = GetFileVersionInfoW(lptstrFilenameW, dwHandle, dwLen, lpData);
	MessageBoxW(NULL, lptstrFilenameW, NULL, NULL);
	FreeStringInternal(lptstrFilenameW);
	return ret;
}

BOOL WINAPI HookPathRenameExtensionA(
	LPSTR pszPath,
	LPCSTR pszExt
)
{
	//MessageBoxA(NULL, pszPath, NULL, NULL);
	return OriginalPathRenameExtensionA(pszPath, pszExt);
}

ATOM WINAPI HookRegisterClassA(
	_In_ CONST WNDCLASSA* lpWndClass
)
{
	WNDCLASSW* lpWndClassW = new(WNDCLASSW);
	lpWndClassW->style = lpWndClass->style;
	lpWndClassW->lpfnWndProc = lpWndClass->lpfnWndProc;
	lpWndClassW->cbClsExtra = lpWndClass->cbClsExtra;
	lpWndClassW->cbWndExtra = lpWndClass->cbWndExtra;
	lpWndClassW->hInstance = lpWndClass->hInstance;
	lpWndClassW->hIcon = lpWndClass->hIcon;
	lpWndClassW->hCursor = lpWndClass->hCursor;
	lpWndClassW->hbrBackground = lpWndClass->hbrBackground;
	lpWndClassW->lpszMenuName = MultiByteToWideCharInternal(lpWndClass->lpszMenuName);
	lpWndClassW->lpszClassName = MultiByteToWideCharInternal(lpWndClass->lpszClassName);
	return RegisterClassW(lpWndClassW);
}

ATOM WINAPI HookRegisterClassExA(
	_In_ CONST WNDCLASSEXA* lpWndClass
)
{
	WNDCLASSEXW* lpWndClassW = new(WNDCLASSEXW);
	lpWndClassW->cbSize = lpWndClass->cbSize;
	lpWndClassW->style = lpWndClass->style;
	lpWndClassW->lpfnWndProc = lpWndClass->lpfnWndProc;
	lpWndClassW->cbClsExtra = lpWndClass->cbClsExtra;
	lpWndClassW->cbWndExtra = lpWndClass->cbWndExtra;
	lpWndClassW->hInstance = lpWndClass->hInstance;
	lpWndClassW->hIcon = lpWndClass->hIcon;
	lpWndClassW->hCursor = lpWndClass->hCursor;
	lpWndClassW->hbrBackground = lpWndClass->hbrBackground;
	lpWndClassW->lpszMenuName = MultiByteToWideCharInternal(lpWndClass->lpszMenuName);
	lpWndClassW->lpszClassName = MultiByteToWideCharInternal(lpWndClass->lpszClassName);
	lpWndClassW->hIconSm = lpWndClass->hIconSm;
	return RegisterClassExW(lpWndClassW);
}

inline LRESULT CallProcAddress(LPVOID lpProcAddress, HWND hWnd, HWND hMDIClient,
	BOOL bMDIClientEnabled, INT uMsg, WPARAM wParam, LPARAM lParam)
{
	typedef LRESULT(WINAPI* fnWNDProcAddress)(HWND, int, WPARAM, LPARAM);
	typedef LRESULT(WINAPI* fnMDIProcAddress)(HWND, HWND, int, WPARAM, LPARAM);
	// MDI or not ??? 
	return (bMDIClientEnabled) ? ((fnMDIProcAddress)(DWORD_PTR)lpProcAddress)(hWnd, hMDIClient, uMsg, wParam, lParam)
		: ((fnWNDProcAddress)(DWORD_PTR)lpProcAddress)(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK HookDefWindowProcA(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	if(IsWindowUnicode(hWnd))
		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	else
		return OriginalDefWindowProcA(hWnd, Msg, wParam, lParam);
}

DWORD WINAPI HookGetTimeZoneInformation(
	_Out_ LPTIME_ZONE_INFORMATION lpTimeZoneInformation
)
{
	DWORD ret = OriginalGetTimeZoneInformation(lpTimeZoneInformation);
	if (ret != TIME_ZONE_ID_INVALID) {
		// Warning Bias becomes negative!!!
		lpTimeZoneInformation->Bias = -settings.Bias;
	}
	return ret;
}

BOOL WINAPI HookCreateDirectoryA(
	_In_ LPCSTR lpPathName,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	LPWSTR lpPathNameW = MultiByteToWideCharInternal(lpPathName,Original.CodePage);
	BOOL ret = CreateDirectoryW(lpPathNameW, lpSecurityAttributes);
	if (lpPathNameW)
	{
		FreeStringInternal(lpPathNameW);
	}
	return ret;
}

HANDLE WINAPI HookCreateFileA(
	_In_ LPCSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
)
{
	LPWSTR lpFileNameW = MultiByteToWideCharInternal(lpFileName,Original.CodePage);
	HANDLE ret = CreateFileW(
		lpFileNameW,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile
	);
	if (lpFileNameW)
	{
		FreeStringInternal(lpFileNameW);
	}
	return ret;
}

int WINAPI HookGetLocaleInfoA(
	_In_ LCID Locale,
	_In_ LCTYPE LCType,
	_Out_writes_opt_(cchData) LPSTR lpLCData,
	_In_ int cchData
)
{
	return OriginalGetLocaleInfoA(settings.LCID, LCType, lpLCData, cchData);
}

int WINAPI HookGetLocaleInfoW(
	_In_ LCID Locale,
	_In_ LCTYPE LCType,
	_Out_writes_opt_(cchData) LPWSTR lpLCData,
	_In_ int cchData
)
{
	return OriginalGetLocaleInfoW(settings.LCID, LCType,lpLCData,cchData);
}