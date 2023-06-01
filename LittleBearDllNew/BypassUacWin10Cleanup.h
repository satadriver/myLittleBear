#pragma once
#include <windows.h>
#include "PublicVar.h"
#ifndef BYPASSUACWIN10CLEANUP_H_H_H
#define BYPASSUACWIN10CLEANUP_H_H_H

DWORD WINAPI Win10CleanupFolderWatcher(LPVOID lpstData);
DWORD __stdcall BypassUacWin10Cleanup(LPVOID lpCpuBits);
#endif
