

#include <Windows.h>
#include <stdio.h>
#include "api.h"
#include "StrHelper.h"
#include "log.h"
#include "public.h"


int __cdecl log(const WCHAR* format, ...) {
	WCHAR szbuf[2048];

	va_list   pArgList;

	va_start(pArgList, format);

	int nByteWrite = vswprintf_s(szbuf, sizeof(szbuf) / sizeof(WCHAR), format, pArgList);

	va_end(pArgList);

	OutputDebugStringW(szbuf);

	return nByteWrite;
}


int __cdecl log(const CHAR* format, ...) {
	CHAR szbuf[2048];

	va_list   pArgList;

	va_start(pArgList, format);

	int nByteWrite = vsprintf_s(szbuf, sizeof(szbuf), format, pArgList);

	va_end(pArgList);

	OutputDebugStringA(szbuf);

	return nByteWrite;
}


int writeRawLog(char* pLog)
{
	char strFilePath[MAX_PATH] = { 0 };
	int iRet = makeFullFileName(strFilePath, LITTLEBEAR_LOG_FILE_NAME);
	HANDLE hFile = CreateFileA(strFilePath, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	iRet = SetFilePointer(hFile, 0, 0, FILE_END);

	char szTime[MAX_PATH];
	SYSTEMTIME stTime = { 0 };
	GetLocalTime(&stTime);
	wsprintfA(szTime, "%u/%u/%u %d:%d:%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	DWORD dwCnt = 0;
	char szMsg[MAX_WSPRINTF_BUFFER_SIZE];
	int iLen = wsprintfA(szMsg, "%s %s", szTime, pLog);

	iRet = WriteFile(hFile, szMsg, iLen, &dwCnt, 0);
	CloseHandle(hFile);
	if (iRet == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

/*
int writeLog(char* pLog)
{
	char strFilePath[MAX_PATH] = { 0 };
	int iRet = makeFullFileName(strFilePath, LITTLEBEAR_LOG_FILE_NAME);

	HANDLE hFile = lpCreateFileA(strFilePath, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		int iError = lpRtlGetLastWin32Error();
		if (iError == 4 || iError == 5)
		{
			return FALSE;
		}
	}

	iRet = lpSetFilePointer(hFile, 0, 0, FILE_END);

	char szTime[MAX_PATH];
	SYSTEMTIME stTime;
	lpGetLocalTime(&stTime);
	lpwsprintfA(szTime, "%u/%u/%u %d:%d:%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	DWORD dwCnt;
	char szMsg[MAX_WSPRINTF_BUFFER_SIZE];
	int iLen = lpwsprintfA(szMsg, "%s %s", szTime, pLog);

	int iRes = lpWriteFile(hFile, szMsg, iLen, &dwCnt, 0);
	lpCloseHandle(hFile);
	if (iRes == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}
*/

int __cdecl writeLog(char* format, ...)
{
	CHAR szbuf[2048];

	va_list   pArgList;

	va_start(pArgList, format);

	const char* symbol = "[liujinguang] ";
	lstrcpyA(szbuf, symbol);

	int symbol_len = lstrlenA(symbol);

	int nByteWrite = vsprintf_s(szbuf + symbol_len, sizeof(szbuf) - symbol_len, format, pArgList);

	va_end(pArgList);

	OutputDebugStringA(szbuf);

	char strFilePath[MAX_PATH] = { 0 };
	int iRet = makeFullFileName(strFilePath, LITTLEBEAR_LOG_FILE_NAME);

	HANDLE hFile = lpCreateFileA(strFilePath, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		int iError = lpRtlGetLastWin32Error();
		if (iError == 4 || iError == 5)
		{
			return FALSE;
		}
	}

	iRet = lpSetFilePointer(hFile, 0, 0, FILE_END);

	char szTime[MAX_PATH];
	SYSTEMTIME stTime;
	lpGetLocalTime(&stTime);
	lpwsprintfA(szTime, "%u/%u/%u %d:%d:%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	DWORD dwCnt;
	char szMsg[MAX_WSPRINTF_BUFFER_SIZE];
	int iLen = lpwsprintfA(szMsg, "%s %s", szTime, szbuf);

	int iRes = lpWriteFile(hFile, szMsg, iLen, &dwCnt, 0);
	lpCloseHandle(hFile);
	if (iRes == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}