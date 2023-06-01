#include "main.h"
#include <windows.h>
#include "Public.h"
#include "DownloadFile.h"
#include "Commander.h"
#include "UpDownResult.h"
#include "mysqlOper.h"
#include "FileHelper.h"
#include "FileReadLine.h"
#include <shlwapi.h>
#include "PublicFunc.h"

#include <string>

using namespace std;

int DownloadFiles::DownloadFile(int cmd, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam, char* DownUploadBuf, int bufsize) {
	char szShowInfo[1024];

	int iRet = 0;

	char szCurrentDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurrentDir);
	lstrcatA(szCurrentDir, "\\");

	string cmdfp = Commander::makeCmdFileName(szCurrentDir, cmdfn, lpUnique);

	char* utf8fn = 0;
	int cmdfs = 0;
	iRet = FileHelper::fileReader(cmdfp.c_str(), &utf8fn, &cmdfs);
	if (iRet <= 0)
	{
		return FALSE;
	}
	DeleteFileA(cmdfp.c_str());

	string gbkfn = makeJsonFromTxt(utf8fn);
	delete utf8fn;


	LPNETWORKFILEHDR lphdr = (LPNETWORKFILEHDR)DownUploadBuf;
	lphdr->packhdr.cmd = cmd;
	lphdr->packhdr.unique = *lpUnique;
	//lphdr->packhdr.packlen = sizeof(int) + lstrlenA(szCmd);
	lphdr->len = gbkfn.length();
	memmove(DownUploadBuf + sizeof(NETWORKFILEHDR), gbkfn.c_str(), gbkfn.length());
	int iLen = sizeof(NETWORKFILEHDR) + gbkfn.length();

	char szfn[MAX_PATH] = { 0 };
	lstrcpyA(szfn, gbkfn.c_str());
	PathStripPathA(szfn);

	string localfn = string(szCurrentDir) + DOWNLOAD_FILE_DIR_NAME + "\\" + szfn;

	HANDLE hfilenew = CreateFileA(localfn.c_str(), GENERIC_WRITE | GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfilenew == INVALID_HANDLE_VALUE)
	{
		wsprintfA(szShowInfo, "DownloadFile:%s open error\r\n", localfn.c_str());
		WriteLog(szShowInfo);
		return FALSE;
	}

	DWORD filesizehigh = 0;
	int filesize = GetFileSize(hfilenew, &filesizehigh);
	if (filesize >= MAX_BUF_SIZE || filesizehigh || filesize <= 0)
	{
		CloseHandle(hfilenew);
		wsprintfA(szShowInfo, "DownloadFile:%s file:%u error\r\n", localfn.c_str(), filesize);
		WriteLog(szShowInfo);
		return FALSE;
	}

	lphdr->packhdr.packlen = sizeof(int) + gbkfn.length() + filesize;

	iRet = send(stParam.hSockClient, DownUploadBuf, iLen, 0);
	if (iRet <= 0)
	{
		CloseHandle(hfilenew);

		iRet = WSAGetLastError();

		wsprintfA(szShowInfo, "CommandProc downloadfile:%s send header error code:%u\r\n", localfn.c_str(), iRet);
		WriteLog(szShowInfo);

		return NETWORK_ERROR_CODE;
	}


	int sendtimes = filesize / bufsize;
	int sendmod = filesize % bufsize;
	int sendsize = 0;
	DWORD dwCnt = 0;
	for (int i = 0; i < sendtimes; i++)
	{
		iRet = ReadFile(hfilenew, DownUploadBuf, bufsize, &dwCnt, 0);
		int tmpsendsize = send(stParam.hSockClient, DownUploadBuf, dwCnt, 0);
		if (tmpsendsize <= 0)
		{
			CloseHandle(hfilenew);

			wsprintfA(szShowInfo, "CommandProc downloadfile:%s send size:%u error\r\n", localfn.c_str(), sendsize);
			WriteLog(szShowInfo);
			return NETWORK_ERROR_CODE;
		}
		sendsize += tmpsendsize;
	}

	if (sendmod)
	{
		iRet = ReadFile(hfilenew, DownUploadBuf, sendmod, &dwCnt, 0);
		int tmpsendsize = send(stParam.hSockClient, DownUploadBuf, dwCnt, 0);
		if (tmpsendsize <= 0)
		{
			CloseHandle(hfilenew);

			wsprintfA(szShowInfo, "CommandProc downloadfile:%s send size:%u error\r\n", localfn.c_str(), sendsize);
			WriteLog(szShowInfo);

			return NETWORK_ERROR_CODE;
		}
		sendsize += tmpsendsize;
	}
	CloseHandle(hfilenew);

#ifdef USE_MYSQL
	MySql::enterLock();
	MySql* mysql = new MySql();

	string mac = FileReadLine::getmac(lpUnique->cMAC);

	replaceSlashAndEnter(localfn);
	char* localutf8fn = 0;
	int localutf8fnlen = GBKToUTF8(localfn.c_str(), &localutf8fn);
	if (localutf8fnlen)
	{
		mysql->insertDownloadFile(mac, localutf8fn);
		delete localutf8fn;
	}

	delete mysql;
	MySql::leaveLock();
#endif

	wsprintfA(szShowInfo, "CommandProc downloadfile:%s send size:%u ok\r\n", localfn.c_str(), sendsize);
	WriteLog(szShowInfo);
	return TRUE;

}