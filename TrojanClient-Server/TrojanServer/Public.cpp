#include <windows.h>
#include "Public.h"


int CryptData(char * lpdata,int len,unsigned char * key){
	for(int i = 0,j = 0; i < len ;)
	{
		lpdata[i] ^= key[j];
		j ++;
		if (j == CRYPT_KEY_SIZE)
		{
			j = 0; 
		}
		i ++;
	}
	return TRUE;
}


int GenerateCryptKey(unsigned char * key){

	LPSYSTEMTIME lptime = (LPSYSTEMTIME)key;
	GetLocalTime(lptime);
	return TRUE;
}


int GetPathFromFullName(char* strFullName,char * strDst)
{
	lstrcpyA(strDst,strFullName);

	char * pStr = strDst;
	for (int i = lstrlenA(pStr); i > 0 ; i--)
	{
		if (pStr[i] == 0x5c)
		{
			pStr[i + 1] = 0;
			return TRUE;
		}
	}

	return FALSE;
}



int GetNameFromFullName(char* strFullName,char * strDst)
{
	char * pStr = strFullName;
	for (int i = lstrlenA(pStr); i > 0 ; i--)
	{
		if (pStr[i] == 0x5c)
		{
			i ++;
			memmove(strDst,pStr + i ,lstrlenA(pStr + i));
			return TRUE;
		}
	}

	return FALSE;
}



int WriteLogFile(char * pLog)
{
	HANDLE hFile = CreateFileA(LOG_FILENAME,GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile == INVALID_HANDLE_VALUE){
		return FALSE;
	}

	int iRet = SetFilePointer(hFile,0,0,FILE_END);

	SYSTEMTIME sttime = {0};
	GetLocalTime(&sttime);
	char sztime[1024];
	int sztimelen = wsprintfA(sztime,"%u/%u/%u %u:%u:%u ",sttime.wYear,sttime.wMonth,sttime.wDay,sttime.wHour,sttime.wMinute,sttime.wSecond);
	DWORD dwCnt = 0;
	iRet = WriteFile(hFile,sztime,sztimelen,&dwCnt,0);

	iRet = WriteFile(hFile,pLog,lstrlenA(pLog),&dwCnt,0);
	CloseHandle(hFile);

	if (iRet == FALSE || dwCnt != lstrlenA(pLog)){
		return FALSE;
	}

	return TRUE;
}