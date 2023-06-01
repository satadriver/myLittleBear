#pragma once
#ifndef FRESHONLINELIST_H_H_H
#define FRESHONLINELIST_H_H_H

#include <windows.h>
#include "main.h"


class OnlineManager {
public:
	static DWORD __stdcall RefreshOnlineInfo();
	static DWORD CheckIfOnlineExist(LPUNIQUECLIENTSYMBOL lpUnique, SOCKET hSock, sockaddr_in  stAddr, char* lpRecvDataBuf, DWORD dwType, DWORD dwThreadID);
};



#endif