#include"LRHookFunc.h"
#include"LRWin32u.h"
#include"MessageTable.h"

//OriginalNtUserCreateWindowEx = AttachDllFunc("NtUserCreateWindowEx", HookNtUserCreateWindowEx, "user32.dll");

typedef HWND(WINAPI* NtUserCreateWindowExFn)(
	ULONG                   ExStyle,
	PLARGE_UNICODE_STRING   ClassName,
	PLARGE_UNICODE_STRING   ClassVersion,
	PLARGE_UNICODE_STRING   WindowName,
	ULONG                   Style,
	LONG                    X,
	LONG                    Y,
	LONG                    Width,
	LONG                    Height,
	HWND                    ParentWnd,
	HMENU                   Menu,
	PVOID                   Instance,
	LPVOID                  Param,
	ULONG                   ShowMode,
	ULONG                   Unknown1,
	ULONG                   Unknown2,
	ULONG                   Unknown3
	);
static NtUserCreateWindowExFn OriginalNtUserCreateWindowEx = (NtUserCreateWindowExFn)DetourFindFunction("win32u.dll", "NtUserCreateWindowEx");

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

HHOOK CbtHook = nullptr;
FILE* f = fopen("test.txt", "w");

/*************************************/
/* Unicde to Ansi UserCall Functions */
/*************************************/
LRESULT NTAPI UNICODE_EMPTY(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
}

LRESULT NTAPI UNICODE_INLPCREATESTRUCT(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LRESULT			Result;
	LPCREATESTRUCTW	CreateStructW;
	CREATESTRUCTA	CreateStructA;

	CreateStructW = (LPCREATESTRUCTW)lParam;

	CreateStructA.lpszClass	= nullptr;
	CreateStructA.lpszName	= nullptr;

	if (CreateStructW)
	{
		CreateStructA = *(LPCREATESTRUCTA)CreateStructW;
		CreateStructA.lpszClass	= WideCharToMultiByteInternal(CreateStructW->lpszClass);
		CreateStructA.lpszName	= WideCharToMultiByteInternal(CreateStructW->lpszName);

		lParam = (LPARAM)&CreateStructA;
	}

	Result = CallWindowProcA(PrevProc, Window, Message, wParam, lParam);

	FreeStringInternal((LPVOID)CreateStructA.lpszClass);
	FreeStringInternal((LPVOID)CreateStructA.lpszName);

	return Result;
}

LRESULT NTAPI UNICODE_INLPMDICREATESTRUCT(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LRESULT				Result;
	LPMDICREATESTRUCTW	MdiCreateStructW;
	MDICREATESTRUCTA	MdiCreateStructA;

	MdiCreateStructW = (LPMDICREATESTRUCTW)lParam;

	MdiCreateStructA.szClass = nullptr;
	MdiCreateStructA.szTitle = nullptr;

	if (MdiCreateStructW)
	{
		MdiCreateStructA = *(LPMDICREATESTRUCTA)MdiCreateStructW;
		MdiCreateStructA.szClass = WideCharToMultiByteInternal(MdiCreateStructW->szClass);
		MdiCreateStructA.szTitle = WideCharToMultiByteInternal(MdiCreateStructW->szTitle);

		lParam = (LPARAM)&MdiCreateStructA;
	}

	Result = CallWindowProcA(PrevProc, Window, Message, wParam, lParam);

	FreeStringInternal((LPVOID)MdiCreateStructA.szClass);
	FreeStringInternal((LPVOID)MdiCreateStructA.szTitle);

	return Result;
}

LRESULT NTAPI UNICODE_INSTRINGNULL(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result;
	LPWSTR	Unicode;
	LPSTR	Ansi;

	Unicode = (LPWSTR)lParam;

	MessageBoxW(NULL, Unicode, NULL, NULL);

	Ansi = nullptr;

	if (Unicode)
	{
		Ansi	=	WideCharToMultiByteInternal(Unicode);
		lParam	=	(LPARAM)Ansi;
	}

	Result = CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
	
	FreeStringInternal(Ansi);

	return Result;
}

LRESULT NTAPI UNICODE_OUTSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	MessageBoxA(NULL,"UNICODE_OUTSTRING",NULL,NULL);
	LPWSTR	UnicodeBuffer;
	LPSTR	AnsiBuffer;
	UINT	UnicodeSize, AnsiSize;

	UnicodeSize = wParam;
	UnicodeBuffer = (LPWSTR)lParam;

	AnsiSize = UnicodeSize * sizeof(WCHAR);
	AnsiBuffer = (LPSTR)AllocateZeroedMemory(AnsiSize);
	if (AnsiBuffer == nullptr)
		return 0;

	wParam = AnsiSize;
	lParam = (LPARAM)AnsiBuffer;

	AnsiSize = CallWindowProcA(PrevProc, Window, Message, wParam, lParam);

	if (AnsiSize != 0)
		WideCharToMultiByte(CP_ACP, 0, UnicodeBuffer, UnicodeSize, AnsiBuffer, AnsiSize, NULL, NULL);

	FreeStringInternal(AnsiBuffer);

	return AnsiSize / sizeof(WCHAR);
}

LRESULT NTAPI UNICODE_INSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return UNICODE_INSTRINGNULL(PrevProc, Window, Message, wParam, lParam);
}

LRESULT NTAPI UNICODE_INCNTOUTSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//MessageBoxA(NULL, "UNICODE_INCNTOUTSTRING", NULL, NULL);
	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
}

LRESULT NTAPI UNICODE_INCBOXSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return UNICODE_INSTRINGNULL(PrevProc, Window, Message, wParam, lParam);
}

LRESULT NTAPI UNICODE_OUTCBOXSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//MessageBoxA(NULL, "UNICODE_OUTCBOXSTRING", NULL, NULL);
	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
}

LRESULT NTAPI UNICODE_INLBOXSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return UNICODE_INSTRINGNULL(PrevProc, Window, Message, wParam, lParam);
}

LRESULT NTAPI UNICODE_OUTLBOXSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return UNICODE_INSTRINGNULL(PrevProc, Window, Message, wParam, lParam);
}

LRESULT NTAPI UNICODE_INCNTOUTSTRINGNULL(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return UNICODE_INSTRINGNULL(PrevProc, Window, Message, wParam, lParam);
}

LRESULT NTAPI UNICODE_GETDBCSTEXTLENGTHS(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//MessageBoxA(NULL, "UNICODE_GETDBCSTEXTLENGTHS", NULL, NULL);
	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
}

/****************************************/
/* Ansi to Unicode KernelCall Functions */
/****************************************/
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
	LRESULT			Result;
	LPCREATESTRUCTA CreateStructA;
	CREATESTRUCTW   CreateStructW;

	CreateStructA = (LPCREATESTRUCTA)lParam;
	CreateStructW.lpszClass	=	nullptr;
	CreateStructW.lpszName	=	nullptr;
	
	if (CreateStructA)
	{
		CreateStructW = *(LPCREATESTRUCTW)CreateStructA;
		CreateStructW.lpszClass = MultiByteToWideCharInternal(CreateStructA->lpszClass);
		if (CreateStructA->lpszClass != nullptr)
		{
			CreateStructW.lpszName = MultiByteToWideCharInternal(CreateStructA->lpszName);
			
			CLEAR_FLAG(Flags, WINDOW_FLAG_ANSI);
			/*if(CreateStructA->lpszName)
			filelog << CreateStructA->lpszName << std::endl;*/
		}
		lParam = (LPARAM)&CreateStructW;
	}
	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
	FreeStringInternal((LPVOID)CreateStructW.lpszClass);
	FreeStringInternal((LPVOID)CreateStructW.lpszName);
	
	return Result;
}

LRESULT NTAPI ANSI_INLPMDICREATESTRUCT(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	LRESULT				Result;
	LPMDICREATESTRUCTA  MdiCreateStructA;
	MDICREATESTRUCTW    MdiCreateStructW;

	MdiCreateStructA = (LPMDICREATESTRUCTA)lParam;
	MdiCreateStructW.szClass = nullptr;
	MdiCreateStructW.szTitle = nullptr;

	if (MdiCreateStructA)
	{
		MdiCreateStructW = *(LPMDICREATESTRUCTW)MdiCreateStructA;
		MdiCreateStructW.szClass = MultiByteToWideCharInternal(MdiCreateStructA->szClass);
		MdiCreateStructW.szTitle = MultiByteToWideCharInternal(MdiCreateStructA->szTitle);

		CLEAR_FLAG(Flags, WINDOW_FLAG_ANSI);

		lParam = (LPARAM)&MdiCreateStructW;
	}
	
	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);

	FreeStringInternal((LPVOID)MdiCreateStructW.szClass);
	FreeStringInternal((LPVOID)MdiCreateStructW.szTitle);

	return Result;
}

LRESULT NTAPI ANSI_INSTRINGNULL(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	LRESULT	Result;
	LPSTR	Ansi;
	LPWSTR	Unicode;

	Ansi	= (LPSTR)lParam;
	Unicode	= nullptr;

	if (Ansi)
	{
		Unicode = MultiByteToWideCharInternal(Ansi);
		lParam = (LPARAM)Unicode;
		CLEAR_FLAG(Flags, WINDOW_FLAG_ANSI);
	}

	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
	FreeStringInternal(Unicode);
	return Result;
}

LRESULT NTAPI ANSI_OUTSTRING(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	LRESULT Result;
	MessageBoxA(NULL, "ANSI_OUTSTRING", NULL, NULL);
	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
	return Result;
}

LRESULT NTAPI ANSI_INSTRING(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	return ANSI_INSTRINGNULL(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
}

LRESULT NTAPI ANSI_INCNTOUTSTRING(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	LRESULT Result;
	MessageBoxA(NULL, "ANSI_INCNTOUTSTRING", NULL, NULL);
	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
	return Result;
}

LRESULT NTAPI ANSI_INCBOXSTRING(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	return ANSI_INSTRINGNULL(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
}

LRESULT NTAPI ANSI_OUTCBOXSTRING(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	LRESULT Result;
	MessageBoxA(NULL, "ANSI_OUTCBOXSTRING", NULL, NULL);
	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
	return Result;
}

LRESULT NTAPI ANSI_INLBOXSTRING(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	return ANSI_INSTRINGNULL(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
}

LRESULT NTAPI ANSI_OUTLBOXSTRING(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	return ANSI_INSTRINGNULL(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
}

LRESULT NTAPI ANSI_INCNTOUTSTRINGNULL(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	return ANSI_INSTRINGNULL(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
}

LRESULT NTAPI ANSI_GETDBCSTEXTLENGTHS(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
{
	LRESULT Result;
	MessageBoxA(NULL, "ANSI_GETDBCSTEXTLENGTHS", NULL, NULL);
	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
	return Result;
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

	if (nCode == HCBT_CREATEWND)
	{
		SetPropW(hWnd, L"OriginalProcA", OriginalProcA);

		SetWindowLongW(hWnd, GWLP_WNDPROC, (LONG)WindowProcW);
	}
	
	return CallNextHookEx(CbtHook, nCode, wParam, lParam);
}

HWND WINAPI HookNtUserCreateWindowEx(
	ULONG                   ExStyle,
	PLARGE_UNICODE_STRING   ClassName,
	PLARGE_UNICODE_STRING   ClassVersion,
	PLARGE_UNICODE_STRING   WindowName,
	ULONG                   Style,
	LONG                    X,
	LONG                    Y,
	LONG                    Width,
	LONG                    Height,
	HWND                    ParentWnd,
	HMENU                   Menu,
	PVOID                   Instance,
	LPVOID                  Param,
	ULONG                   ShowMode,
	ULONG                   Unknown1,
	ULONG                   Unknown2,
	ULONG                   Unknown3
)
{
	LARGE_UNICODE_STRING UnicodeWindowName;

	InitEmptyLargeString(&UnicodeWindowName);

	LOOP_ONCE
	{
		if (!FLAG_ON(ExStyle, WS_EX_ANSI))
		{
			break;
		}
		if (WindowName != nullptr)
		{
			if (CaptureAnsiWindowName(WindowName, &UnicodeWindowName) == nullptr)
				break;
		}

		WindowName = &UnicodeWindowName;
		//fprintf(f, "%lx %d %d %d\n", ex_style, window_name->Length, window_name->MaximumLength, window_name->Ansi);
		//LPSTR lstr = WideCharToMultiByteInternal(WindowName->UnicodeBuffer, 949);
		//filelog << lstr << std::endl;
		CbtHook = SetWindowsHookExA(WH_CBT, CBTProc, nullptr, GetCurrentThreadId());
	}
	
	HWND ret = OriginalNtUserCreateWindowEx(ExStyle,
		ClassName,
		ClassVersion,
		WindowName,
		Style,
		X,
		Y,
		Width,
		Height,
		ParentWnd,
		Menu,
		Instance,
		Param,
		ShowMode,
		Unknown1,
		Unknown2,
		Unknown3
	);

	if (CbtHook)
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
	LOOP_ONCE
	{
		if (Message >= MessageSize)
			break;

		if (!FLAG_ON(Flags, WINDOW_FLAG_ANSI))
			break;

		return MessageTable[Message].AnsiCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
	}
	return OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
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

	//DetourAttach(&(PVOID&)OriginalCreateWindowExA, HookCreateWindowExA);
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
	CbtHook = SetWindowsHookExA(WH_CBT, CBTProc, nullptr, GetCurrentThreadId());
	HWND ret = OriginalCreateWindowExA(
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
	if (CbtHook)
		UnhookWindowsHookEx(CbtHook);
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