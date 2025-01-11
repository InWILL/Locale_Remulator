#include "LRCommonLibrary.h"

//Create File Map to share memory
//https://docs.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory
int LRConfigFileMap::WrtieConfigFileMap(LRProfile *profile)
{
	SetEnvironmentVariableW(L"LRCodePage", (LPCWSTR)&profile->CodePage);
	SetEnvironmentVariableW(L"LRLCID", (LPCWSTR)&profile->LCID);
	SetEnvironmentVariableW(L"LRBIAS", (LPCWSTR)&profile->Bias);
	SetEnvironmentVariableW(L"LRHookIME", (LPCWSTR)&profile->HookIME);
	SetEnvironmentVariableW(L"LRHookLCID", (LPCWSTR)&profile->HookLCID);
	SetEnvironmentVariableW(L"LRLocation", (LPCWSTR)&profile->Location);
	return 0;
}

int LRConfigFileMap::ReadConfigFileMap(LRProfile* profile)
{
	GetEnvironmentVariableW(L"LRCodePage", (LPWSTR)&profile->CodePage, sizeof(UINT));
	GetEnvironmentVariableW(L"LRLCID", (LPWSTR)&profile->LCID, sizeof(UINT));
	GetEnvironmentVariableW(L"LRBIAS", (LPWSTR)&profile->Bias, sizeof(long));
	GetEnvironmentVariableW(L"LRHookIME", (LPWSTR)&profile->HookIME,sizeof(int));
	GetEnvironmentVariableW(L"LRHookLCID", (LPWSTR)&profile->HookLCID, sizeof(int));
	wchar_t buffer[LOCALE_NAME_MAX_LENGTH];
	GetEnvironmentVariableW(L"LRLocation", buffer, LOCALE_NAME_MAX_LENGTH);
	profile->Location = buffer;
	return 0;
}