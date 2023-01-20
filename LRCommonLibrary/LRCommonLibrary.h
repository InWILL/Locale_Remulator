#pragma once
#include<Windows.h>
#include<stdio.h>
#include<conio.h>
#include<tchar.h>
#include<cstring>
#include<iostream>
#include<fstream>

struct LRProfile
{
	UINT CodePage;
	UINT LCID;
	long Bias;
	int HookIME;
	int HookLCID;
};

const int BUF_SIZE = sizeof(LRProfile);

class LRConfigFileMap
{
public:
	int WrtieConfigFileMap(LRProfile *profile);
	int ReadConfigFileMap(LRProfile* profile);
};
