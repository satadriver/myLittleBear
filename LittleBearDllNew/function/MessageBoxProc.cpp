


#include <windows.h>
#include "../network/NetWorkData.h"


int __stdcall MessageBoxProc(char * msgparam){

	char * param = msgparam;

	int caplen = *(int*)param;
	param += sizeof(int);
	char *szcap = new char[caplen + 4];
	memset(szcap,0,caplen + 4);
	memmove(szcap,param,caplen);
	param += caplen;

	int textlen = *(int*)param;
	char * sztext = new char[textlen + 4];
	param += sizeof(int);
	memset(sztext,0,textlen + 4);
	memmove(sztext,param,textlen);
	param += sizeof(int);
	MessageBoxA(0,sztext,szcap,MB_OKCANCEL);

	delete msgparam;
	return TRUE;
}


