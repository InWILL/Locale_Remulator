#include"LRHookFunc.h"
#include"LRWin32u.h"
#include"MessageTable.h"

//OriginalNtUserCreateWindowEx = AttachDllFunc("NtUserCreateWindowEx", HookNtUserCreateWindowEx, "user32.dll");

static PVOID OriginalNtUserCreateWindowEx = DetourFindFunction("win32u.dll", "NtUserCreateWindowEx");

typedef LRESULT(WINAPI* NtUserMessageCallFn)(
	HWND         Window,
	UINT         Message,
	WPARAM       wParam,
	LPARAM       lParam,
	ULONG_PTR    xParam,
	ULONG        xpfnProc,
	ULONG        Flags
	);
static NtUserMessageCallFn OriginalNtUserMessageCall = (NtUserMessageCallFn)DetourFindFunction("win32u.dll", "NtUserMessageCall");

HHOOK CbtHook;
//FILE* f = fopen("test.txt", "w");

/* Unicde to Ansi UserCall Functions */
LRESULT NTAPI UNICODE_EMPTY(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
}

LRESULT NTAPI UNICODE_INLPCREATESTRUCT(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LPCREATESTRUCTW CreateStructW;
	CREATESTRUCTA   CreateStructA;

	CreateStructW = (LPCREATESTRUCTW)lParam;

	if (CreateStructW)
	{
		CreateStructA = *(LPCREATESTRUCTA)CreateStructW;
		CreateStructA.lpszClass	= WideCharToMultiByteInternal(CreateStructW->lpszClass, settings.CodePage);
		CreateStructA.lpszName	= WideCharToMultiByteInternal(CreateStructW->lpszName, settings.CodePage);
		
		lParam = (LPARAM)&CreateStructA;

		LPSTR lstr = WideCharToMultiByteInternal(CreateStructW->lpszName, settings.CodePage);
		//fputws(CreateStructW->lpszName, f);
		
		/*if(CreateStructW->lpszName)*/
		//filelog << lstr << std::endl;
	}
	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
}

/* Ansi to Unicode KernelCall Functions */
LRESULT NTAPI ANSI_EMPTY(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	return OriginalNtUserMessageCall(
		Window,
		Message,
		wParam,
		lParam,
		xParam,
		xpfnProc,
		Flags
	);
}


LRESULT NTAPI ANSI_INLPCREATESTRUCT(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	LPCREATESTRUCTA CreateStructA;
	CREATESTRUCTW   CreateStructW;

	CreateStructA = (LPCREATESTRUCTA)lParam;
	
	if (CreateStructA)
	{
		CreateStructW = *(LPCREATESTRUCTW)CreateStructA;
		CreateStructW.lpszClass = MultiByteToWideCharInternal(CreateStructA->lpszClass, settings.CodePage);
		if (CreateStructA->lpszClass != nullptr)
		{
			CreateStructW.lpszName = MultiByteToWideCharInternal(CreateStructA->lpszName, settings.CodePage);
			
			CLEAR_FLAG(Flags, WINDOW_FLAG_ANSI);
			/*if(CreateStructA->lpszName)
			filelog << CreateStructA->lpszName << std::endl;*/
		}
		lParam = (LPARAM)&CreateStructW;
	}
	return OriginalNtUserMessageCall(
		Window,
		Message,
		wParam,
		lParam,
		xParam,
		xpfnProc,
		Flags
		);
}

LRESULT NTAPI WindowProcW(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC	PrevProc = (WNDPROC)GetPropW(Window, L"OriginalProcA");
	if(Message < MessageSize)
	{
		return MessageTable[Message].UnicodeCall(PrevProc, Window, Message, wParam, lParam);
	}
	
	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
}

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;
	WNDPROC OriginalProcA = (WNDPROC)GetWindowLongA(hWnd, GWLP_WNDPROC);

	SetPropW(hWnd, L"OriginalProcA", OriginalProcA);

	SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG)WindowProcW);
	return CallNextHookEx(CbtHook, nCode, wParam, lParam);
}

HWND WINAPI HookNtUserCreateWindowEx(DWORD ex_style, PLARGE_UNICODE_STRING class_name,
	PLARGE_UNICODE_STRING version, PLARGE_UNICODE_STRING window_name,
	DWORD style, INT x, INT y, INT cx, INT cy,
	HWND parent, HMENU menu, HINSTANCE instance, void* params,
	DWORD flags, HINSTANCE client_instance, DWORD unk, BOOL ansi)
{
	LARGE_UNICODE_STRING UnicodeWindowName;

	InitEmptyLargeString(&UnicodeWindowName);

	LOOP_ONCE
	{
		if (!FLAG_ON(ex_style, WS_EX_ANSI))
		{
			break;
		}
		if (window_name != nullptr)
		{
			if (CaptureAnsiWindowName(window_name, &UnicodeWindowName) == nullptr)
				break;
		}

		window_name = &UnicodeWindowName;
		// fprintf(f, "%lx %d %d %d\n", ex_style, window_name->Length, window_name->MaximumLength, window_name->Ansi);
		//LPSTR lstr = WideCharToMultiByteInternal(window_name->UnicodeBuffer, 949);
		//filelog << lstr << std::endl;
		CbtHook = SetWindowsHookExA(WH_CBT, CBTProc, nullptr, GetCurrentThreadId());
	}

	typedef HWND(WINAPI* Fn)(DWORD ex_style, PLARGE_UNICODE_STRING class_name,
		PLARGE_UNICODE_STRING version, PLARGE_UNICODE_STRING window_name,
		DWORD style, INT x, INT y, INT cx, INT cy,
		HWND parent, HMENU menu, HINSTANCE instance, void* params,
		DWORD flags, HINSTANCE client_instance, DWORD unk, BOOL ansi);
	HWND ret = ((Fn)OriginalNtUserCreateWindowEx)(ex_style, class_name,
		version, window_name,
		style, x, y, cx, cy,
		parent, menu, instance, params,
		flags, client_instance, unk, ansi);

	if (CbtHook != nullptr)
		UnhookWindowsHookEx(CbtHook);

	return ret;
}

LRESULT WINAPI HookNtUserMessageCall(
	HWND         Window,
	UINT         Message,
	WPARAM       wParam,
	LPARAM       lParam,
	ULONG_PTR    xParam,
	ULONG        xpfnProc,
	ULONG        Flags
)
{
	if(Message < MessageSize)
	{
		return MessageTable[Message].AnsiCall(
			Window,
			Message,
			wParam,
			lParam,
			xParam,
			xpfnProc,
			Flags
		);
	}
	return OriginalNtUserMessageCall(
		Window,
		Message,
		wParam,
		lParam,
		xParam,
		xpfnProc,
		Flags
		);
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

	DetourAttach(&(PVOID&)OriginalNtUserCreateWindowEx, HookNtUserCreateWindowEx);
	DetourAttach(&(PVOID&)OriginalNtUserMessageCall, HookNtUserMessageCall);

	//DetourAttach(&(PVOID&)OriginalWinExec, HookWinExec);
	//DetourAttach(&(PVOID&)OriginalCreateProcessA, HookCreateProcessA);
	//DetourAttach(&(PVOID&)OriginalCreateProcessW, HookCreateProcessW);
	////DetourAttach(&(PVOID&)OriginalShellExecuteA, HookShellExecuteA);
	////DetourAttach(&(PVOID&)OriginalShellExecuteW, HookShellExecuteW);
	////DetourAttach(&(PVOID&)OriginalShellExecuteExA, HookShellExecuteExA);
	////DetourAttach(&(PVOID&)OriginalShellExecuteExW, HookShellExecuteExW);

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

	
	DetourDetach(&(PVOID&)OriginalWinExec, HookWinExec);
	DetourDetach(&(PVOID&)OriginalCreateProcessA, HookCreateProcessA);
	DetourDetach(&(PVOID&)OriginalCreateProcessW, HookCreateProcessW);
	//DetourDetach(&(PVOID&)OriginalShellExecuteA, HookShellExecuteA);
	//DetourDetach(&(PVOID&)OriginalShellExecuteW, HookShellExecuteW);
	//DetourDetach(&(PVOID&)OriginalShellExecuteExA, HookShellExecuteExA);
	//DetourDetach(&(PVOID&)OriginalShellExecuteExW, HookShellExecuteExW);
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
