#include <Windows.h>
#include "public.h"
#include "main.h"
#include "DownloadProgramRecver.h"
#include "Network.h"


DWORD __stdcall DownloadProgramListener()
{
	int sListen = Network::listenPort(DOWNLOADPROGRAM_PORT);
	char szClientInfo[MAX_PATH];
	char szShowInfo[1024];

	NETWORKPROCPARAM stClientInfo = { 0 };
	while (TRUE)
	{
		sockaddr_in stClientAddr = { 0 };
		int iClientAddrLen = sizeof(sockaddr_in);
		SOCKET sClient = accept(sListen, (sockaddr*)&stClientAddr, &iClientAddrLen);
		if (sClient != INVALID_SOCKET)
		{
			stClientInfo.hSockClient = sClient;
			stClientInfo.stAddrClient = stClientAddr;
			HANDLE hClientProc = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DownloadProgramRecver, &stClientInfo, 0, &stClientInfo.dwThreadID);
			CloseHandle(hClientProc);
		}
		else
		{
			int iRet = WSAGetLastError();
			LPVOID lpMsgBuf = 0;
			int iReturn = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, iRet, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
			if (iReturn)
			{
				InetAddrFormatString(stClientInfo, szClientInfo);
				wsprintfA(szShowInfo, "listen accept error,error code:%u,error description:%s client:%s\r\n", iRet, lpMsgBuf, szClientInfo);
				WriteLog(szShowInfo);
				LocalFree(lpMsgBuf);
			}
		}
	}

	return TRUE;
}