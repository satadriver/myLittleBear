

#include <windows.h>
#include <WinSock.h>
#include "main.h"
#include "Public.h"
#include "RemoteControlRecver.h"
#include "Commander.h"
#include "OnlineManager.h"
#include "RemoteControlListener.h"
#include "network.h"



int __stdcall RemoteControlServer::RemoteControlListener() {

	int iRet = 0;
	int hSockListen = Network::listenPort(REMOTE_CONTROLPORT);

	while (TRUE) {
		sockaddr_in stAddrClient = { 0 };
		int iAddrSize = sizeof(sockaddr_in);

		SOCKET hSockClient = accept(hSockListen, (sockaddr*)&stAddrClient, &iAddrSize);
		if (hSockClient == 0 || hSockClient == INVALID_SOCKET)
		{
			WriteLog("RemoteControlListen accept error:%d\r\n", GetLastError());
			continue;
		}
		else
		{
			DWORD dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(hSockClient, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				WriteLog("RemoteControlListen recv time setsockopt error code:%u\r\n", GetLastError());
				closesocket(hSockClient);
				continue;
			}

			dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(hSockClient, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				WriteLog("RemoteControlListen send time setsockopt error code:%u\r\n", GetLastError());
				closesocket(hSockClient);
				continue;
			}

			char tmphdr[MAX_WSPRINTF_BUFFER_SIZE];
			iRet = recv(hSockClient, tmphdr, MAX_WSPRINTF_BUFFER_SIZE, 0);
			NETWORKPACKETHEADER lphdr = *(LPNETWORKPACKETHEADER)tmphdr;
			UNIQUECLIENTSYMBOL stUnique = lphdr.unique;
			if ((iRet == sizeof(NETWORKPACKETHEADER) + sizeof(STREMOTECONTROLPARAMS)) && (lphdr.cmd == REMOTECONTROL_PARAMS) &&
				(lphdr.packlen == sizeof(STREMOTECONTROLPARAMS)))
			{
				char reply[MAX_WSPRINTF_BUFFER_SIZE];
				iRet = Commander::SendCmdPacket(&stUnique, hSockClient, reply, MAX_WSPRINTF_BUFFER_SIZE, RECV_DATA_OK);
				if (iRet)
				{
					STREMOTECONTROLPARAMS param = *(LPSTREMOTECONTROLPARAMS)(tmphdr + sizeof(NETWORKPACKETHEADER));

					//dwbmbitssize = ((bitmap.bmWidth * wbitcount+31)/32)* 4 * bitmap.bmHeight ;
					int remoteBufSize = ((param.screenx * param.bitsperpix + 31) / 32) * 4 * param.screeny + 0x1000;
					//bmpfilehdr bmpinfohdr palette

					char* lpClientBitmap = new char[remoteBufSize];
					if (lpClientBitmap == 0)
					{
						closesocket(hSockClient);
						WriteLog("RemoteControlListen new operator client bitmap buffer error\r\n");
						continue;
					}

					REMOTE_CONTROL_PARAM* lpRemoteControlParam = new REMOTE_CONTROL_PARAM;

					lpRemoteControlParam->lpbmpDataSize = 0;
					lpRemoteControlParam->hSockClient = hSockClient;
					lpRemoteControlParam->stAddrClient = stAddrClient;
					lpRemoteControlParam->dataType = 0;
					lpRemoteControlParam->lpClientBitmap = lpClientBitmap;
					lpRemoteControlParam->bufLimit = remoteBufSize;
					lpRemoteControlParam->hwndWindow = FALSE;
					lpRemoteControlParam->param = param;
					lpRemoteControlParam->unique = stUnique;

					DWORD winthreadid = 0;
					HANDLE hThreadRemoteWindow = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RemoteControler::RemoteControlWindow,
						(LPVOID)lpRemoteControlParam, 0, &winthreadid);

					CloseHandle(hThreadRemoteWindow);

					iRet = OnlineManager::CheckIfOnlineExist(&stUnique, hSockClient, stAddrClient, lpClientBitmap, REMOTE_CONTROL_THREAD, winthreadid);

					continue;
				}
				else {
					closesocket(hSockClient);
				}
			}
			else {
				closesocket(hSockClient);
			}
		}
		continue;
	}

	return FALSE;
}