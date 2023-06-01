
#include <windows.h>
#include <winsock.h>



#define DOWNLOADPROGRAM_PATH "DownloadProgram"

typedef struct
{
	sockaddr_in saClient;
	SOCKET sClient;
}STCLIENTSOCKINFO,*LPSTCLIENTSOCKINFO;

DWORD __stdcall DownloadProgram();
DWORD GetStringFromAddr(STCLIENTSOCKINFO stParam,char * pBuf);
int __stdcall WriteLogFile(char * pLog);
DWORD __stdcall SendProgramToClient(LPSTCLIENTSOCKINFO pClientInfo);