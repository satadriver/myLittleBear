
#include "Network.h"
#include <windows.h>
#include "PublicFunc.h"
#include "Public.h"


int Network::listenPort(unsigned short port) {

	int iRet = 0;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET)
	{
		return FALSE;
	}

	sockaddr_in stServerAddr = { 0 };
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = ntohs(port);
	stServerAddr.sin_addr.S_un.S_addr = dwIP;
	//stServerAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	iRet = bind(sListen, (sockaddr*)&stServerAddr, sizeof(sockaddr_in));
	if (iRet == INVALID_SOCKET)
	{
		closesocket(sListen);

		WriteLog("bind port:%u error:%u\r\n", port, WSAGetLastError());

		return FALSE;
	}

	iRet = listen(sListen, MAX_SERVER_LISTEN_COUNT);
	if (iRet == INVALID_SOCKET)
	{
		closesocket(sListen);

		WriteLog("listen port:%u error:%u\r\n", port, GetLastError());
		return FALSE;
	}

	return sListen;
}