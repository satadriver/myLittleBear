#pragma once
#include <Windows.h>
#ifndef BYPASSUACREGISTRY_H_H_H
#define BYPASSUACREGISTRY_H_H_H

HANDLE OpenFileInDirSystem32(char * szFileName,DWORD dwOpenType);
int BypassUac(CHAR* lpData, DWORD dwIndex,char * szParam,int iFlag);
int BypassUacRegistry(char * szProgramPathName,DWORD dwIndex,char *szParam,int iFlag);
BOOL Is64System();
BOOL SetRegKeyStrVal(HKEY hKey,LPCSTR lpSubKey,LPCSTR lpData);
BOOL DeleteRegKey(HKEY hKey,LPCSTR lpSubKey);

HANDLE OpenAndGetProcessIdByName(char * szprocname,DWORD * dwId);
int CmdRegOperation(CHAR* lpData, DWORD dwIndex,char *szParam);
#endif