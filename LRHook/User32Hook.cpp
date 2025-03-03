#include"User32Hook.h"
//
//#define FLAG_ON(_V, _F)     (!!((_V) & (_F)))
//
//#define WS_EX_ANSI          0x80000000
//#define WINDOW_FLAG_ANSI    0x00000001
//
//#define CLEAR_FLAG(_V, _F)  ((_V) &= ~(_F))
//
//typedef struct _LARGE_UNICODE_STRING
//{
//	ULONG Length;
//	ULONG MaximumLength : 31;
//	ULONG Ansi : 1;
//
//	union
//	{
//		PWSTR   UnicodeBuffer;
//		PSTR    AnsiBuffer;
//		ULONG64 Buffer;
//	};
//
//} LARGE_UNICODE_STRING, * PLARGE_UNICODE_STRING;
//
//typedef struct _UNICODE_STRING {
//	USHORT Length;
//	USHORT MaximumLength;
//	PWSTR  Buffer;
//} UNICODE_STRING, * PUNICODE_STRING;
//
//typedef struct _STRING {
//	USHORT Length;
//	USHORT MaximumLength;
//	PCHAR Buffer;
//} ANSI_STRING, * PANSI_STRING;
//
//typedef HWND(WINAPI* NtUserCreateWindowExFn)(
//	ULONG                   ExStyle,
//	PLARGE_UNICODE_STRING   ClassName,
//	PLARGE_UNICODE_STRING   ClassVersion,
//	PLARGE_UNICODE_STRING   WindowName,
//	ULONG                   Style,
//	LONG                    X,
//	LONG                    Y,
//	LONG                    Width,
//	LONG                    Height,
//	HWND                    ParentWnd,
//	HMENU                   Menu,
//	PVOID                   Instance,
//	LPVOID                  Param,
//	ULONG                   ShowMode,
//	ULONG                   Unknown1,
//	ULONG                   Unknown2,
//	ULONG                   Unknown3
//	);
//static NtUserCreateWindowExFn OriginalNtUserCreateWindowEx = (NtUserCreateWindowExFn)DetourFindFunction("win32u.dll", "NtUserCreateWindowEx");
//
//typedef LRESULT(WINAPI* NtUserMessageCallFn)(
//	HWND         Window,
//	UINT         Message,
//	WPARAM       wParam,
//	LPARAM       lParam,
//	ULONG_PTR    xParam,
//	ULONG        xpfnProc,
//	ULONG        Flags
//	);
//static NtUserMessageCallFn OriginalNtUserMessageCall = (NtUserMessageCallFn)DetourFindFunction("win32u.dll", "NtUserMessageCall");
//
///*************************************/
///* Unicde to Ansi UserCall Functions */
///*************************************/
//LRESULT NTAPI UNICODE_EMPTY(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
//}
//
//LRESULT NTAPI UNICODE_INLPCREATESTRUCT(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	LRESULT			Result;
//	LPCREATESTRUCTW	CreateStructW;
//	CREATESTRUCTA	CreateStructA;
//
//	CreateStructW = (LPCREATESTRUCTW)lParam;
//
//	CreateStructA.lpszClass = nullptr;
//	CreateStructA.lpszName = nullptr;
//
//	if (CreateStructW)
//	{
//		CreateStructA = *(LPCREATESTRUCTA)CreateStructW;
//		CreateStructA.lpszClass = WideCharToMultiByteInternal(CreateStructW->lpszClass);
//		CreateStructA.lpszName = WideCharToMultiByteInternal(CreateStructW->lpszName);
//
//		filelog << CreateStructA.lpszName << std::endl;
//
//		lParam = (LPARAM)&CreateStructA;
//	}
//
//	Result = CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
//
//	FreeStringInternal((LPVOID)CreateStructA.lpszClass);
//	FreeStringInternal((LPVOID)CreateStructA.lpszName);
//
//	return Result;
//}
//
//LRESULT NTAPI UNICODE_INLPMDICREATESTRUCT(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	LRESULT				Result;
//	LPMDICREATESTRUCTW	MdiCreateStructW;
//	MDICREATESTRUCTA	MdiCreateStructA;
//
//	MdiCreateStructW = (LPMDICREATESTRUCTW)lParam;
//
//	MdiCreateStructA.szClass = nullptr;
//	MdiCreateStructA.szTitle = nullptr;
//
//	if (MdiCreateStructW)
//	{
//		MdiCreateStructA = *(LPMDICREATESTRUCTA)MdiCreateStructW;
//		MdiCreateStructA.szClass = WideCharToMultiByteInternal(MdiCreateStructW->szClass);
//		MdiCreateStructA.szTitle = WideCharToMultiByteInternal(MdiCreateStructW->szTitle);
//
//		lParam = (LPARAM)&MdiCreateStructA;
//	}
//
//	Result = CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
//
//	FreeStringInternal((LPVOID)MdiCreateStructA.szClass);
//	FreeStringInternal((LPVOID)MdiCreateStructA.szTitle);
//
//	return Result;
//}
//
//LRESULT NTAPI UNICODE_INSTRINGNULL(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	LRESULT Result;
//	LPWSTR	Unicode;
//	LPSTR	Ansi;
//
//	Unicode = (LPWSTR)lParam;
//
//	MessageBoxW(NULL, Unicode, NULL, NULL);
//
//	Ansi = nullptr;
//
//	if (Unicode)
//	{
//		Ansi = WideCharToMultiByteInternal(Unicode);
//		lParam = (LPARAM)Ansi;
//	}
//
//	Result = CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
//
//	FreeStringInternal(Ansi);
//
//	return Result;
//}
//
//LRESULT NTAPI UNICODE_OUTSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	MessageBoxA(NULL, "UNICODE_OUTSTRING", NULL, NULL);
//	LPWSTR	UnicodeBuffer;
//	LPSTR	AnsiBuffer;
//	UINT	UnicodeSize, AnsiSize;
//
//	UnicodeSize = wParam;
//	UnicodeBuffer = (LPWSTR)lParam;
//
//	AnsiSize = UnicodeSize * sizeof(WCHAR);
//	AnsiBuffer = (LPSTR)AllocateZeroedMemory(AnsiSize);
//	if (AnsiBuffer == nullptr)
//		return 0;
//
//	wParam = AnsiSize;
//	lParam = (LPARAM)AnsiBuffer;
//
//	AnsiSize = CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
//
//	if (AnsiSize != 0)
//		WideCharToMultiByte(CP_ACP, 0, UnicodeBuffer, UnicodeSize, AnsiBuffer, AnsiSize, NULL, NULL);
//
//	FreeStringInternal(AnsiBuffer);
//
//	return AnsiSize / sizeof(WCHAR);
//}
//
//LRESULT NTAPI UNICODE_INSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	return UNICODE_INSTRINGNULL(PrevProc, Window, Message, wParam, lParam);
//}
//
//LRESULT NTAPI UNICODE_INCNTOUTSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	//MessageBoxA(NULL, "UNICODE_INCNTOUTSTRING", NULL, NULL);
//	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
//}
//
//LRESULT NTAPI UNICODE_INCBOXSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	return UNICODE_INSTRINGNULL(PrevProc, Window, Message, wParam, lParam);
//}
//
//LRESULT NTAPI UNICODE_OUTCBOXSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	//MessageBoxA(NULL, "UNICODE_OUTCBOXSTRING", NULL, NULL);
//	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
//}
//
//LRESULT NTAPI UNICODE_INLBOXSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	return UNICODE_INSTRINGNULL(PrevProc, Window, Message, wParam, lParam);
//}
//
//LRESULT NTAPI UNICODE_OUTLBOXSTRING(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	return UNICODE_INSTRINGNULL(PrevProc, Window, Message, wParam, lParam);
//}
//
//LRESULT NTAPI UNICODE_INCNTOUTSTRINGNULL(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	return UNICODE_INSTRINGNULL(PrevProc, Window, Message, wParam, lParam);
//}
//
//LRESULT NTAPI UNICODE_GETDBCSTEXTLENGTHS(WNDPROC PrevProc, HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	//MessageBoxA(NULL, "UNICODE_GETDBCSTEXTLENGTHS", NULL, NULL);
//	return CallWindowProcA(PrevProc, Window, Message, wParam, lParam);
//}
//
/****************************************/
/* Ansi to Unicode KernelCall Functions */
/****************************************/
//LRESULT NTAPI ANSI_INLPCREATESTRUCT(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
//{
//	LRESULT			Result;
//	LPCREATESTRUCTA CreateStructA;
//	CREATESTRUCTW   CreateStructW;
//
//	CreateStructA = (LPCREATESTRUCTA)lParam;
//	CreateStructW.lpszClass = nullptr;
//	CreateStructW.lpszName = nullptr;
//
//	if (CreateStructA)
//	{
//		CreateStructW = *(LPCREATESTRUCTW)CreateStructA;
//		CreateStructW.lpszClass = MultiByteToWideCharInternal(CreateStructA->lpszClass);
//		if (CreateStructA->lpszClass != nullptr)
//		{
//			CreateStructW.lpszName = MultiByteToWideCharInternal(CreateStructA->lpszName);
//
//			CLEAR_FLAG(Flags, WINDOW_FLAG_ANSI);
//			/*if(CreateStructA->lpszName)
//			filelog << CreateStructA->lpszName << std::endl;*/
//		}
//		lParam = (LPARAM)&CreateStructW;
//	}
//	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
//	FreeStringInternal((LPVOID)CreateStructW.lpszClass);
//	FreeStringInternal((LPVOID)CreateStructW.lpszName);
//
//	return Result;
//}
//
//LRESULT NTAPI ANSI_INLPMDICREATESTRUCT(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
//{
//	LRESULT				Result;
//	LPMDICREATESTRUCTA  MdiCreateStructA;
//	MDICREATESTRUCTW    MdiCreateStructW;
//
//	MdiCreateStructA = (LPMDICREATESTRUCTA)lParam;
//	MdiCreateStructW.szClass = nullptr;
//	MdiCreateStructW.szTitle = nullptr;
//
//	if (MdiCreateStructA)
//	{
//		MdiCreateStructW = *(LPMDICREATESTRUCTW)MdiCreateStructA;
//		MdiCreateStructW.szClass = MultiByteToWideCharInternal(MdiCreateStructA->szClass);
//		MdiCreateStructW.szTitle = MultiByteToWideCharInternal(MdiCreateStructA->szTitle);
//
//		CLEAR_FLAG(Flags, WINDOW_FLAG_ANSI);
//
//		lParam = (LPARAM)&MdiCreateStructW;
//	}
//
//	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
//
//	FreeStringInternal((LPVOID)MdiCreateStructW.szClass);
//	FreeStringInternal((LPVOID)MdiCreateStructW.szTitle);
//
//	return Result;
//}
//
LRESULT NTAPI ANSI_INSTRINGNULL(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LRESULT	Result;
	LPSTR	Ansi;
	LPWSTR	Unicode;

	Ansi = (LPSTR)lParam;
	Unicode = nullptr;

	if (Ansi)
	{
		Unicode = MultiByteToWideCharInternal(Ansi);
		lParam = (LPARAM)Unicode;
	}

	Result = SendMessageW(Window, Message, wParam, lParam);
	FreeStringInternal(Unicode);
	return Result;
}

LRESULT NTAPI ANSI_OUTSTRING(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	UINT	UnicodeSize, AnsiSize;
	LPWSTR	UnicodeBuffer;
	LPSTR	AnsiBuffer;

	UnicodeSize = wParam;
	UnicodeBuffer = (LPWSTR)lParam;

	AnsiSize = UnicodeSize * sizeof(WCHAR);
	AnsiBuffer = (LPSTR)AllocateZeroedMemory(AnsiSize);
	if (AnsiBuffer == nullptr)
		return 0;

	wParam = AnsiSize;
	lParam = (LPARAM)AnsiBuffer;

	AnsiSize = SendMessageW(Window, Message, wParam, lParam);

	if (AnsiSize != 0)
		MultiByteToWideChar(CP_ACP, 0, AnsiBuffer, AnsiSize, UnicodeBuffer, UnicodeSize);

	FreeStringInternal(AnsiBuffer);

	return AnsiSize / sizeof(WCHAR);
}
//
//LRESULT NTAPI ANSI_INCNTOUTSTRING(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
//{
//	LRESULT Result;
//	MessageBoxA(NULL, "ANSI_INCNTOUTSTRING", NULL, NULL);
//	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
//	return Result;
//}
//
//LRESULT NTAPI ANSI_OUTCBOXSTRING(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
//{
//	LRESULT Result;
//	MessageBoxA(NULL, "ANSI_OUTCBOXSTRING", NULL, NULL);
//	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
//	return Result;
//}
//
//LRESULT NTAPI ANSI_GETDBCSTEXTLENGTHS(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, ULONG xpfnProc, ULONG Flags)
//{
//	LRESULT Result;
//	MessageBoxA(NULL, "ANSI_GETDBCSTEXTLENGTHS", NULL, NULL);
//	Result = OriginalNtUserMessageCall(Window, Message, wParam, lParam, xParam, xpfnProc, Flags);
//	return Result;
//}