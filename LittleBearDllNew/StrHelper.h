#pragma once

#ifndef STRINGHELPER_H_H_H
#define STRINGHELPER_H_H_H

#include <windows.h>
#include <vector>
#include <iostream>

using namespace std;

#define MAX_WSPRINTF_BUFFER_SIZE 1024




vector<string> splitstr(string str, string sep);

void back2forwardSlash(string& str);

void back2forwardPlus(string& str);

unsigned int getIpFromStr(char* strip);

int strTrim(char* data);

int GBKToUTF8(const char* gb2312, char** lpdatabuf);

int UTF8ToGBK(const char* utf8, char** lpdatabuf);

char* _HexStrStr(char  Src[], int SrcLen, char Dst[], int DstLen);
int _wcscmp(const wchar_t* pSrc, const wchar_t* pDst);
int __stdcall _MylstrlenA(char* pData);
char* _strstr(char* Src, char* Dst);

int makeFullFileName(char* strModuleFileName, char* strName);

int MStringToWString(const char* lpcstr, int srclen, wchar_t* lpwstr, int dstlen);
int WStringToMString(const wchar_t* lpcwstr, int wstrlen, char* lpstr, int strlen);

time_t StringToDatetime(char* str);

int GetPathFromFullName(char* strFullName, char* strDst);
int GetNameFromFullName(char* strFullName, char* dstname);
int GetMainNameFormName(char* szName, char* szMainName);

string makeJsonFromTxt(char* utf);

#endif