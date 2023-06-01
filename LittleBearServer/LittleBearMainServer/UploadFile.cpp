#include "main.h"
#include <windows.h>
#include "Public.h"
#include "Shlwapi.h"

#include "UploadFile.h"
#include "Commander.h"
#include "UpDownResult.h"
#include "FileHelper.h"
#include "mysqlOper.h"
#include "FileReadLine.h"
#include "PublicFunc.h"

#include <string>

#pragma comment( lib, "Shlwapi.lib")

using namespace std;

int UploadFiles::UploadFile(int cmd, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam, char* lpUploadBuf, int bufsize) {
	char szShowInfo[1024];
	char szClientInfo[MAX_PATH];
	int iRet = 0;

	char szCurrentDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurrentDir);
	lstrcatA(szCurrentDir, "\\");

	string cmdfp = Commander::makeCmdFileName(szCurrentDir, cmdfn, lpUnique);

	int utf8fnlen = 0;
	char* utf8fn = 0;
	iRet = FileHelper::fileReader(cmdfp.c_str(), &utf8fn, &utf8fnlen);
	DeleteFileA(cmdfp.c_str());
	if (iRet <= 0)
	{
		return FALSE;
	}

	string gbkfn = makeJsonFromTxt(utf8fn);
	delete utf8fn;

	LPNETWORKFILEHDR lphdr = (LPNETWORKFILEHDR)lpUploadBuf;
	lphdr->packhdr.cmd = cmd;
	lphdr->packhdr.unique = *lpUnique;
	lphdr->packhdr.packlen = sizeof(int) + gbkfn.length();
	lphdr->len = gbkfn.length();
	memmove(lpUploadBuf + sizeof(NETWORKFILEHDR), gbkfn.c_str(), gbkfn.length());
	int sendpacklen = sizeof(NETWORKFILEHDR) + gbkfn.length();

	iRet = send(stParam.hSockClient, lpUploadBuf, sendpacklen, 0);
	if (iRet <= 0)
	{
		wsprintfA(szShowInfo, "CommandProc uploadfile send file name:%s error code:%u\r\n", gbkfn.c_str(), WSAGetLastError());
		WriteLog(szShowInfo);
		return NETWORK_ERROR_CODE;
	}


	int recvsize = recv(stParam.hSockClient, lpUploadBuf, bufsize, 0);
	if (recvsize < sizeof(NETWORKPACKETHEADER))
	{
		InetAddrFormatString(stParam, szClientInfo);
		wsprintfA(szShowInfo, "CommandProc uploadfile:%s recv error code:%u,client:%s\r\n", gbkfn.c_str(), GetLastError(), szClientInfo);
		WriteLog(szShowInfo);
		return NETWORK_ERROR_CODE;
	}

	LPNETWORKPACKETHEADER lpnewhdr = (LPNETWORKPACKETHEADER)(lpUploadBuf);
	UNIQUECLIENTSYMBOL newunique = (lpnewhdr->unique);
	if (memcmp(newunique.cMAC, lpUnique->cMAC, MAC_ADDRESS_LENGTH) || lpUnique->dwVolumeNO != newunique.dwVolumeNO)
	{
		wsprintfA(szShowInfo, "CommandProc uploadfile mac or volumeNO is not same,client volumeNO:%u,recved volumeNO:%u\r\n",
			lpUnique->dwVolumeNO, lpnewhdr->unique.dwVolumeNO);
		WriteLog(szShowInfo);
		return NETWORK_ERROR_CODE;
	}

	int uploadsize = lpnewhdr->packlen;
	if (uploadsize < 0 || uploadsize >= MAX_BUF_SIZE)
	{
		wsprintfA(szShowInfo, "CommandProc uploadfile:%s,size:%u error\r\n", gbkfn.c_str(), uploadsize);
		WriteLog(szShowInfo);
		return NETWORK_ERROR_CODE;
	}

	int dwCommand = lpnewhdr->cmd;
	if (dwCommand == FILE_TRANSFER_NOT_FOUND)
	{
		wsprintfA(szShowInfo, "upload file:%s from client error,because the file is not found\r\n", gbkfn.c_str());
		WriteLog(szShowInfo);
		return FALSE;
	}
	else if (dwCommand == FILE_TRANSFER_TOO_BIG)
	{
		wsprintfA(szShowInfo, "upload file:%s from client error,because the file size:%u is too big to transfer\r\n", gbkfn.c_str(), uploadsize);
		WriteLog(szShowInfo);
		return FALSE;
	}
	else if (dwCommand == FILE_TRANSFER_ZERO)
	{
		wsprintfA(szShowInfo, "upload file:%s from client error,because the file size is 0\r\n", gbkfn.c_str());
		WriteLog(szShowInfo);
		return FALSE;
	}
	else if (dwCommand == UPLOADFILE)
	{
		char szfn[MAX_PATH] = { 0 };
		lstrcpyA(szfn, gbkfn.c_str());
		PathStripPathA(szfn);

		string localfn = string(szCurrentDir) + UPLOAD_FILE_DIR_NAME + "\\" + szfn;

		HANDLE hfilenew = CreateFileA(localfn.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (hfilenew == INVALID_HANDLE_VALUE)
		{
			wsprintfA(szShowInfo, "uploadfile:%s write local file:%s error\r\n", gbkfn.c_str(), localfn.c_str());
			WriteLog(szShowInfo);
			return FALSE;
		}

		char* lpdata = lpUploadBuf + sizeof(NETWORKPACKETHEADER);
		int datalen = recvsize - sizeof(NETWORKPACKETHEADER);
		DWORD dwCnt = 0;
		iRet = WriteFile(hfilenew, lpdata, datalen, &dwCnt, 0);

		int recvlen = datalen;
		while (recvlen < uploadsize)
		{
			int tmplen = recv(stParam.hSockClient, lpUploadBuf, bufsize, 0);
			if (tmplen <= 0)
			{
				CloseHandle(hfilenew);
				wsprintfA(szShowInfo, "uploadfile local file:%s recv block:%u error\r\n", localfn.c_str(), recvlen);
				WriteLog(szShowInfo);

				return NETWORK_ERROR_CODE;
			}
			iRet = WriteFile(hfilenew, lpUploadBuf, tmplen, &dwCnt, 0);
			recvlen += tmplen;
			if (recvlen >= uploadsize)
			{
				break;
			}
		}

		CloseHandle(hfilenew);

#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(lpUnique->cMAC);
		string uploadfn = localfn.c_str();
		replaceSlashAndEnter(uploadfn);
		char* utf8fn = 0;
		iRet = GBKToUTF8(uploadfn.c_str(), &utf8fn);
		if (utf8fn)
		{
			mysql->insertUploadFile(mac, utf8fn);
			delete utf8fn;
		}
		delete mysql;
		MySql::leaveLock();
#endif
		wsprintfA(szShowInfo, "uploadfile local file:%s recv size:%u ok\r\n", localfn.c_str(), recvlen);
		WriteLog(szShowInfo);
		return TRUE;
	}
	else {
		wsprintfA(szShowInfo, "upload file:%s unrecognized client response command\r\n", gbkfn.c_str());
		WriteLog(szShowInfo);
		return NETWORK_ERROR_CODE;;
	}
	return TRUE;
}