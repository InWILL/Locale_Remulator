#include"LRHookFunc.h"

void AttachFunctions() {
	//HookDllFunc((LPCSTR)(DWORD_PTR)CreateWindowExA, (LPVOID)(DWORD_PTR)HookCreateWindowExA, NULL);
	//DetourAttach(&(PVOID&)OriginalMessageBoxA, HookMessageBoxA);
	DetourAttach(&(PVOID&)OriginalGetACP, HookGetACP);
	DetourAttach(&(PVOID&)OriginalGetOEMCP, HookGetOEMCP);
	DetourAttach(&(PVOID&)OriginalSendMessageA, HookSendMessageA);
	DetourAttach(&(PVOID&)OriginalMultiByteToWideChar, HookMultiByteToWideChar);
	DetourAttach(&(PVOID&)OriginalWideCharToMultiByte, HookWideCharToMultiByte);
	DetourAttach(&(PVOID&)OriginalWinExec, HookWinExec);
	DetourAttach(&(PVOID&)OriginalCreateProcessA, HookCreateProcessA);
	//DetourAttach(&(PVOID&)OriginalShellExecuteA, HookShellExecuteA);
	
	//DetourAttach(&(PVOID&)OriginalDefWindowProcA, HookDefWindowProcA);
	//DetourAttach(&(PVOID&)OriginalDefDlgProcA, HookDefDlgProcA);
}

void DetachFunctions() {
	//DetourDetach(&(PVOID&)OriginalMessageBoxA, HookMessageBoxA);
	DetourDetach(&(PVOID&)OriginalGetACP, HookGetACP);
	DetourDetach(&(PVOID&)OriginalGetOEMCP, HookGetOEMCP);
	DetourDetach(&(PVOID&)OriginalSendMessageA, HookSendMessageA);
	DetourDetach(&(PVOID&)OriginalMultiByteToWideChar, HookMultiByteToWideChar);
	DetourDetach(&(PVOID&)OriginalWideCharToMultiByte, HookWideCharToMultiByte);
	DetourDetach(&(PVOID&)OriginalWinExec, HookWinExec);
	DetourDetach(&(PVOID&)OriginalCreateProcessA, HookCreateProcessA);
	//DetourDetach(&(PVOID&)OriginalShellExecuteA, HookShellExecuteA);
	
	//DetourDetach(&(PVOID&)OriginalDefWindowProcA, HookDefWindowProcA);
	//DetourDetach(&(PVOID&)OriginalDefDlgProcA, HookDefDlgProcA);
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

HWND WINAPI HookCreateWindowExA(
	DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	
	//NTLEA_TLS_DATA* p = GetTlsValueInternal();
	//DWORD PrevCallType = p->CurrentCallType; p->CurrentCallType = CT_CREATE_WINDOW;
	// 1. prepare the thread hook for the next step windowproc hook, then createwindow and hook it!
	LPCWSTR lpWindowNameW = (lpWindowName) ? MultiByteToWideCharInternal(lpWindowName) : NULL;
	LPCWSTR lpClassNameW = ((DWORD_PTR)lpClassName & 0xFFFF0000) ? MultiByteToWideCharInternal(lpClassName) : NULL;
	//InstallCbtHook(p);
	// 2. createwindow unicode, each window msghandler is unrelated : http://hi.baidu.com/tiancao222/item/d2f0dc370617dff3e6bb7a61
	HWND hwnd = CreateWindowExW(dwExStyle, (lpClassNameW ? lpClassNameW : (LPCWSTR)lpClassName), lpWindowNameW, dwStyle,
		x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	//	ntprintfA(128, 1, "%p, %p - %p, %p\n", lpClassName, lpWindowName, lpClassNameW, lpWindowNameW);
		// 3. unprepare the thread hook
	//UninstallCbtHook(p);
	if (lpWindowNameW) FreeStringInternal((LPVOID)lpWindowNameW);
	if (lpClassNameW) FreeStringInternal((LPVOID)lpClassNameW);
	// 4. cleanup resource 
	//p->CurrentCallType = PrevCallType;
	return hwnd;
}

UINT WINAPI HookGetACP(void)
{
	return settings.CodePage;
}
UINT WINAPI HookGetOEMCP(void)
{
	return settings.CodePage;
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
	case LB_FINDSTRINGEXACT: // LN305
	case LB_ADDSTRING: // LN305
	case LB_INSERTSTRING: // LN305
	case LB_FINDSTRING: // LN305
	case LB_ADDFILE: // LN305
	case LB_SELECTSTRING: // LN305
	case LB_DIR: // LN305
		type = 1;
		//	break;
	case CB_FINDSTRINGEXACT: // LN306
	case CB_ADDSTRING: // LN306
	case CB_INSERTSTRING: // LN306
	case CB_SELECTSTRING: // LN306
	case CB_DIR: // LN306
	case CB_FINDSTRING: // LN306
	{
		int ret = CheckWindowStyle(hWnd, type); // ebx = 0 / 1
		if (ret != -1) {
			LPCWSTR lParamW = lParam ? MultiByteToWideCharInternal((LPCSTR)lParam) : NULL;
			// LN899
			LRESULT hr = CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, (LPARAM)lParamW, Param1, Param2, Param3, FunctionType);
			if (lParamW/*ebx*/) FreeStringInternal((LPVOID)lParamW);
			return hr;
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
	//filelog << std::hex << uMsg << std::endl;
	return SendUnicodeMessage((LPVOID)(DWORD_PTR)SendMessageW, hWnd, uMsg, wParam, lParam, 0, 0, 0, 0);
}

typedef struct {
	WNDPROC		AnsiSystemClassProc;
	WNDPROC		UnicodeSystemClassProc;
} SYSTEM_CLASS_WNDPROC;

typedef struct {
	DWORD		InternalCall;
	DWORD		DBCSLeadByte;
	DWORD		IsFontAvailable;
	DWORD		CurrentCallType;
	DLGPROC		DialogProc;
	DWORD		IsCreateFileCall;
	LONG		hWindowHooking; // counter for avoiding setup/uninst hook-recursive
	HHOOK		hWindowCbtHookAnsi;
	HHOOK		hWindowCbtHookUnicode;
	SYSTEM_CLASS_WNDPROC SystemClassDesc[15];
	//	WNDPROC		DynamicClassProc[MAXSYSCLASSDESC]; // runtime cache !?
} NTLEA_TLS_DATA;

char const* SystemClassNameA[] = {
	"BUTTON", "COMBOBOX", "ComboLBox", /*"#32770",*/ "EDIT", "LISTBOX", "MDICLIENT", "RichEdit", "RICHEDIT_CLASS",
	"SCROLLBAR", "STATIC", "SysTreeView32", "SysListView32", "SysAnimate32", "SysHeader32", "tooltips_class32",
	//	"SysTabControl32", "ToolbarWindow32", "ComboBoxEx32", "SysDateTimePick32", "SysMonthCal32", "ReBarWindow32", 
	//	"msctls_progress32", "msctls_trackbar32", "msctls_statusbar32", "msctls_updown32", "msctls_hotkey32",
		/*NULL, */
};
wchar_t const* SystemClassNameW[] = {
	L"BUTTON", L"COMBOBOX", L"ComboLBox", /*L"32770",*/ L"EDIT", L"LISTBOX", L"MDICLIENT", L"RichEdit", L"RICHEDIT_CLASS",
	L"SCROLLBAR", L"STATIC", L"SysTreeView32", L"SysListView32", L"SysAnimate32", L"SysHeader32", L"tooltips_class32",
	//	L"SysTabControl32", L"ToolbarWindow32", L"ComboBoxEx32", L"SysDateTimePick32", L"SysMonthCal32", L"ReBarWindow32",
	//	L"msctls_progress32", L"msctls_trackbar32", L"msctls_statusbar32", L"msctls_updown32", L"msctls_hotkey32",
		/*NULL, */
};

inline LPVOID AllocateZeroedMemory(SIZE_T size/*eax*/) {
	return HeapAlloc(settings.hHeap, HEAP_ZERO_MEMORY, size);
}

inline NTLEA_TLS_DATA* GetTlsValueInternal(void) {
	DWORD n = GetLastError();
	NTLEA_TLS_DATA* p = (NTLEA_TLS_DATA*)TlsGetValue(settings.nTlsIndex);
	SetLastError(n); // thus the tlsgetvalue won't affect the env 
	if (!p) {
		p = (NTLEA_TLS_DATA*)AllocateZeroedMemory(sizeof(NTLEA_TLS_DATA));
		TlsSetValue(settings.nTlsIndex, p);
		for (int i = 0; i < 15; ++i) {
			WNDCLASSA wndclassa;
			if (GetClassInfoA(NULL, SystemClassNameA[i], &wndclassa)) {
				p->SystemClassDesc[i].AnsiSystemClassProc = wndclassa.lpfnWndProc;
			}
			WNDCLASSW wndclassw;
			if (GetClassInfoW(NULL, SystemClassNameW[i], &wndclassw)) {
				p->SystemClassDesc[i].UnicodeSystemClassProc = wndclassw.lpfnWndProc;
			}
			//	ntprintfA(256, 1, "info: %s - %p %p\n", SystemClassNameA[i], wndclassa.lpfnWndProc, wndclassw.lpfnWndProc);
		}
		SetLastError(0); // also restore the errorstate !
	}
	return p;
}

int WINAPI HookMultiByteToWideChar(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
	/*
	if (GetTlsValueInternal()->IsCreateFileCall) {
		GetTlsValueInternal()->IsCreateFileCall = 0;
		CodePage= (CodePage >= CP_UTF7) ? CodePage : CP_ACP; // create file should use default CP, or else file won't be found !! 
	}
	else
	*/
	//CodePage = (CodePage >= CP_UTF7) ? CodePage : settings.CodePage;
	return OriginalMultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}

int WINAPI HookWideCharToMultiByte(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
	//	if (lpMultiByteStr && lpWideCharStr) OutputDebugStringW(lpWideCharStr);
	//CodePage = (CodePage >= CP_UTF7) ? CodePage : settings.CodePage;
	return OriginalWideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
}

UINT WINAPI HookWinExec(
	_In_ LPCSTR lpCmdLine,
	_In_ UINT uCmdShow
)
{
	LPSTR lpCommandLine = (LPSTR)LocalAlloc(0, strlen(lpCmdLine) + 1);
	sprintf(lpCommandLine, "%s", lpCmdLine);

	LRConfigFileMap filemap;
	filemap.WrtieConfigFileMap(&settings);

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFOA));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFOA);
	/*return HookCreateProcessA(NULL, lpCommandLine, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi);*/
	bool ret = DetourCreateProcessWithDllExA(NULL, lpCommandLine, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi, settings.DllPath, NULL);
	filemap.FreeConfigFileMap();
	if (ret == TRUE)
		return 0x21;
	else return 0;
	//return OriginalWinExec(lpCmdLine, uCmdShow);
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
	//MessageBoxA(NULL, lpApplicationName, lpCommandLine, NULL);
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
		settings.DllPath, 
		OriginalCreateProcessA);
}

HINSTANCE HookShellExecuteA(
	_In_opt_ HWND hwnd,
	_In_opt_ LPCSTR lpOperation,
	_In_ LPCSTR lpFile,
	_In_opt_ LPCSTR lpParameters,
	_In_opt_ LPCSTR lpDirectory,
	_In_ INT nShowCmd
)
{
	MessageBox(NULL, TEXT("ShellExecuteA"), TEXT("ShellExecuteA"), NULL);
	return OriginalShellExecuteA(
		hwnd,
		lpOperation,
		lpFile,
		lpParameters,
		lpDirectory,
		nShowCmd);
}