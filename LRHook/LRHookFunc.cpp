#include"LRHookFunc.h"

//WNDPROC originalProc;
ORIGINAL Original = { NULL };

void AttachFunctions() 
{
	//SetWindowsHookExW(WH_CBT, CBTProc, NULL, GetCurrentThreadId());
	DetourAttach(&(PVOID&)OriginalCreateWindowExA, HookCreateWindowExA);
	DetourAttach(&(PVOID&)OriginalMessageBoxA, HookMessageBoxA);
	DetourAttach(&(PVOID&)OriginalGetACP, HookGetACP);
	DetourAttach(&(PVOID&)OriginalGetOEMCP, HookGetOEMCP);
	DetourAttach(&(PVOID&)OriginalSendMessageA, HookSendMessageA);
	DetourAttach(&(PVOID&)OriginalMultiByteToWideChar, HookMultiByteToWideChar);
	DetourAttach(&(PVOID&)OriginalWideCharToMultiByte, HookWideCharToMultiByte);
	DetourAttach(&(PVOID&)OriginalWinExec, HookWinExec);
	DetourAttach(&(PVOID&)OriginalCreateProcessA, HookCreateProcessA);
	DetourAttach(&(PVOID&)OriginalCreateProcessW, HookCreateProcessW);
	//DetourAttach(&(PVOID&)OriginalShellExecuteA, HookShellExecuteA);
	//DetourAttach(&(PVOID&)OriginalShellExecuteW, HookShellExecuteW);
	
	DetourAttach(&(PVOID&)OriginalSetWindowTextA, HookSetWindowTextA);
	//DetourAttach(&(PVOID&)OriginalGetWindowTextA, HookGetWindowTextA);
	DetourAttach(&(PVOID&)OriginalCreateFontIndirectA, HookCreateFontIndirectA);
	DetourAttach(&(PVOID&)OriginalTextOutA, HookTextOutA);
	DetourAttach(&(PVOID&)OriginalGetClipboardData, HookGetClipboardData);
	DetourAttach(&(PVOID&)OriginalSetClipboardData, HookSetClipboardData);
	
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

	//DetourAttach(&(PVOID&)OriginalDefWindowProcA, HookDefWindowProcA);
	//DetourAttach(&(PVOID&)OriginalDefDlgProcA, HookDefDlgProcA);
}

void DetachFunctions() 
{
	DetourDetach(&(PVOID&)OriginalCreateWindowExA, HookCreateWindowExA);
	DetourDetach(&(PVOID&)OriginalMessageBoxA, HookMessageBoxA);
	DetourDetach(&(PVOID&)OriginalGetACP, HookGetACP);
	DetourDetach(&(PVOID&)OriginalGetOEMCP, HookGetOEMCP);
	DetourDetach(&(PVOID&)OriginalSendMessageA, HookSendMessageA);
	DetourDetach(&(PVOID&)OriginalMultiByteToWideChar, HookMultiByteToWideChar);
	DetourDetach(&(PVOID&)OriginalWideCharToMultiByte, HookWideCharToMultiByte);
	DetourDetach(&(PVOID&)OriginalWinExec, HookWinExec);
	DetourDetach(&(PVOID&)OriginalCreateProcessA, HookCreateProcessA);
	DetourDetach(&(PVOID&)OriginalCreateProcessW, HookCreateProcessW);
	//DetourDetach(&(PVOID&)OriginalShellExecuteA, HookShellExecuteA);
	//DetourDetach(&(PVOID&)OriginalShellExecuteW, HookShellExecuteW);

	DetourDetach(&(PVOID&)OriginalSetWindowTextA, HookSetWindowTextA);
	//DetourDetach(&(PVOID&)OriginalGetWindowTextA, HookGetWindowTextA);
	DetourDetach(&(PVOID&)OriginalCreateFontIndirectA, HookCreateFontIndirectA);
	DetourDetach(&(PVOID&)OriginalTextOutA, HookTextOutA);
	DetourDetach(&(PVOID&)OriginalGetClipboardData, HookGetClipboardData);
	DetourDetach(&(PVOID&)OriginalSetClipboardData, HookSetClipboardData);

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

	//DetourDetach(&(PVOID&)OriginalDefWindowProcA, HookDefWindowProcA);
	//DetourDetach(&(PVOID&)OriginalDefDlgProcA, HookDefDlgProcA);
}

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_CREATEWND)
	{
		HWND hWnd = (HWND)wParam;
		LPCBT_CREATEWNDW CreateWnd = (LPCBT_CREATEWNDW)lParam;
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

HWND WINAPI HookCreateWindowExA(
	DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
	int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (dwExStyle == 257 || hWndParent == NULL)
		return OriginalCreateWindowExA(
			dwExStyle,
			lpClassName,
			lpWindowName,
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
	LPCWSTR wstrlpClassName = lpClassName ? MultiByteToWideCharInternal(lpClassName, Original.CodePage) : NULL;
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
	case EM_REPLACESEL: // LN320
	case WM_SETTEXT: // LN320
	case WM_SETTINGCHANGE: // LN320
	case WM_DEVMODECHANGE: // LN320
	{
		LPCWSTR lParamW = lParam ? MultiByteToWideCharInternal((LPCSTR)lParam) : NULL;
		//	ntprintfA(1024, 1, "3. A(%s) -> W(%S)", (LPCSTR)lParam, lParamW);
		LRESULT hr = CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, (LPARAM)lParamW, Param1, Param2, Param3, FunctionType);
		// LN301
		if (lParamW) FreeStringInternal((LPVOID)lParamW);
		return hr;
	}	break;
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
			if (lParamW) FreeStringInternal((LPVOID)lParamW);
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
	//MessageBoxA(NULL, lpCmdLine, "WinExec", NULL);
	//LRConfigFileMap filemap;
	//filemap.WrtieConfigFileMap(&settings);

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFOA));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFOA);
	
	bool ret = DetourCreateProcessWithDllExA(NULL, lpCmdLine, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi, Original.DllPath, OriginalCreateProcessA);

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
	//MessageBoxA(NULL, lpApplicationName, "HookCreateProcessA", NULL);
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
	//MessageBoxW(NULL, lpApplicationName, TEXT("HookCreateProcessW"), NULL);
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
	int len = (int)SendMessageW(hWnd, WM_GETTEXTLENGTH, 0, 0) + 1;
	LPWSTR lpStringW = (LPWSTR)AllocateZeroedMemory(len * sizeof(wchar_t));

	int ret = GetWindowTextW(hWnd, lpStringW, len);
	if (ret > 0) {
		int size = WideCharToMultiByte(CP_ACP, 0, lpStringW, -1, lpString, nMaxCount, NULL, NULL);
		if (size > 0) ret = size - 1;
		else {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				lpString[nMaxCount - 1] = '\0'; ret = nMaxCount - 1;
			}
			else {
				lpString[0] = '\0'; ret = 0;
			}
		}
	}
	else {
		lpString[0] = '\0'; ret = 0;
	}
	FreeStringInternal(lpStringW);
	return ret;
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

//set font characters set
HFONT WINAPI HookCreateFontIndirectA(
	LOGFONTA* lplf
)
{
	//MessageBoxA(NULL, lplf->lfFaceName, "HookCreateFontIndirectA", NULL);
	//lplf->lfCharSet = JOHAB_CHARSET;
	if (strcmp(settings.lfFaceName, "None") != 0)
		strcpy(lplf->lfFaceName, settings.lfFaceName);
	return OriginalCreateFontIndirectA(lplf);
}

BOOL WINAPI HookTextOutA(
	HDC    hdc,
	int    x,
	int    y,
	LPSTR lpString,
	int    c
)
{
	LPWSTR wstr = MultiByteToWideCharInternal(lpString, settings.CodePage);
	if (wstr)
	{
		bool ret = TextOutW(hdc, x, y, wstr, 1);
		FreeStringInternal(wstr);
		return ret;
	}
	return OriginalTextOutA(hdc, x, y, lpString, c);
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
