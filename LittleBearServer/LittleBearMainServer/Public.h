

#pragma once
#ifndef PUBLIC_H_H_H
#define PUBLIC_H_H_H


#include <Windows.h>
#include "main.h"
#include <iostream>

using namespace std;

void replaceJsonSlash(string& str);

void replaceSlash(string&);

void replaceSlashAndEnter(string& str);

void replaceSlashAndEnterAndQuot(string& str);

string getformatdt();

int StandardFormatMacString(char* strMac, int len, LPUNIQUECLIENTSYMBOL lpUnique);

DWORD __stdcall OpenFireWallPort();

int GBKToUTF8(const char* gb2312, char** lpdatabuf);

int UTF8ToGBK(const char* utf8, char** lpdatabuf);

DWORD ErrorFormat(LPNETWORKPROCPARAM lpparam, char* error, char* prefix);

DWORD InetAddrFormatString(NETWORKPROCPARAM stParam, char* pBuf);

DWORD GetIPFromConfigFile();

int CheckIfProgramExist();

int GetPathFromFullName(char* strFullName, char* strDst);

int WriteLog_old(char* pLog);

int __cdecl WriteLog(char* format, ...);

int WriteDataFile(char* szFileName, char* pData, int iDataSize);

int WriteDataFileWithType(char* szLogFileName, char* pLog, int iSize, int iMode);





#endif