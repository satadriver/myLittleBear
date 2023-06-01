#include <windows.h>
#include <WinSock.h>
#include "main.h"
#include "Public.h"
#include "CommandRecver.h"
#include "CommandListener.h"
#include "Network.h"

int __stdcall CommandListener::NetWorkCommandListener()
{
	char szShowInfo[1024];

	int hSockListen = Network::listenPort(NETWORK_CMDPORT);

	while (TRUE)
	{
		sockaddr_in stAddrClient = { 0 };

		int iAddrSize = sizeof(sockaddr_in);

		SOCKET hSockClient = accept(hSockListen, (sockaddr*)&stAddrClient, &iAddrSize);
		if (hSockClient)
		{
			DWORD dwSockTimeOut = CMDSOCK_TIME_OUT_VALUE;
			if (setsockopt(hSockClient, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				wsprintfA(szShowInfo, "NetWorkCommandProc recv time setsockopt error code:%u\r\n", GetLastError());
				WriteLog(szShowInfo);
				continue;
			}

			dwSockTimeOut = CMDSOCK_TIME_OUT_VALUE;
			if (setsockopt(hSockClient, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				wsprintfA(szShowInfo, "NetWorkCommandProc send time setsockopt error code:%u\r\n", GetLastError());
				WriteLog(szShowInfo);
				continue;
			}

			NETWORKPROCPARAM stParam = { 0 };
			stParam.hSockClient = hSockClient;
			stParam.stAddrClient = stAddrClient;
			HANDLE hMainProc = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CommandRecver::CommandRecverProc, &stParam, 0, &stParam.dwThreadID);
			CloseHandle(hMainProc);
			Sleep(100);
		}
		else
		{
			WriteLog("NetWorkCommandProc accept error\r\n");
		}
	}
	return TRUE;
}