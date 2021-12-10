#pragma once
#include<Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include<iostream>

#define BUF_SIZE 256
const TCHAR szConfigFileMap[] = TEXT("LRConfigFileMap");
const UINT szMsg = 932;

struct LRProfile
{
	UINT CodePage;
};

class LRConfigFileMap
{
public:
	int WrtieConfigFileMap();
	int ReadConfigFileMap();
	void FreeConfigFileMap();
private:
	HANDLE hMapFile;
	UINT *pBuf;
};
