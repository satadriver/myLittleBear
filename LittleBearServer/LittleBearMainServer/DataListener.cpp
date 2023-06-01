#include <windows.h>
#include <WinSock.h>
#include "main.h"
#include "Public.h"
#include "DataRecverer.h"
#include "DataListener.h"
#include "network.h"




int __stdcall DataListener::NetWorkDataListener()
{
	char szShowInfo[1024];

	int hSockListen = Network::listenPort(NETWORK_DATAPORT);

	while (TRUE)
	{
		sockaddr_in stAddrClient = { 0 };

		int iAddrSize = sizeof(sockaddr_in);

		SOCKET hSockClient = accept(hSockListen, (sockaddr*)&stAddrClient, &iAddrSize);
		if (hSockClient)
		{
			DWORD dwBufSize = 0x100000;
			int iRet = setsockopt(hSockClient, SOL_SOCKET, SO_SNDBUF, (char*)&dwBufSize, sizeof(DWORD));
			iRet = setsockopt(hSockClient, SOL_SOCKET, SO_RCVBUF, (char*)&dwBufSize, sizeof(DWORD));

			DWORD dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(hSockClient, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				wsprintfA(szShowInfo, "NetWorkDataRecverProc recv time setsockopt error code:%u\r\n", GetLastError());
				WriteLog(szShowInfo);
				continue;
			}

			dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(hSockClient, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				wsprintfA(szShowInfo, "NetWorkDataRecverProc send time setsockopt error code:%u\r\n", GetLastError());
				WriteLog(szShowInfo);
				continue;
			}

			NETWORKPROCPARAM stParam = { 0 };
			stParam.hSockClient = hSockClient;
			stParam.stAddrClient = stAddrClient;
			HANDLE hMainProc = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DataRecvers::DataRecverProc, &stParam, 0, &stParam.dwThreadID);
			CloseHandle(hMainProc);
			Sleep(100);
		}
		else
		{
			WriteLog("NetWorkDataRecverProc accept error\r\n");
		}
	}
	return TRUE;
}