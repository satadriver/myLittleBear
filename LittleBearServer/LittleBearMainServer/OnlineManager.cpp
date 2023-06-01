
#include "OnlineManager.h"
#include "Public.h"
#include <time.h>
#include <map>
#include <string>
#include <string.h>
#include <iostream>


using namespace std;

map <string, ONLINEINFO> ClientOnlineMap;
map <string, ONLINEINFO> ::iterator maponlineit;


DWORD __stdcall OnlineManager::RefreshOnlineInfo(){

	char szIP[64];
	char szTime[MAX_PATH];
	char szShowInfo[1024];
	int iRet = 0;

	int judgeDelay = (HEARTBEAT_LOOP_DELAY/1000) * 2;

	while(TRUE)
	{
		HANDLE hOnline = CreateFileA(CLIENT_ONLINE_FILE_NAME,GENERIC_READ | GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		if (hOnline == INVALID_HANDLE_VALUE )
		{
			Sleep(ONLINE_REFRESH_TIME);
			continue;
		}

		HANDLE hThreadInfo = CreateFileA(THREAD_INFO_FILE_NAME,GENERIC_READ | GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		if (hThreadInfo == INVALID_HANDLE_VALUE )
		{
			Sleep(ONLINE_REFRESH_TIME);
			continue;
		}

		time_t tmNow = time(0);
		DWORD dwNumber = 0;
		for(maponlineit = ClientOnlineMap.begin(); maponlineit != ClientOnlineMap.end();){
			if (tmNow - maponlineit->second.tmLastOnlineTime <= judgeDelay && tmNow >= maponlineit->second.tmLastOnlineTime)
			{
				DWORD dwLocalIP = maponlineit->second.stAddr.sin_addr.S_un.S_addr;
				unsigned char * pIP = (unsigned char*)&dwLocalIP;
				wsprintfA(szIP,"%u.%u.%u.%u",pIP[0],pIP[1],pIP[2],pIP[3]);

				DWORD dwPort = ntohs(maponlineit->second.stAddr.sin_port);

				SYSTEMTIME stTime = {0};
				GetLocalTime(&stTime);
				wsprintfA(szTime, "%u-%u-%u %d:%d:%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

				string username = string((char*)maponlineit->second.stUnique.username, MAX_USERNAME_SIZE);

				int iLen = wsprintfA(szShowInfo,
					"{\"MAC\":\"%02X_%02X_%02X_%02X_%02X_%02X\","
					"\"user:\":\"%s\","
					"\"IP\":\"%s\","
					"\"PORT\":\"%u\","
					"\"type\":\"%u\","
					"\"threadid\":\"%u\","
					"\"time\":\"%s\","
					"\"current time_t\":\"%I64d\","
					"\"last time_t\":\"%I64d\"}\r\n",
					maponlineit->second.stUnique.cMAC[0],maponlineit->second.stUnique.cMAC[1],maponlineit->second.stUnique.cMAC[2],
					maponlineit->second.stUnique.cMAC[3],maponlineit->second.stUnique.cMAC[4],maponlineit->second.stUnique.cMAC[5],
					username.c_str(),
					szIP,
					dwPort,
					maponlineit->second.dwType,
					maponlineit->second.dwThreadID,
					szTime,
					tmNow,
					maponlineit->second.tmLastOnlineTime);
				DWORD dwCnt = 0;
				int iRet = WriteFile(hOnline,szShowInfo,iLen,&dwCnt,0);

				maponlineit ++;
				continue;
			}
			else if (tmNow - maponlineit->second.tmLastOnlineTime > judgeDelay && tmNow > maponlineit->second.tmLastOnlineTime)
			{	
				HANDLE hThread = OpenThread(THREAD_ALL_ACCESS,0,maponlineit->second.dwThreadID);
				if (hThread)
				{
					maponlineit->second.dwLiveFlag = TRUE;
					
					maponlineit->second.tmLastOnlineTime = tmNow;

					/*
					closesocket(maponlineit->second.hSock);

					char * lpRecvDataBuf = maponlineit->second.lpRecvDataBuf;
					if (lpRecvDataBuf)
					{
						delete [] lpRecvDataBuf;
					}
					*/

					iRet = TerminateThread(hThread,0);
					CloseHandle(hThread);
				}
				ClientOnlineMap.erase(maponlineit++);
				continue;
			}
			else if(tmNow < maponlineit->second.tmLastOnlineTime)
			{
				HANDLE hThread = OpenThread(THREAD_ALL_ACCESS,0,maponlineit->second.dwThreadID);
				if (hThread)
				{
					maponlineit->second.dwLiveFlag = TRUE;
					maponlineit->second.tmLastOnlineTime = tmNow;

					iRet = TerminateThread(hThread,0);
					CloseHandle(hThread);
				}
				ClientOnlineMap.erase(maponlineit++);
				continue;
			}


			DWORD dwIPAddr = maponlineit->second.stAddr.sin_addr.S_un.S_addr;
			unsigned char * pIP = (unsigned char*)&dwIPAddr;
			wsprintfA(szIP,"%u.%u.%u.%u",pIP[0],pIP[1],pIP[2],pIP[3]);

			DWORD dwPort = ntohs(maponlineit->second.stAddr.sin_port);

			SYSTEMTIME stTime = {0};
			GetLocalTime(&stTime);
			wsprintfA(szTime, "%u-%u-%u %d:%d:%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

			string username = string((char*)maponlineit->second.stUnique.username, MAX_USERNAME_SIZE);
			
			int iLen = wsprintfA(szShowInfo,
				"NO:%u\r\nAlive:%u\r\nMAC:%02X_%02X_%02X_%02X_%02X_%02X\r\nIP:%s\r\nPORT:%u\r\ntime:%s\r\ncurrent time_t:%I64d record time_t:%I64d\r\nharddisk Volume Number:%u\r\nsocket:%u\r\nlpRecvDataBuf:0x%08x\r\nthread type:%u\r\nthread id:%u\r\n\r\n",
				dwNumber,maponlineit->second.dwLiveFlag,
				maponlineit->second.stUnique.cMAC[0],maponlineit->second.stUnique.cMAC[1],maponlineit->second.stUnique.cMAC[2],
				maponlineit->second.stUnique.cMAC[3],maponlineit->second.stUnique.cMAC[4],maponlineit->second.stUnique.cMAC[5],
				szIP,dwPort,szTime,tmNow,maponlineit->second.tmLastOnlineTime,maponlineit->second.stUnique.dwVolumeNO,maponlineit->second.hSock,
				maponlineit->second.lpRecvDataBuf,maponlineit->second.dwType,maponlineit->second.dwThreadID);
			DWORD dwCnt = 0;
			int iRet = WriteFile(hThreadInfo,szShowInfo,iLen,&dwCnt,0);

			maponlineit ++;

			dwNumber ++;
		}

		CloseHandle(hThreadInfo);
		CloseHandle(hOnline);

		Sleep(ONLINE_REFRESH_TIME );
		continue;
	}

	return TRUE;
}



DWORD OnlineManager::CheckIfOnlineExist(LPUNIQUECLIENTSYMBOL lpUnique,SOCKET hSock,sockaddr_in  stAddr,
	char * lpRecvDataBuf,DWORD dwType,DWORD dwThreadID){
	char  szKey[MAX_PATH];
	wsprintfA(szKey,"%02X_%02X_%02X_%02X_%02X_%02X_%08X",
		lpUnique->cMAC[0],lpUnique->cMAC[1],lpUnique->cMAC[2],lpUnique->cMAC[3],lpUnique->cMAC[4],lpUnique->cMAC[5],lpUnique->dwVolumeNO);

	maponlineit = ClientOnlineMap.find(string(szKey));
	if (maponlineit != ClientOnlineMap.end())
	{
		maponlineit->second.tmLastOnlineTime	= time(0);
		maponlineit->second.dwThreadID			= dwThreadID;
		maponlineit->second.dwType				= dwType;
		maponlineit->second.hSock				= hSock;
		maponlineit->second.lpRecvDataBuf		= lpRecvDataBuf;
		maponlineit->second.stAddr				= stAddr;
		maponlineit->second.stUnique			= *lpUnique;

	}
	else{
		ONLINEINFO stOnlineInfo				= {0};
		stOnlineInfo.stUnique				= *lpUnique;
		stOnlineInfo.hSock					= hSock;
		stOnlineInfo.stAddr					= stAddr;
		stOnlineInfo.tmLastOnlineTime		= time(0);
		stOnlineInfo.lpRecvDataBuf			= lpRecvDataBuf;
		stOnlineInfo.dwType					= dwType;
		stOnlineInfo.dwThreadID				= dwThreadID;
		stOnlineInfo.dwLiveFlag				= FALSE;
		ClientOnlineMap.insert(map<string,ONLINEINFO>::value_type(string(szKey),stOnlineInfo));
	}
	return TRUE;
}
