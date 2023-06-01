#include <windows.h>
#include <winsock.h>
#include "Public.h"
#include "SendClientWhiteListAndDll.h"
#pragma comment(lib,"ws2_32.lib")

#define OUTPUT_FORMAT_EXE







DWORD GetIPFromConfigFile()
{
	HANDLE hFile = CreateFileA(IP_CONFIG_FILE,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwFileSizeHigh = 0;
	int iFileSize = GetFileSize(hFile,&dwFileSizeHigh);
	char szIp[1024] = {0};
	DWORD dwCnt = 0;
	int iRet = ReadFile(hFile,szIp,iFileSize,&dwCnt,0);
	CloseHandle(hFile);
	if (iRet == FALSE  || dwCnt != iFileSize)
	{
		iRet = GetLastError();
		return FALSE;
	}

	int i =0;
	int j =0;
	for ( i = 0,j = 0; i < iFileSize; i ++)
	{
		if (szIp[i] == ' ' ||szIp[i] == '\r' || szIp[i] == '\n')
		{
			continue;
		}else if ((szIp[i] >= '0' && szIp[i] <= '9') || szIp[i] == '.')
		{
			szIp[j] = szIp[i];
			j ++;
			continue;
		}
		else{
			MessageBoxA(0,"ip config file error","ipconfig file error",MB_OK);
			return FALSE;
		}
	}

	*(szIp + j) = 0;

	DWORD dwIP = inet_addr(szIp);
	return dwIP;
}




int MainProc(){
	DWORD dwIP = GetIPFromConfigFile();
	if (dwIP == FALSE)
	{
		return FALSE;
	}

	WSAData stWsa = {0};
	int iRet = WSAStartup(0x0202,&stWsa);
	if (iRet )
	{
		return FALSE;
	}

	//iRet = GetWhiteListAndrDllFromServer(dwIP);

	CloseHandle(CreateThread(0,0,(LPTHREAD_START_ROUTINE)SendWhiteListAndrDllFromServer,(LPVOID)dwIP,0,0));
	Sleep(-1);

	WSACleanup();
	return TRUE;
}


#ifdef OUTPUT_FORMAT_EXE
int __stdcall WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	int ret = MainProc();
	return TRUE;
}
#else
int __stdcall DllMain( HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved ){
	if (dwReason == DLL_PROCESS_ATTACH)
	{

		int ret = MainProc();
	}else if (dwReason == DLL_PROCESS_DETACH)
	{

	}else if (dwReason == DLL_THREAD_ATTACH)
	{

	}else if (dwReason == DLL_THREAD_DETACH)
	{

	}

	return TRUE;
}
#endif