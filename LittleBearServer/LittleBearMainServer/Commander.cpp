#include <windows.h>
#include "main.h"
#include "Public.h"
#include "PublicFunc.h"
#include "Commander.h"

#include <string>

using namespace std;

string Commander::makeCmdFileName(char* szCurrentDir, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique) {
	char strMac[MAX_PATH];
	wsprintfA(strMac, "%02X_%02X_%02X_%02X_%02X_%02X",
		lpUnique->cMAC[0], lpUnique->cMAC[1], lpUnique->cMAC[2], lpUnique->cMAC[3], lpUnique->cMAC[4], lpUnique->cMAC[5]);

	return string(szCurrentDir) + COMMAND_DIR_NAME + "\\" + strMac + "_" + cmdfn;

	// 	char szCommandFilePath[MAX_PATH] = { 0 };
	// 	lstrcpyA(szCommandFilePath, szCurrentDir);
	// 	lstrcatA(szCommandFilePath, COMMAND_DIR_NAME);
	// 	lstrcatA(szCommandFilePath, "\\");
	// 	lstrcatA(szCommandFilePath, strMac);
	// 	lstrcatA(szCommandFilePath, "_");
	// 	lstrcatA(szCommandFilePath, cmdfn);
	// 	return string(szCommandFilePath);
}


int Commander::CommandWithoutParam(int cmd, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam) {
	char szShowInfo[1024];

	char szCurrentDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurrentDir);
	lstrcatA(szCurrentDir, "\\");

	string filename = makeCmdFileName(szCurrentDir, cmdfn, lpUnique);
	HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);

		char szData[1024];

		LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)szData;
		lphdr->cmd = cmd;
		lphdr->unique = *lpUnique;
		lphdr->packlen = 0;
		int iLen = sizeof(NETWORKPACKETHEADER);

		int iRet = send(stParam.hSockClient, szData, iLen, 0);
		if (iRet <= 0)
		{
			iRet = WSAGetLastError();
			wsprintfA(szShowInfo, "CommandProc send file:%s,command:%u error code:%u\r\n", filename.c_str(), cmd, iRet);
			WriteLog(szShowInfo);
			return NETWORK_ERROR_CODE;
		}
		else
		{
			DeleteFileA(filename.c_str());
			wsprintfA(szShowInfo, "CommandProc send file:%s,command:%u ok\r\n", filename.c_str(), cmd);
			WriteLog(szShowInfo);
			return TRUE;
		}
	}
	return FALSE;
}



int Commander::CommandWithString(int cmd, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam) {
	char szShowInfo[1024];

	char szCurrentDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurrentDir);
	lstrcatA(szCurrentDir, "\\");

	string filename = makeCmdFileName(szCurrentDir, cmdfn, lpUnique);
	HANDLE hFileCmd = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFileCmd != INVALID_HANDLE_VALUE)
	{
		DWORD dwSizeHigh = 0;
		DWORD dwFileSize = GetFileSize(hFileCmd, &dwSizeHigh);
		if (dwSizeHigh || dwFileSize >= CMD_BUFFER_SIZE)
		{
			wsprintfA(szShowInfo, "command file:%s is too big\r\n", cmdfn);
			WriteLog(szShowInfo);
			CloseHandle(hFileCmd);
			DeleteFileA(filename.c_str());
		}

		DWORD dwCnt = 0;
		char szCmd[CMD_BUFFER_SIZE] = { 0 };
		int iRet = ReadFile(hFileCmd, szCmd, dwFileSize, &dwCnt, 0);
		CloseHandle(hFileCmd);


		char szData[CMD_BUFFER_SIZE];
		LPNETWORKFILEHDR lphdr = (LPNETWORKFILEHDR)szData;
		lphdr->packhdr.cmd = cmd;
		lphdr->packhdr.unique = *lpUnique;
		lphdr->packhdr.packlen = sizeof(int) + lstrlenA(szCmd);
		lphdr->origin_len = lstrlenA(szCmd);
		memmove(szData + sizeof(NETWORKFILEHDR), szCmd, lstrlenA(szCmd));
		int iLen = sizeof(NETWORKFILEHDR) + lstrlenA(szCmd);

		/*
		int iLen = 0;
		*(DWORD*)(szData + iLen) = cmd;
		iLen += sizeof(DWORD);
		*(LPUNIQUECLIENTSYMBOL)(szData + iLen ) = *lpUnique;
		iLen +=  sizeof(UNIQUECLIENTSYMBOL);
		*(DWORD*)(szData + iLen) = 0;
		int lenpos = iLen;
		iLen += sizeof(DWORD);

		*(DWORD*)(szData + iLen) = lstrlenA(szCmd);
		iLen += sizeof(DWORD);
		lstrcatA(szData +iLen,szCmd);
		iLen += lstrlenA(szCmd);

		*(DWORD*)(szData + lenpos) = sizeof(int) + lstrlenA(szCmd);
		*/

		iRet = send(stParam.hSockClient, szData, iLen, 0);
		if (iRet <= 0)
		{
			iRet = WSAGetLastError();
			wsprintfA(szShowInfo, "CommandProc send file:%s,command:%u,cmd:%s error code:%u\r\n", filename.c_str(), cmd, szCmd, iRet);
			WriteLog(szShowInfo);
			return NETWORK_ERROR_CODE;
		}
		else
		{
			DeleteFileA(filename.c_str());
			wsprintfA(szShowInfo, "CommandProc send file:%s,command:%u,cmd:%s ok\r\n", filename.c_str(), cmd, szCmd);
			WriteLog(szShowInfo);
			return TRUE;
		}
	}
	return FALSE;
}




int Commander::CommandWith2String(int cmd, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam) {
	char szShowInfo[1024];

	char szCurrentDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurrentDir);
	lstrcatA(szCurrentDir, "\\");

	string filename = makeCmdFileName(szCurrentDir, cmdfn, lpUnique);
	HANDLE hFileCmd = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFileCmd != INVALID_HANDLE_VALUE)
	{
		DWORD dwSizeHigh = 0;
		DWORD dwFileSize = GetFileSize(hFileCmd, &dwSizeHigh);
		if (dwSizeHigh || dwFileSize >= CMD_BUFFER_SIZE)
		{
			wsprintfA(szShowInfo, "command file:%s is too big\r\n", cmdfn);
			WriteLog(szShowInfo);
			CloseHandle(hFileCmd);
			DeleteFileA(filename.c_str());
		}

		DWORD dwCnt = 0;
		char szDataParam[CMD_BUFFER_SIZE] = { 0 };
		int iRet = ReadFile(hFileCmd, szDataParam, dwFileSize, &dwCnt, 0);
		CloseHandle(hFileCmd);

		char szCmd[2][CMD_BUFFER_SIZE];
		iRet = Get2CommandParams(szDataParam, szCmd, "\r\n");
		if (iRet == FALSE)
		{
			return FALSE;
		}


		char szData[CMD_BUFFER_SIZE];
		LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)szData;
		char* lp2str = (szData + sizeof(NETWORKPACKETHEADER));
		int offset = 0;
		*(DWORD*)(lp2str + offset) = lstrlenA(szCmd[0]);
		offset += sizeof(int);
		lstrcpyA(lp2str + offset, szCmd[0]);
		offset += lstrlenA(szCmd[0]);

		*(DWORD*)(lp2str + offset) = lstrlenA(szCmd[1]);
		offset += sizeof(int);
		lstrcpyA(lp2str + offset, szCmd[1]);
		offset += lstrlenA(szCmd[1]);

		/*
		LPDOUBLESTRINGPARAM lpdp = (LPDOUBLESTRINGPARAM)(szData + sizeof(NETWORKPACKETHEADER));
		lpdp->name1len = lstrlenA(szCmd[0]);
		lstrcpyA(lpdp->name1 ,szCmd[0]);
		lpdp->name2len = lstrlenA(szCmd[1]);
		lstrcpyA(lpdp->name2 ,szCmd[1]);
		*/

		lphdr->cmd = cmd;
		lphdr->unique = *lpUnique;
		lphdr->packlen = sizeof(int) + sizeof(int) + lstrlenA(szCmd[0]) + lstrlenA(szCmd[1]);

		int iLen = sizeof(NETWORKPACKETHEADER) + lphdr->packlen;


		iRet = send(stParam.hSockClient, szData, iLen, 0);
		if (iRet <= 0)
		{
			iRet = WSAGetLastError();
			wsprintfA(szShowInfo, "CommandProc send file:%s cmd[0]:%s cmd[1]:%s error code:%u\r\n", filename.c_str(), szCmd[0], szCmd[1], iRet);
			WriteLog(szShowInfo);
			return NETWORK_ERROR_CODE;
		}
		else
		{
			DeleteFileA(filename.c_str());
			wsprintfA(szShowInfo, "CommandProc send file:%s cmd[0]:%s cmd[1]:%s ok\r\n", filename.c_str(), szCmd[0], szCmd[1]);
			WriteLog(szShowInfo);
			return TRUE;
		}
	}
	return FALSE;
}


int Commander::SendCmdPacket(SOCKET s, int cmd) {
	return 0;
}




int Commander::RecvCmdPacket(SOCKET s) {

	char szData[1024];
	int iRet = recv(s, szData, 1024, 0);
	LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)szData;
	if (lphdr->packlen != 0 || iRet != sizeof(NETWORKPACKETHEADER))
	{
		return FALSE;
	}
	return lphdr->cmd;
}






int Commander::SendCmdPacket(LPUNIQUECLIENTSYMBOL lpUnique, SOCKET s, char* lpdata, int datasize, int cmd) {

	LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)lpdata;
	lphdr->cmd = cmd;
	lphdr->unique = *lpUnique;
	lphdr->packlen = 0;
	int iLen = sizeof(NETWORKPACKETHEADER);

	int iRet = send(s, lpdata, iLen, 0);
	if (iRet <= 0)
	{
		return FALSE;
	}

	return iRet;
}

