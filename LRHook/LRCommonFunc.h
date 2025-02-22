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