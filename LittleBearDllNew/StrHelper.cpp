
#include <iostream>
#include <Windows.h>
#include <vector>
#include "log.h"
#include "api.h"
#include "../public.h"

using namespace std;




vector<string> splitstr(string str, string sep) {

	vector<string> result;

	int start = 0;
	int end = 0;
	while (1)
	{
		int end = str.find(sep, start);
		if (end >= 0)
		{
			string item = str.substr(start, end - start);
			result.push_back(item);

			start = end + sep.length();
		}
		else {
			break;
		}
	}

	return result;
}




int strTrim(char* data) {
	int len = lstrlenA(data);
	char* src = data;
	char* dst = data;

	int i = 0;
	int k = 0;
	for (; i < len; )
	{
		if (src[i] == ' ' || src[i] == '\t' || src[i] == '\r' || src[i] == '\n')
		{
			i++;
		}
		else {
			dst[k] = src[i];
			i++;
			k++;
		}
	}

	dst[k] = 0;

	return k;
}


void back2forwardSlash(string& str) {
	int len = str.length();
	for (int i = 0; i < len; i++)
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
		}
	}
}


void back2forwardPlus(string& str) {

	for (int i = 0; i < str.length(); )
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
			i++;
		}
		else if (str.at(i) == '\r' || str.at(i) == '\n')
		{
			str = str.replace(i, 1, "");
		}
		else {
			i++;
		}
	}
}

int UTF8ToGBK(const char* utf8, char** lpdatabuf)
{
	if (lpdatabuf <= 0)
	{
		return FALSE;
	}
	int needunicodelen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	if (needunicodelen <= 0)
	{
		//WriteLogFile("UTF8ToGBK MultiByteToWideChar get len error\r\n");
		*lpdatabuf = 0;
		return FALSE;
	}

	needunicodelen += 16;
	wchar_t* wstr = new wchar_t[needunicodelen];

	int unicodelen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, needunicodelen);
	if (unicodelen <= 0)
	{
		//WriteLogFile("UTF8ToGBK MultiByteToWideChar error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	*(wstr + unicodelen) = 0;

	int needgbklen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (needgbklen <= 0)
	{
		//WriteLogFile("UTF8ToGBK WideCharToMultiByte get len error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	needgbklen += 16;
	*lpdatabuf = new char[needgbklen];

	int gbklen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, *lpdatabuf, needgbklen, NULL, NULL);
	delete[] wstr;
	if (gbklen <= 0)
	{
		//WriteLogFile("UTF8ToGBK WideCharToMultiByte error\r\n");
		delete[](*lpdatabuf);
		*lpdatabuf = 0;
		return FALSE;
	}

	*(*lpdatabuf + gbklen) = 0;
	return gbklen;
}



int GBKToUTF8(const char* gb2312, char** lpdatabuf)
{
	if (lpdatabuf <= 0)
	{
		return FALSE;
	}
	int needunicodelen = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	if (needunicodelen <= 0)
	{
		//WriteLogFile("GBKToUTF8 MultiByteToWideChar get len error\r\n");
		*lpdatabuf = 0;
		return FALSE;
	}

	needunicodelen += 16;
	wchar_t* wstr = new wchar_t[needunicodelen];

	int unicodelen = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, needunicodelen);
	if (unicodelen <= 0)
	{
		//WriteLogFile("GBKToUTF8 MultiByteToWideChar error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	*(wstr + unicodelen) = 0;

	int needutf8len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (needutf8len <= 0)
	{
		//WriteLogFile("GBKToUTF8 WideCharToMultiByte get len error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	needutf8len += 16;
	*lpdatabuf = new char[needutf8len];

	int utf8len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, *lpdatabuf, needutf8len, NULL, NULL);
	delete[] wstr;
	if (utf8len <= 0)
	{
		//WriteLogFile("GBKToUTF8 WideCharToMultiByte error\r\n");
		delete* lpdatabuf;
		*lpdatabuf = 0;
		return FALSE;
	}

	*(*lpdatabuf + utf8len) = 0;
	return utf8len;
}



int MStringToWString(const char* lpcstr, int srclen, wchar_t* lpwstr, int dstlen)
{
	int len = lpMultiByteToWideChar(CP_ACP, 0, lpcstr, srclen, (LPWSTR)lpwstr, dstlen);
	if (len > 0)
	{
		lpwstr[len] = 0;
		return len;
	}
	return 0;
}



int WStringToMString(const wchar_t* lpcwstr, int wstrlen, char* lpstr, int strlen)
{
	int len = lpWideCharToMultiByte(CP_ACP, 0, lpcwstr, wstrlen, lpstr, strlen, NULL, NULL);
	if (len > 0)
	{
		lpstr[len] = 0;
		return len;
	}
	return 0;
}





int makeFullFileName(char* strModuleFileName, char* strName)
{
	lplstrcpyA(strModuleFileName, strDataPath);

	lplstrcatA(strModuleFileName, strName);

	return TRUE;
}


char* _HexStrStr(char  Src[], int SrcLen, char Dst[], int DstLen)
{
	for (int i = 0; i <= (SrcLen - DstLen) + 1; i++)
	{
		if (lpRtlCompareMemory(&Src[i], Dst, DstLen) == (unsigned int)DstLen)
		{
			return &Src[i];
		}
	}
	return FALSE;
}


int _wcscmp(const wchar_t* pSrc, const wchar_t* pDst)
{
	int iSrcLen = 0;
	while (pSrc[iSrcLen])
	{
		iSrcLen++;
	}

	int iDstLen = 0;
	while (pDst[iDstLen])
	{
		iDstLen++;
	}

	if (iDstLen != iSrcLen)
	{
		return -1;
	}


	for (int j = 0; j < iDstLen; j++)
	{
		if (pSrc[j] == pDst[j])
		{
		}
		else
		{
			return -1;
		}
	}

	return 0;
}


//此函数要在加载API前用到并且替换LSTRLENA 所以必须要__stdcall
int __stdcall _MylstrlenA(char* pData)
{
	int iLen = 0;
	while (TRUE)
	{
		if (pData[iLen] == 0) {
			break;
		}
		iLen++;
	}
	return iLen;
}

char* _strstr(char* Src, char* Dst)
{
	int iSrcLen = lplstrlenA(Src);
	int iDstLen = lplstrlenA(Dst);
	if (iSrcLen < iDstLen)
	{
		return FALSE;
	}

	for (int i = 0; i <= iSrcLen - iDstLen + 1; i++)
	{
		if (lpRtlCompareMemory(&Src[i], Dst, iDstLen) == (unsigned int)iDstLen)
		{
			return &Src[i];
		}
	}
	return FALSE;
}




time_t StringToDatetime(char* str)
{
	tm tm_ = { 0 };
	int year, month, day, hour, minute, second;
	int iRet = sscanf_s(str, "%d-%d-%d||%d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	if (iRet != 6)
	{
		return FALSE;
	}
	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;

	time_t t_ = mktime(&tm_); //已经减了8个时区  

	return t_; //秒时间  
}


int GetPathFromFullName(char* strFullName, char* strDst)
{
	int srclen = lplstrlenA(strFullName);
	lplstrcpyA(strDst, strFullName);
	for (unsigned int i = srclen - 1; i >= 0; i--)
	{
		if (strDst[i] == '\\')
		{
			*(strDst + i + 1) = 0;
			return i + 1;
		}
	}

	strDst[0] = 0;
	return FALSE;
}

int GetMainNameFormName(char* szName, char* szMainName)
{
	int len = (int)lplstrlenA(szName);
	for (int i = 0; i < len; i++)
	{
		if (szName[i] == '.')
		{
			*(szMainName + i) = 0;
			return i;
		}
		else
		{
			szMainName[i] = szName[i];
		}
	}
	return FALSE;
}


int GetNameFromFullName(char* strFullName, char* dstname)
{
	for (int i = lplstrlenA(strFullName) - 1; i >= 0; i--)
	{
		if (strFullName[i] == 0x5c)
		{
			lplstrcpyA(dstname, strFullName + i + 1);
			return TRUE;
		}
	}

	return FALSE;
}







unsigned int getIpFromStr(char* strip) {

	strTrim(strip);

	int len = lstrlenA(strip);
	int number = 0;
	int alpha = 0;
	int dot = 0;
	for (int i = 0; i < len; i++)
	{
		if (strip[i] >= '0' && strip[i] <= '9')
		{
			number++;
		}
		else if (strip[i] >= 'a' && strip[i] <= 'z' || strip[i] >= 'A' && strip[i] <= 'Z') {

			alpha++;
		}
		else if (strip[i] == '.')
		{
			dot++;
		}
		else {
			return FALSE;
		}
	}

	if (number + dot == len && dot == 3)
	{
		return lpinet_addr(strip);
	}
	else {
		DWORD dwIP = 0;
		hostent* pHostent = lpgethostbyname(strip);		// from dns to ip
		if (pHostent) {

			if (pHostent->h_addrtype == AF_INET)
			{
				int idx = 0;

				while (pHostent->h_addr_list[idx] != 0)
				{
					ULONG* pPIp = (ULONG*)pHostent->h_addr_list[idx];
					ULONG pIp = *pPIp;

					in_addr ia;
					ia.S_un.S_addr = pIp;
					writeLog("find ip:%x,address:%s\r\n", pIp, inet_ntoa(ia));

					if (dwIP == 0)
					{
						dwIP = pIp;
					}
					idx++;
				}
			}
			else {
				writeLog("hostent type:%d\r\n", pHostent->h_addrtype);
			}
		}
		else {
			writeLog("gethostbyname:%s null\r\n", strip);
		}
		return dwIP;
	}

	return 0;
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
