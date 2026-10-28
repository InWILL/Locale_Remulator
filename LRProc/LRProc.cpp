#include<iostream>
#include<Windows.h>
#include<detours.h>
#include<cstdio>
#include<cstring>
#include<vcclr.h>

#include"../LRCommonLibrary/LRCommonLibrary.h"
#pragma comment(lib, "LRCommonLibrary.lib")

using namespace System::Reflection;
using namespace System::Runtime::InteropServices;

#ifdef _DEBUG
#using <../LRSubMenu/bin/Debug/LRSubMenus.dll>
#else
#using <../LRSubMenu/bin/Release/LRSubMenus.dll>
#endif

const int GuidLength = 37;

// 32-bit Locale Emulator path (x86 only, opt-in via Hook32LE=true)
// Requires: LoaderDll.dll + LocaleEmulator.dll next to LRProc.exe
#ifndef _WIN64

// RTL_TIME_ZONE_INFORMATION as marshaled by the LE loader (WCHAR[32] names).
#pragma pack(push, 1)
typedef struct _LEB {
    unsigned int AnsiCodePage;
    unsigned int OemCodePage;
    unsigned int LocaleID;
    unsigned int DefaultCharset;
    unsigned int HookUILanguageAPI;
    byte DefaultFaceName[64];
    struct {
        int Bias;
        wchar_t StandardName[32];
        struct { unsigned short Year, Month, Day, Hour, Minute, Second, Milliseconds, Weekday; } StandardDate;
        int StandardBias;
        wchar_t DaylightName[32];
        struct { unsigned short Year, Month, Day, Hour, Minute, Second, Milliseconds, Weekday; } DaylightDate;
        int DaylightBias;
    } Timezone;
} LEB;
#pragma pack(pop)

// 64-bit-aligned registry redirection structs (per the LE loader's C# source).
#pragma pack(push, 8)
typedef struct { unsigned short Length; unsigned short MaximumLength; unsigned long long Buffer; } US64;
typedef struct {
    unsigned long long Root;
    US64 SubKey;
    US64 ValueName;
    unsigned int DataType;
    unsigned int _pad;
    unsigned long long Data;
    unsigned long long DataSize;
} REG_ENTRY64;
typedef struct { REG_ENTRY64 Original; REG_ENTRY64 Redirected; } REG_REDIRECT64;
#pragma pack(pop)

typedef UINT(WINAPI* LeCreateProcessFn)(
    void* leb, LPCWSTR app, LPCWSTR cmd, LPCWSTR cwd, UINT flags,
    STARTUPINFOW* si, PROCESS_INFORMATION* pi,
    LPSECURITY_ATTRIBUTES pa, LPSECURITY_ATTRIBUTES ta, LPVOID env, HANDLE token
);

static unsigned int CharsetFromCodePage(unsigned int cp)
{
    switch (cp)
    {
    case 932: return 128;   // SHIFTJIS
    case 936: return 134;   // GB2312
    case 949: return 129;   // HANGEUL
    case 950: return 136;   // BIG5
    case 1250: return 238;
    case 1251: return 204;
    case 1253: return 161;
    case 1254: return 162;
    case 1255: return 177;
    case 1256: return 178;
    case 1257: return 186;
    default:  return 0;     // ANSI
    }
}

// LEB buffer layout: [LEB 256B][regCount 8B][entries][object data]
static unsigned char* BuildLebBuffer(LRProfile* p, size_t* outSize)
{
    LEB leb;
    memset(&leb, 0, sizeof(leb));
    leb.AnsiCodePage   = p->CodePage;
    leb.OemCodePage    = p->CodePage;
    leb.LocaleID       = p->LCID;
    leb.DefaultCharset = CharsetFromCodePage(p->CodePage);
    leb.HookUILanguageAPI = 0;

    // Timezone: use the system's current bias.
    TIME_ZONE_INFORMATION tzi = {0};
    if (GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_UNKNOWN)
        leb.Timezone.Bias = tzi.Bias;

    // Registry redirection: ACP/OEMCP/InstallLanguage/Default
    wchar_t cpStr[16], lcidStr[16];
    swprintf(cpStr, 16, L"%lu", (unsigned long)p->CodePage);
    swprintf(lcidStr, 16, L"%lu", (unsigned long)p->LCID);
    const wchar_t* subKey = L"System\\CurrentControlSet\\Control\\Nls\\CodePage";
    struct { const wchar_t* name; const wchar_t* value; } entries[4] = {
        { L"InstallLanguage", lcidStr },
        { L"Default",         lcidStr },
        { L"OEMCP",           cpStr   },
        { L"ACP",             cpStr   },
    };
    const int regCount = 4;

    unsigned char* buf = (unsigned char*)calloc(1, 8192);
    memcpy(buf, &leb, sizeof(leb));                                   // 0..255
    *(unsigned long long*)(buf + 256) = (unsigned long long)regCount; // 256..263
    REG_REDIRECT64* reg = (REG_REDIRECT64*)(buf + 264);
    size_t objPos = 264 + (size_t)regCount * 128;

    for (int i = 0; i < regCount; i++)
    {
        size_t offSub, offName, offData;
        { size_t o = objPos; size_t l = wcslen(subKey) * 2;
          memcpy(buf + objPos, subKey, l); objPos += l; buf[objPos++] = 0; buf[objPos++] = 0; offSub = o; }
        { size_t o = objPos; size_t l = wcslen(entries[i].name) * 2;
          memcpy(buf + objPos, entries[i].name, l); objPos += l; buf[objPos++] = 0; buf[objPos++] = 0; offName = o; }
        { size_t o = objPos; size_t l = wcslen(entries[i].value) * 2;
          memcpy(buf + objPos, entries[i].value, l); objPos += l; buf[objPos++] = 0; buf[objPos++] = 0; offData = o; }

        REG_ENTRY64* e = &reg[i].Original;
        e->Root = 0x80000002ULL; // HKEY_LOCAL_MACHINE
        e->SubKey.Length = e->SubKey.MaximumLength = (unsigned short)(wcslen(subKey) * 2);
        e->SubKey.Buffer = (unsigned long long)offSub;
        e->ValueName.Length = e->ValueName.MaximumLength = (unsigned short)(wcslen(entries[i].name) * 2);
        e->ValueName.Buffer = (unsigned long long)offName;
        e->DataType = 1; // REG_SZ
        e->Data = (unsigned long long)offData;
        e->DataSize = (unsigned long long)(wcslen(entries[i].value) * 2);
        reg[i].Redirected = *e;
    }
    *outSize = objPos;
    return buf;
}

static int RunWithLocaleEmulator(LPCWSTR CommandLine, LRProfile* beta)
{
    HMODULE hLoader = LoadLibraryA("LoaderDll.dll");
    if (!hLoader)
    {
        MessageBox(NULL,
            TEXT("LoaderDll.dll not found. Place the Locale Emulator files\n(LoaderDll.dll + LocaleEmulator.dll) next to LRProc.exe."),
            TEXT("Locale Remulator"), MB_OK | MB_ICONERROR);
        return 1;
    }
    LeCreateProcessFn pLeCreateProcess = (LeCreateProcessFn)GetProcAddress(hLoader, "LeCreateProcess");
    if (!pLeCreateProcess)
    {
        FreeLibrary(hLoader);
        return 1;
    }

    size_t bufSize = 0;
    unsigned char* buf = BuildLebBuffer(beta, &bufSize);

    STARTUPINFOW si = {0}; si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {0};

    UINT ret = pLeCreateProcess(buf, CommandLine, CommandLine, NULL,
                                0, &si, &pi, NULL, NULL, NULL, NULL);
    free(buf);
    if (ret != 0)
    {
        FreeLibrary(hLoader);
        MessageBox(NULL, TEXT("LeCreateProcess failed."), TEXT("Locale Remulator"), MB_OK | MB_ICONERROR);
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    FreeLibrary(hLoader);
    return 0;
}

#endif // !_WIN64

#ifdef _WIN64
// x64: spawn the x86 helper (LRProc32.exe) which loads LoaderDll.dll
// and performs the actual Locale Emulator launch.
static int SpawnLeHelper(LPCWSTR lpCmdLine)
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    wchar_t* slash = wcsrchr(exePath, L'\\');
    if (!slash) return 1;
    *(slash + 1) = 0;

    wchar_t helper[MAX_PATH];
    wsprintfW(helper, L"%sLRProc32.exe", exePath);

    if (GetFileAttributesW(helper) == INVALID_FILE_ATTRIBUTES)
    {
        MessageBox(NULL,
            L"LRProc32.exe not found. Place the x86 build next to LRProc.exe\nfor Hook32LE (Locale Emulator) support.",
            L"Locale Remulator", MB_OK | MB_ICONERROR);
        return 1;
    }

    STARTUPINFOW si = {0}; si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {0};
    if (!CreateProcessW(helper, const_cast<LPWSTR>(lpCmdLine), NULL, NULL,
        FALSE, 0, NULL, NULL, &si, &pi))
        return 1;

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return (int)exitCode;
}
#endif // _WIN64

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow)
{
	LRCSharpLibrary::LRConfig::CheckConfigFile();

	if (__argc < 3)
	{
		MessageBox(
			NULL,
			TEXT(R"(
Welcome to Locale Remulator x64 command line tool.

Usage: LRProc.exe GUID Path Args

GUID	Guid of the target profile (in LRConfig.xml).
Path	Full path of the target application.
Args	Additional arguments will be passed to the application.

You can also run LREditor to use this applicaction.
			)"),
			TEXT("Locale Remulator x64"),
			NULL
		);
		return 0;
	}

	System::String^ Guid = gcnew System::String(__wargv[1]);
	System::String^ CurrentPath = LRCSharpLibrary::LRConfig::CurrentPath;

	LPWSTR cmdTail = wcsstr(lpCmdLine, __wargv[1]);
	LPWSTR targetW = cmdTail + GuidLength;   // points at the path argument

	System::String^ DllPath = System::IO::Path::Combine(CurrentPath,
		#ifdef _WIN64
		"LRHookx64.dll"
		#else
		"LRHookx32.dll"
		#endif
	);
	LPSTR dllpath = (LPSTR)(void*)Marshal::StringToHGlobalAnsi(DllPath);

	LRCSharpLibrary::LRProfile^ alpha = LRCSharpLibrary::LRConfig::GetProfile(Guid);

	LRProfile beta;
	beta.CodePage = alpha->CodePage;
	beta.LCID = alpha->LCID;
	beta.Bias = alpha->Bias; // Bias will become negative in HookGetTimeZoneInformation
	beta.HookIME = alpha->HookIME;
	beta.HookLCID = alpha->HookLCID;

	// Read Hook32LE flag from LRConfig.xml (opt-in, not in LRProfile struct)
	int useLocaleEmulator = 0;
	{
		try
		{
			System::String^ configPath = LRCSharpLibrary::LRConfig::ConfigPath;
			if (System::IO::File::Exists(configPath))
			{
				System::String^ xml = System::IO::File::ReadAllText(configPath);
				if (xml->Contains(L"<Hook32LE>true</Hook32LE>"))
					useLocaleEmulator = 1;
			}
		}
		catch (...) { }
	}

	LRConfigFileMap filemap;
	filemap.WrtieConfigFileMap(&beta);

	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	//std::cout << beta.CodePage;

#ifndef _WIN64
	if (useLocaleEmulator)
		return RunWithLocaleEmulator(targetW, &beta);
#else
	if (useLocaleEmulator)
		return SpawnLeHelper(lpCmdLine);
#endif

	DetourCreateProcessWithDllExW(NULL, targetW, NULL,
		NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&si, &pi, dllpath, NULL);

	return 0;
}
