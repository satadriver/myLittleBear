
#include "../api.h"
#include <Windows.h>
#include <lm.h>
#include <Winternl.h>
#include "../initHelper.h"
#include "public.h"
#include "log.h"

#include <tlhelp32.h>
#include <Psapi.h>
#include "../Utils.h"

#include <Shlwapi.h>
#include<psapi.h>
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "shlwapi.lib")



void Executecpuid(DWORD veax, DWORD* Regs)
{
#ifndef _WIN64
	DWORD deax;
	DWORD debx;
	DWORD decx;
	DWORD dedx;

	__asm
	{
		mov eax, veax; 将输入参数移入eax
		cpuid; 执行cpuid
		mov deax, eax; 以下四行代码把寄存器中的变量存入临时变量
		mov debx, ebx
		mov decx, ecx
		mov dedx, edx
	}

	Regs[0] = deax;
	Regs[1] = debx;
	Regs[2] = decx;
	Regs[3] = dedx;
#else
	return;
#endif
}


char* GetCPUBrand(char* strCpuBrand)
{
	strCpuBrand[0] = 0;

	char strcpu[256] = { 0 };
	DWORD Regs[4] = { 0 };
	DWORD BRANDID = 0x80000002;		// 从0x80000002开始，到0x80000004结束,用来存储商标字符串，48个字符
	for (DWORD i = 0; i < 3; i++)
	{
		Executecpuid(BRANDID + i, Regs);
		lpRtlMoveMemory(strcpu + i * 16, (char*)Regs, 16);
	}

	lplstrcpyA(strCpuBrand, strcpu);

	return strCpuBrand;
}




int GetSystemDir(char* sysdir) {
	int iRet = lpGetSystemDirectoryA(sysdir, MAX_PATH);
	return iRet;
}






int IsWow64()
{
	BOOL bIsWow64 = FALSE;
	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function and GetProcAddress to get a pointer to the function if available.

	char szIsWow64Process[] = { 'I','s','W','o','w','6','4','P','r','o','c','e','s','s',0 };
	if (lpDllKernel32 == 0)
	{
		lpDllKernel32 = (HMODULE)GetKernel32Base();
	}
	typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)lpGetProcAddress(lpDllKernel32, szIsWow64Process);
	if (NULL != fnIsWow64Process)
	{
		int iRet = fnIsWow64Process(lpGetCurrentProcess(), &bIsWow64);
		if (iRet)
		{
			if (bIsWow64)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}


int CpuBits() {
	SYSTEM_INFO si;
	lpGetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		return 64;
	else
		return 32;
}

int GetOsBits() {
	int wow = IsWow64();

	int cpubits = CpuBits();

	if (cpubits == 64 && wow == FALSE)
	{
		return 64;
	}
	else if (cpubits == 64 && wow==TRUE)
	{
		return 64;
	}

	return 32;
}

BOOL IsWin11AndLater()
{

	//Windows 10 从内部版本 10240 开始，以内部版本 19044 结束。Windows 11 从内部版本 22000 开始，Environment.OSVersion.Version.Build >= 22000;
	NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
	OSVERSIONINFOEXW osInfo;
	*(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");
	if (NULL != RtlGetVersion)
	{
		osInfo.dwOSVersionInfoSize = sizeof(osInfo);
		RtlGetVersion(&osInfo);

		return (osInfo.dwMajorVersion >= 10 && osInfo.dwBuildNumber >= 22000);
	}

	return FALSE;
}


int GetWindowsVersion(char* strSysVersion)
{
	WKSTA_INFO_100* wkstaInfo = NULL;
	NET_API_STATUS netStatus = lpNetWkstaGetInfo(NULL, 100, (LPBYTE*)&wkstaInfo);
	if (netStatus == NERR_Success)
	{
		DWORD dwMajVer = wkstaInfo->wki100_ver_major;
		DWORD dwMinVer = wkstaInfo->wki100_ver_minor;
		DWORD dwVersion = (DWORD)MAKELONG(dwMinVer, dwMajVer);
		netStatus = lpNetApiBufferFree(wkstaInfo);

		char strWin9x[] = { 'W','i','n','d','o','w','s',' ','9','.','x',0 };
		char strWin2000[] = { 'W','i','n','d','o','w','s',' ','2','0','0','0',0 };
		char strWinXP[] = { 'W','i','n','d','o','w','s',' ','X','P',0 };
		char strWinVista[] = { 'W','i','n','d','o','w','s',' ','V','i','s','t','a',0 };
		char strWin7[] = { 'W','i','n','d','o','w','s',' ','7',0 };
		char strWin8[] = { 'W','i','n','d','o','w','s',' ','8',0 };
		char strWin10[] = { 'W','i','n','d','o','w','s',' ','1','0',0 };
		char strWin11[] = { 'W','i','n','d','o','w','s',' ','1','1',0 };
		char strWin12[] = { 'W','i','n','d','o','w','s',' ','1','2',0 };
		char strWinUnknow[] = { 'U','n','k','n','o','w',0 };

		iSystemVersion = 0;
		if (dwVersion < 0x50000)
		{
			lplstrcpyA(strSysVersion, strWin9x);
			iSystemVersion = SYSTEM_VERSION_WIN9X;
		}
		else if (dwVersion == 0x50000)
		{
			lplstrcpyA(strSysVersion, strWin2000);
			iSystemVersion = SYSTEM_VERSION_WIN2000;
		}
		else if (dwVersion > 0x50000 && dwVersion < 0x60000)
		{
			lplstrcpyA(strSysVersion, strWinXP);
			iSystemVersion = SYSTEM_VERSION_XP;
		}
		else if (dwVersion == 0x60000)
		{
			lplstrcpyA(strSysVersion, strWinVista);
			iSystemVersion = SYSTEM_VERSION_VISTA;
		}
		else if (dwVersion == 0x60001)
		{
			lplstrcpyA(strSysVersion, strWin7);
			iSystemVersion = SYSTEM_VERSION_WIN7;
		}
		else if (dwVersion >= 0x60002 && dwVersion <= 0x60003)
		{
			lplstrcpyA(strSysVersion, strWin8);
			iSystemVersion = SYSTEM_VERSION_WIN8;
		}
		else if (dwVersion >= 0x60003 || dwVersion == 0x100000)
		{
			int iswin11 = IsWin11AndLater();
			if (iswin11)
			{
				lplstrcpyA(strSysVersion, strWin11);
				iSystemVersion = SYSTEM_VERSION_WIN11;
			}
			else {
				lplstrcpyA(strSysVersion, strWin10);
				iSystemVersion = SYSTEM_VERSION_WIN10;
			}
		}
		else
		{
			lplstrcpyA(strSysVersion, strWinUnknow);
			iSystemVersion = SYSTEM_VERSION_UNKNOW;
		}
		return iSystemVersion;
	}

	return FALSE;
}




int GetHostName(char* szHostName)
{
	return lpgethostname(szHostName, MAX_PATH);
}




int GetUserAndComputerName(char* strUserName, char* strComputerName)
{
	DWORD dwSize = MAX_PATH;
	int iRet = lpGetUserNameA(strUserName, &dwSize);
	if (iRet == 0)
	{
		return FALSE;
	}

	dwSize = MAX_PATH;
	iRet = lpGetComputerNameA(strComputerName, &dwSize);
	if (iRet == 0)
	{
		return FALSE;
	}

	return TRUE;
}




__kernel_entry NTSTATUS  NTAPI lpNtQueryInformationProcess(HMODULE lpDllntdll, IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL)
{
	NTSTATUS rc = 0;
	char szNtQueryInformationProcess[] = { 'N','t','Q','u','e','r','y','I','n','f','o','r','m','a','t','i','o','n','P','r','o','c','e','s','s',0 };
	typedef NTSTATUS(WINAPI* NTQUERYINFORMATIONPROCESS)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
	NTQUERYINFORMATIONPROCESS pfn = (NTQUERYINFORMATIONPROCESS)lpGetProcAddress(lpDllntdll, szNtQueryInformationProcess);
	if (pfn)
	{
		rc = pfn(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
	}

	return rc;
}


int GetParentProcName(char* szParentProcName)
{
	int result = 0;
	if (szParentProcName == 0)
	{
		return FALSE;
	}

	szParentProcName[0] = 0;

	HANDLE hProcess = lpOpenProcess(PROCESS_QUERY_INFORMATION, FALSE, lpGetCurrentProcessId());
	if (hProcess)
	{
		LONG status = 0;
		PROCESS_BASIC_INFORMATION pbi = { 0 };
		if (lpDllntdll == NULL)
		{
			char szDllntdll[] = { 'n','t','d','l','l','.','d','l','l',0 };
			lpDllntdll = (HMODULE)lpLoadLibraryA(szDllntdll);
		}
		status = lpNtQueryInformationProcess(lpDllntdll, hProcess, ProcessBasicInformation, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
		if (NT_SUCCESS(status))
		{
			DWORD dwParentPID = (UINT)pbi.Reserved3;
			HANDLE hParentProcess = lpOpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwParentPID);
			if (hParentProcess)
			{
				CHAR szBuf[1024] = { 0 };
				status = lpNtQueryInformationProcess(lpDllntdll, hParentProcess, (PROCESSINFOCLASS)27, (PVOID)szBuf, sizeof(szBuf), NULL);
				if (NT_SUCCESS(status))
				{
					PUNICODE_STRING lpuImageFileName = (PUNICODE_STRING)szBuf;

					int len = lpWideCharToMultiByte(CP_ACP, 0, lpuImageFileName->Buffer, -1, szParentProcName, MAX_PATH, NULL, NULL);

					result = TRUE;
				}
				lpCloseHandle(hParentProcess);
			}
			else {
				writeLog("open processid:%d error\r\n", dwParentPID);

				result = getProcNameByPID(dwParentPID, szParentProcName, MAX_PATH);
			}
		}
		else {
			writeLog("lpNtQueryInformationProcess error\r\n");
		}
		lpCloseHandle(hProcess);
	}
	return result;
}











int GetDiskVolumeSerialNo(DWORD* lpulVolumeSerialNo) {
	int iRet = lpGetVolumeInformationA(0, 0, 0, lpulVolumeSerialNo, 0, 0, 0, 0);
	if (iRet == 0)
	{
		writeLog("GetHradDiskInfo lpGetVolumeInformationA error\r\n");
		return FALSE;
	}
	else {

		writeLog("lpGetVolumeInformationA:%u\r\n", *lpulVolumeSerialNo);

		return TRUE;
	}
}




int GetDiskSpaceInfo(char* szBuf)
{
	int iRet = 0;

	char partition[0x1000];
	char* szBufPtr = partition;

	char strDisk[128];
	char* strDiskPtr = strDisk;
	int iLen = lpGetLogicalDriveStringsA(sizeof(strDisk), strDisk);

	ULARGE_INTEGER iTotalSize = { 0 };
	for (int i = 0; i < iLen / 4; ++i)
	{
		ULARGE_INTEGER liFreeBytesLeast;
		ULARGE_INTEGER liFreeBytesTotol;
		ULARGE_INTEGER liTotalBytes;

		int iRes = lpGetDriveTypeA(strDiskPtr);
		//会出现不存在软盘异常 必须去掉DRIVE_REMOVABLE?
		if (iRes == DRIVE_FIXED || iRes == DRIVE_REMOTE || iRes == DRIVE_CDROM || iRes == DRIVE_REMOVABLE)
		{
			if ((*strDiskPtr == 'A' || *strDiskPtr == 'B' || *strDiskPtr == 'a' || *strDiskPtr == 'b') && iRes == DRIVE_REMOVABLE)
			{

			}
			else
			{
				iRet = lpGetDiskFreeSpaceExA(strDiskPtr, &liFreeBytesLeast, &liTotalBytes, &liFreeBytesTotol);
				if (iRet)
				{
					int iSize = lpwsprintfA(szBufPtr, ",%c盘容量:%I64dGB,剩余容量:%I64dGB",
						strDiskPtr[0], liTotalBytes.QuadPart / 1000000000, liFreeBytesLeast.QuadPart / 1000000000);
					szBufPtr += iSize;
					iTotalSize.QuadPart += liTotalBytes.QuadPart;
				}
			}
		}

		strDiskPtr += 4;
	}

	int len = lpwsprintfA(szBuf, "磁盘总容量:%I64dGB%s", iTotalSize.QuadPart / 1000000000, partition);
	return len;
}