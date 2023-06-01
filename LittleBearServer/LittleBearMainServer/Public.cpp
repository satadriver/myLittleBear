
#include <windows.h>
#include <iostream>
#include <time.h>
#include "main.h"
#include "Public.h"
#include <Shlwapi.h>
#include "mysqlOper.h"
#include <Dbghelp.h>

#pragma comment( lib, "Shlwapi.lib")
#pragma comment(lib,"dbghelp.lib")



using namespace std;

string getformatdt() {
	SYSTEMTIME sttime = { 0 };
	GetLocalTime(&sttime);
	char sztime[256] = { 0 };
	wsprintfA(sztime, "%u-%u-%u %u:%u:%u", sttime.wYear, sttime.wMonth, sttime.wDay, sttime.wHour, sttime.wMinute, sttime.wSecond);
	return string(sztime);
}


void replaceJsonSlash(string& str) {
	int len = str.length();
	for (int i = 0; i < len - 2; i++)
	{
		if (memcmp(str.c_str() + i, "\\\\", 2) == 0)
		{
			str.at(i) = '\\';
			str.at(i + 1) = '/';
		}
	}
}


void replaceSlash(string& str) {
	int len = str.length();
	for (int i = 0; i < len; i++)
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
		}
	}
}

void replaceSlashAndEnter(string& str) {

	for (int i = 0; i < str.length(); )
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
			i++;
		}
		else if (str.at(i) == '\r' || str.at(i) == '\n')
		{
			str = str.replace(i, 1, "");
		}
		else {
			i++;
		}
	}
}


void replaceSlashAndEnterAndQuot(string& str) {

	for (int i = 0; i < str.length(); )
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
			i++;
		}
		else if (str.at(i) == '\''/*|| str.at(i) == ':'*/)
		{
			str = str.replace(i, 1, "\\\'");
			i += 2;
		}
		else if (str.at(i) == '\"')
		{
			str = str.replace(i, 1, "\\\"");
			i += 2;
		}
		else if (str.at(i) == '\r' || str.at(i) == '\n' || str.at(i) == '\t' || str.at(i) == '\b')
		{
			str = str.replace(i, 1, "");
		}
		else {
			i++;
		}
	}
}


int StandardFormatMacString(char* strMac, int len, LPUNIQUECLIENTSYMBOL lpUnique) {
	if (len < 2 * 6 + 5 + 1)
	{
		return FALSE;
	}
	wsprintfA(strMac, "%02X_%02X_%02X_%02X_%02X_%02X",
		lpUnique->cMAC[0], lpUnique->cMAC[1], lpUnique->cMAC[2], lpUnique->cMAC[3], lpUnique->cMAC[4], lpUnique->cMAC[5]);
	return TRUE;
}



int MStringToWString(const char* lpcstr, int srclen, wchar_t* lpwstr, int dstlen)
{
	int len = MultiByteToWideChar(CP_ACP, 0, lpcstr, srclen, (LPWSTR)lpwstr, dstlen);
	lpwstr[len] = 0;
	return len;
}



int WStringToMString(const wchar_t* lpcwstr, int wstrlen, char* lpstr, int strlen)
{
	int len = WideCharToMultiByte(CP_ACP, 0, lpcwstr, wstrlen, lpstr, strlen, NULL, NULL);
	lpstr[len] = 0;
	return len;
}

int UTF8ToGBK(const char* utf8, char** lpdatabuf)
{
	if (lpdatabuf <= 0)
	{
		return FALSE;
	}
	int needunicodelen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	if (needunicodelen <= 0)
	{
		//WriteLogFile("UTF8ToGBK MultiByteToWideChar get len error\r\n");
		*lpdatabuf = 0;
		return FALSE;
	}

	needunicodelen += 1024;
	wchar_t* wstr = new wchar_t[needunicodelen];
	memset(wstr, 0, 2 * (needunicodelen));
	int unicodelen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, needunicodelen);
	if (unicodelen <= 0)
	{
		//WriteLogFile("UTF8ToGBK MultiByteToWideChar error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	*(int*)(wstr + unicodelen) = 0;
	int needgbklen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (needgbklen <= 0)
	{
		//WriteLogFile("UTF8ToGBK WideCharToMultiByte get len error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	needgbklen += 1024;
	*lpdatabuf = new char[needgbklen];
	memset(*lpdatabuf, 0, needgbklen);

	int gbklen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, *lpdatabuf, needgbklen, NULL, NULL);
	delete[] wstr;
	if (gbklen <= 0)
	{
		//WriteLogFile("UTF8ToGBK WideCharToMultiByte error\r\n");
		delete[](*lpdatabuf);
		*lpdatabuf = 0;
		return FALSE;
	}

	*(*lpdatabuf + gbklen) = 0;
	return gbklen;
}



int GBKToUTF8(const char* gb2312, char** lpdatabuf)
{
	if (lpdatabuf <= 0)
	{
		return FALSE;
	}
	int needunicodelen = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	if (needunicodelen <= 0)
	{
		//WriteLogFile("GBKToUTF8 MultiByteToWideChar get len error\r\n");
		*lpdatabuf = 0;
		return FALSE;
	}

	needunicodelen += 1024;
	wchar_t* wstr = new wchar_t[needunicodelen];
	memset(wstr, 0, 2 * (needunicodelen));
	int unicodelen = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, needunicodelen);
	if (unicodelen <= 0)
	{
		//WriteLogFile("GBKToUTF8 MultiByteToWideChar error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	*(int*)(wstr + unicodelen) = 0;
	int needutf8len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (needutf8len <= 0)
	{
		//WriteLogFile("GBKToUTF8 WideCharToMultiByte get len error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	needutf8len += 1024;
	*lpdatabuf = new char[needutf8len];
	memset(*lpdatabuf, 0, needutf8len);
	int utf8len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, *lpdatabuf, needutf8len, NULL, NULL);
	delete[] wstr;
	if (utf8len <= 0)
	{
		//WriteLogFile("GBKToUTF8 WideCharToMultiByte error\r\n");
		delete* lpdatabuf;
		*lpdatabuf = 0;
		return FALSE;
	}

	*(*lpdatabuf + utf8len) = 0;
	return utf8len;
}



DWORD ErrorFormat(LPNETWORKPROCPARAM lpparam, char* error, char* prefix) {
	SYSTEMTIME systime = { 0 };
	GetLocalTime(&systime);
	char sztime[MAX_PATH];
	wsprintfA(sztime, "%04u_%02u_%2u %02u_%02u_%02u", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
	unsigned char* pip = (unsigned char*)&lpparam->stAddrClient.sin_addr.S_un.S_addr;
	wsprintfA(error, "%s %s,GetLastError:%u,WSAGetLastError:%u,socket:%u,ip:%u_%u_%u_%u,port:%u\r\n",
		sztime, prefix, GetLastError(), WSAGetLastError(), lpparam->hSockClient, pip[0], pip[1], pip[2], pip[3], ntohs(lpparam->stAddrClient.sin_port));
	return TRUE;
}



DWORD InetAddrFormatString(NETWORKPROCPARAM stParam, char* pBuf)
{
	unsigned char* pIP = (unsigned char*)&stParam.stAddrClient.sin_addr.S_un.S_addr;
	int iLen = wsprintfA(pBuf, "IP:%u.%u.%u.%u,Port:%u,socket:%u", pIP[0], pIP[1], pIP[2], pIP[3], ntohs(stParam.stAddrClient.sin_port), stParam.hSockClient);
	return iLen;
}




int GetPathFromFullName(char* strFullName, char* strDst)
{
	int srclen = lstrlenA(strFullName);
	for (unsigned int i = srclen; i > 0; i--)
	{
		if (strFullName[i] == '\\')
		{
			RtlMoveMemory(strDst, strFullName, i + 1);
			*(strDst + i + 1) = 0;
			return i + 1;
		}
	}
	return FALSE;
}

int GetMainNameFormName(char* szName, char* szMainName)
{
	for (int i = 0; i < lstrlenA(szName); i++)
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
	for (int i = lstrlenA(strFullName); i > 0; i--)
	{
		if (strFullName[i] == 0x5c)
		{
			lstrcpyA(dstname, strFullName + i + 1);
			return TRUE;
		}
		else {

		}
	}

	return FALSE;
}

int __cdecl WriteLog(char* format, ...)
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

	HANDLE hFile = CreateFileA(LOGFILENAME, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		int iError = GetLastError();
		if (iError == 4 || iError == 5)
		{
			return FALSE;
		}
	}

	int iRet = SetFilePointer(hFile, 0, 0, FILE_END);

	char szTime[MAX_PATH];
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	wsprintfA(szTime, "%u/%u/%u %d:%d:%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	DWORD dwCnt;
	char szMsg[MAX_WSPRINTF_BUFFER_SIZE];
	int iLen = wsprintfA(szMsg, "%s %s", szTime, szbuf);

	int iRes = WriteFile(hFile, szMsg, iLen, &dwCnt, 0);
	CloseHandle(hFile);
	if (iRes == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

int WriteLog_old(char* pLog)
{
	HANDLE hFile = CreateFileA(LOGFILENAME, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int iRet = SetFilePointer(hFile, 0, 0, FILE_END);

	char szTime[MAX_PATH];
	SYSTEMTIME stTime = { 0 };
	GetLocalTime(&stTime);
	wsprintfA(szTime, "%u-%u-%u %d:%d:%d ", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	DWORD dwCnt = 0;
	char szMsg[MAX_WSPRINTF_BUFFER_SIZE];
	int iLen = wsprintfA(szMsg, "%s %s", szTime, pLog);

	iRet = WriteFile(hFile, szMsg, iLen, &dwCnt, 0);
	CloseHandle(hFile);
	if (iRet == FALSE || dwCnt != iLen)
	{

		return FALSE;
	}

	return TRUE;
}






int WriteDataFileWithType(char* szLogFileName, char* pLog, int iSize, int iMode)
{
	HANDLE hFile = CreateFileA(szLogFileName, GENERIC_WRITE, 0, 0, iMode, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		int iRet = GetLastError();
		if (iRet == 2 || iRet == 3)
		{
			char szDstFilePath[MAX_PATH];
			GetPathFromFullName(szLogFileName, szDstFilePath);
			iRet = MakeSureDirectoryPathExists(szDstFilePath);
			if (iRet)
			{
				hFile = CreateFileA(szLogFileName, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile == INVALID_HANDLE_VALUE)
				{
					return FALSE;
				}
			}
			else {
				return FALSE;
			}
		}
		else {
			return FALSE;
		}
	}

	int iRet = SetFilePointer(hFile, 0, 0, FILE_END);
	DWORD dwCnt = 0;
	iRet = WriteFile(hFile, pLog, iSize, &dwCnt, 0);
	CloseHandle(hFile);
	if (iRet == FALSE || dwCnt != iSize)
	{

		return FALSE;
	}

	return TRUE;
}



int WriteDataFile(char* szFileName, char* pData, int iDataSize)
{
	HANDLE hFile = CreateFileA(szFileName, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		int iRet = GetLastError();
		if (iRet == 2 || iRet == 3)
		{
			char szDstFilePath[MAX_PATH];
			// 			lstrcpyA(szDstFilePath,szFileName);
			// 			PathStripPathA(szDstFilePath);
			GetPathFromFullName(szFileName, szDstFilePath);
			iRet = MakeSureDirectoryPathExists(szDstFilePath);
			if (iRet)
			{
				hFile = CreateFileA(szFileName, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile == INVALID_HANDLE_VALUE)
				{
					return FALSE;
				}
			}
			else {
				return FALSE;
			}
		}
	}

	int iRet = SetFilePointer(hFile, 0, 0, FILE_END);
	DWORD dwCnt = 0;
	iRet = WriteFile(hFile, pData, iDataSize, &dwCnt, 0);
	CloseHandle(hFile);
	if (iRet == FALSE || dwCnt != iDataSize)
	{
		return FALSE;
	}

	return TRUE;
}


int CheckIfProgramExist()
{
	HANDLE hMutex = CreateMutexA(NULL, TRUE, SERVER_MUTEX_NAME);
	DWORD dwRet = GetLastError();
	if (hMutex)
	{
		if (ERROR_ALREADY_EXISTS == dwRet)
		{
			WriteLog("mutex already exist,please shutdown the program and run one instance\r\n");
			CloseHandle(hMutex);
			return TRUE;
		}
		else if (dwRet == 0) {
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else {
		WriteLog("mutex create error\r\n");
		return TRUE;
	}
}




DWORD GetIPFromConfigFile()
{
	HANDLE hFile = CreateFileA(IP_CONFIG_FILE, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwFileSizeHigh = 0;
	int iFileSize = GetFileSize(hFile, &dwFileSizeHigh);
	char szIp[256] = { 0 };
	DWORD dwCnt = 0;
	int iRet = ReadFile(hFile, szIp, iFileSize, &dwCnt, 0);
	CloseHandle(hFile);
	if (iRet == FALSE || dwCnt != iFileSize)
	{
		iRet = GetLastError();
		return FALSE;
	}

	DWORD dwLocalIP = inet_addr(szIp);

	// 	DWORD dwLocalIP = 0;
	// 	hostent * pHostent = gethostbyname(szIp);		//get ip from host name
	// 	if (pHostent) {
	// 		ULONG  pPIp = *(DWORD*)((CHAR*)pHostent + sizeof(hostent) - sizeof(DWORD_PTR));
	// 		ULONG  pIp = *(ULONG*)pPIp;
	// 		dwLocalIP = *(DWORD*)pIp;
	// 	}
	// 	else {
	// 		MessageBoxA(0, "ip set error", "ip error", MB_OK);
	// 	}

	return dwLocalIP;
}




DWORD __stdcall OpenFireWallPort()
{
	//int iRet = WinExec("cmd /c net stop mpssvc",SW_HIDE);
	//int iRet = WinExec("cmd /c netsh advfirewall set privateprofile state off",SW_HIDE);
	//iRet = WinExec("cmd /c netsh advfirewall set publicprofile state off",SW_HIDE);

	char szCmd[MAX_PATH];
	wsprintfA(szCmd, "netsh advfirewall firewall add rule name=\"%d\" protocol=TCP dir=in localport=%u action=allow", NETWORK_DATAPORT, NETWORK_DATAPORT);
	//wsprintfA(szCmd,"netsh firewall set portopening TCP %u %u",NETWORK_DATAPORT,NETWORK_DATAPORT);
	int iRet = WinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		WriteLog("open firewall port error\r\n");
		return FALSE;
	}

	// 	wsprintfA(szCmd,"netsh firewall set portopening TCP %u ENABLE",NETWORK_DATAPORT_OLD);
	// 	iRet = WinExec(szCmd,SW_HIDE);
	// 	if (iRet <= 31)
	// 	{
	// 		WriteLog("open firewall port error\r\n");
	// 		return FALSE;
	// 	}

	//netsh advfirewall firewall add rule name="Netbios-ssn" protocol=TCP dir=in localport=139 action=allow 
	wsprintfA(szCmd, "netsh advfirewall firewall add rule name=\"%d\" protocol=TCP dir=in localport=%u action=allow", NETWORK_CMDPORT, NETWORK_CMDPORT);
	//wsprintfA(szCmd,"netsh firewall set portopening TCP %u %u",NETWORK_CMDPORT, NETWORK_CMDPORT);
	iRet = WinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		WriteLog("open firewall port error\r\n");
		return FALSE;
	}

	wsprintfA(szCmd, "netsh advfirewall firewall add rule name=\"%d\" protocol=TCP dir=in localport=%u action=allow", REMOTE_CONTROLPORT, REMOTE_CONTROLPORT);
	//wsprintfA(szCmd,"netsh firewall set portopening TCP %u %u",REMOTE_CONTROLPORT, REMOTE_CONTROLPORT);
	iRet = WinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		WriteLog("open firewall port error\r\n");
		return FALSE;
	}

	wsprintfA(szCmd, "netsh advfirewall firewall add rule name=\"%d\" protocol=TCP dir=in localport=%u action=allow", DOWNLOADTROJAN_PORT, DOWNLOADTROJAN_PORT);
	//wsprintfA(szCmd,"netsh firewall set portopening TCP %u %u",DOWNLOADTROJAN_PORT, DOWNLOADTROJAN_PORT);
	iRet = WinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		WriteLog("open firewall port error\r\n");
		return FALSE;
	}

	wsprintfA(szCmd, "netsh advfirewall firewall add rule name=\"%d\" protocol=TCP dir=in localport=%u action=allow", DOWNLOADPROGRAM_PORT, DOWNLOADPROGRAM_PORT);
	//wsprintfA(szFireWallPortOpenCmd,"cmd /c netsh firewall set portopening TCP %u %u >> cmdresult.txt",DOWNLOADPROGRAM_PORT, DOWNLOADPROGRAM_PORT);
	iRet = WinExec(szCmd, SW_HIDE);
	return TRUE;
}






