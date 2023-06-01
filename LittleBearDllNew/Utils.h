#pragma once

#ifndef UTILS_H_H_H
#define UTILS_H_H_H

#include <windows.h>

int EnableDebugPrivilege(HANDLE hProc, BOOL bFlag);

int killByOpenProc(char* pname);

int taskkill(char* procname);

DWORD GetProcessIdByName(char* szProcessName);

int CreateProcessByToken(LPSTR lpTokenProcessName, LPSTR szProcessName, LPSTR szparam);

int isSystemDirectory();

int IsSystemPrivilege();

DWORD GetProcessIdByName(char* szProcessName);

HRESULT GetComFolder(int csidl, WCHAR* buffer);

DWORD isPidWow64(DWORD pid);

DWORD getIPstr(DWORD ip, char* strIP);

int adjustPrivileges();

class Uitls {

};

#endif