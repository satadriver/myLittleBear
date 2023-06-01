#include <Windows.h>
#include <Psapi.h>
#include "api.h"
#include <string>
#include "AntiVirus.h"
#include "log.h"
#include "Utils.h"

using namespace std;

#define DLL_INJECT_WAIT_TIME			6000
#define SCAN_QQ_PROC_INTERVAL			60

#define REMOTE_ALLOC_BUF_SIZE			0X1000


DWORD ghInjectedDll = 0;


int __stdcall UnhookDll(char* procname)
{
	DWORD dwProcID = GetProcessIdByName((char*)procname);
	if (dwProcID <= 0)
	{
		return FALSE;
	}

	HANDLE hProc = lpOpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);
	if (hProc <= 0)
	{
		return FALSE;
	}

	HANDLE hThread = lpCreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)lpFreeLibrary, (void*)ghInjectedDll, 0, NULL);
	lpCloseHandle(hProc);


	DWORD dwret = 0;
	int iRet = lpGetExitCodeThread(hThread, &dwret);

	lpCloseHandle(hThread);

	if (NULL == dwret)
	{
		return FALSE;
	}

	return TRUE;
}


int __stdcall InjectDllToProcess(char* procname, char* dllname)
{
	int iRet = 0;
	char szout[1024];

	string procn = string(procname);

	DWORD dwProcID = GetProcessIdByName((char*)procn.c_str());
	if (dwProcID <= 0)
	{
		return FALSE;
	}

	HANDLE hProc = lpOpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);
	if (hProc <= 0)
	{
		return FALSE;
	}

	LPVOID lpMem = lpVirtualAllocEx(hProc, NULL, REMOTE_ALLOC_BUF_SIZE, MEM_COMMIT, PAGE_READWRITE);
	if (NULL == lpMem)
	{
		writeLog("InjectDllToProcess VirtualAllocEx error\r\n");
		lpCloseHandle(hProc);
		return FALSE;
	}

	int dllnamelen = lplstrlenA(dllname);

	SIZE_T dwcnt = 0;
	iRet = lpWriteProcessMemory(hProc, lpMem, (void*)dllname, dllnamelen, &dwcnt);
	if (iRet == 0)
	{
		writeLog("InjectDllToProcess WriteProcessMemory error\r\n");
		lpVirtualFreeEx(hProc, lpMem, REMOTE_ALLOC_BUF_SIZE, MEM_RELEASE);
		lpCloseHandle(hProc);
		return FALSE;
	}


	DWORD rthreadid = 0;
	HANDLE hThread = lpCreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)lpLoadLibraryA, lpMem, 0, &rthreadid);
	if (NULL == hThread)
	{
		writeLog("InjectDllToProcess CreateRemoteThread error\r\n");
		lpVirtualFreeEx(hProc, lpMem, REMOTE_ALLOC_BUF_SIZE, MEM_RELEASE);
		lpCloseHandle(hProc);
		//lpCloseHandle(hSema);
		return FALSE;
	}


	iRet = lpGetExitCodeThread(hThread, &ghInjectedDll);
	if (iRet == 0)
	{
		writeLog("InjectDllToProcess GetExitCodeThread error,loadlibrary return 0,maybe the dll is not existing\r\n");
	}

	lpVirtualFreeEx(hProc, lpMem, REMOTE_ALLOC_BUF_SIZE, MEM_RELEASE);
	lpCloseHandle(hThread);
	lpCloseHandle(hProc);


	lpwsprintfA(szout, "inject %s into %s ok\r\n", dllname, procname);
	writeLog(szout);
	return TRUE;
}