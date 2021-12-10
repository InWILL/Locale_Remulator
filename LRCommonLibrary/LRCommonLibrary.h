#pragma once
#include<Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include<iostream>

#define BUF_SIZE 256
const TCHAR szConfigFileMap[] = TEXT("LRConfigFileMap");
//const UINT szMsg = 932;

struct LRProfile
{
	UINT CodePage;
};

static LRProfile settings;

class LRConfigFileMap
{
public:
	int WrtieConfigFileMap(LRProfile *profile);
	int ReadConfigFileMap();
	void FreeConfigFileMap();
private:
	HANDLE hMapFile;
	LRProfile* pBuf;
};
