#include"LRHookFunc.h"

UINT WINAPI HookGetOEMCP(void) { return HookGetACP(); }


void HookFunctions() {
	HMODULE hDLL;
	hDLL = LoadLibraryA("User32.dll");
	//HookDllFunc("MessageBoxA", HookMessageBoxA, hDLL);
	hDLL = LoadLibraryA("gdi32.dll");
	HookDllFunc("GdiGetCodePage", (LPVOID)(DWORD_PTR)HookGdiGetCodePage, hDLL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetACP, (LPVOID)(DWORD_PTR)HookGetACP, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetOEMCP, (LPVOID)(DWORD_PTR)HookGetOEMCP, NULL);
	//HookDllFunc((LPCSTR)(DWORD_PTR)CreateWindowExA, (LPVOID)(DWORD_PTR)HookCreateWindowExA, NULL);
	//HookFunc(&MessageBoxA, &HookMessageBoxA);
	HookDllFunc((LPCSTR)(DWORD_PTR)SendMessageA, (LPVOID)(DWORD_PTR)HookSendMessageA, NULL);
	//hDLL = LoadLibraryA("Kernel32.dll");
	fpMultiByteToWideChar = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)MultiByteToWideChar, (LPVOID)(DWORD_PTR)HookMultiByteToWideChar, NULL);
	fpWideCharToMultiByte = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)WideCharToMultiByte, (LPVOID)(DWORD_PTR)HookWideCharToMultiByte, NULL);
}

int WINAPI HookMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
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

UINT WINAPI HookGdiGetCodePage(HDC hdc)
{
	UNREFERENCED_PARAMETER(hdc);
	return 932;
}

UINT WINAPI HookGetACP(void)
{
	return 932;
}


/*LRESULT WINAPI HookSendMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	filelog << return_current_time_and_date() << " " << std::hex << uMsg << " " << wParam << " " << lParam << std::endl;
	return fpSendMessage(hWnd, uMsg, wParam, lParam);
}*/

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
	return SendUnicodeMessage((LPVOID)(DWORD_PTR)SendMessageW, hWnd, uMsg, wParam, lParam, 0, 0, 0, 0);
}

int WINAPI HookMultiByteToWideChar(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
	//filelog << CodePage << std::endl;
	CodePage = (CodePage >= CP_UTF7) ? CodePage : 932;
	return OriginalMultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}

int WINAPI HookWideCharToMultiByte(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
	//	if (lpMultiByteStr && lpWideCharStr) OutputDebugStringW(lpWideCharStr);
	CodePage = (CodePage >= CP_UTF7) ? CodePage : 932;
	return OriginalWideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
}