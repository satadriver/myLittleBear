#pragma once

#ifndef BOOTDESKTOP_H_H_H
#define BOOTDESKTOP_H_H_H

#include <windows.h>

DWORD OpenRecycle();

DWORD HideDesktopRecycle();

DWORD HideDesktopMyComputer();

DWORD OpenMyComputer();

DWORD HideDesktopIE();

DWORD __stdcall lnkDesktop(char* szDstPePath, char* szSysDir, char* strUserName, char* strDataPath);

DWORD createDesktopLnk(wchar_t* strCommand, wchar_t* strArguments, wchar_t* strDataPath, wchar_t* strIcon, wchar_t* strLnkName);

#endif