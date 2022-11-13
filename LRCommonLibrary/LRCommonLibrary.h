#pragma once
#include<Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include<cstring>
#include<iostream>
#include<fstream>

const TCHAR szConfigFileMap[] = TEXT("LRConfigFileMap");
//const TCHAR szMsg[] = TEXT("Message from first process.");

struct LRProfile
{
	UINT CodePage;
	UINT LCID;
	int HookIME;
	int HookLCID;
	char lfFaceName[LF_FACESIZE] = "None";
};

const int BUF_SIZE = sizeof(LRProfile);

class LRConfigFileMap
{
public:
	int WrtieConfigFileMap(LRProfile *profile);
	int ReadConfigFileMap(LRProfile* profile);
	void FreeConfigFileMap();
private:
	HANDLE hMapFile;
	LRProfile *pBuf;
};
