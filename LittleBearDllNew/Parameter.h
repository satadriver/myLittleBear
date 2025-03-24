#pragma once


#include <Windows.h>

#ifdef _DEBUG
#define VM_EVASION_DELAY	3
#else
#define VM_EVASION_DELAY	180
#endif

int ExplorerFirstStart();

int ReleaseFirstStart();

int AppProxyStart(char* szParam2);