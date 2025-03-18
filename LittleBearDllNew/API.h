#pragma once

#ifndef API_H_H_H
#define API_H_H_H

#include <Windows.h>
#include <iphlpapi.h>
#include <WinSock.h>
#include <GdiPlus.h>
#include <wchar.h>
#include  <setupapi.h> 
#include <iostream>
#include <tlhelp32.h>
#include <vfw.h>
#include <GdiPlus.h>

#ifndef INCLUDE_JSON_H_H_H

#include <json/json.h>

#endif





typedef BOOL(WINAPI* PFN_Wow64DisableWow64FsRedirection)(OUT  PVOID*);
typedef BOOL(WINAPI* PFN_Wow64RevertWow64FsRedirection)(OUT  PVOID);
typedef HRESULT(__stdcall* ptrCoCreateInstance)(
	_In_  REFCLSID  rclsid,
	_In_  LPUNKNOWN pUnkOuter,
	_In_  DWORD     dwClsContext,
	_In_  REFIID    riid,
	_Out_ LPVOID* ppv
	);
typedef void(__stdcall* ptrCoUninitialize)(void);
typedef HRESULT(__stdcall* ptrCoInitialize)(
	_In_opt_ LPVOID pvReserved
	);
typedef void(__stdcall* ptrSHChangeNotify)(
	LONG    wEventId,
	UINT    uFlags,
	_In_opt_ LPCVOID dwItem1,
	_In_opt_ LPCVOID dwItem2
	);
typedef HINSTANCE(__stdcall* ptrShellExecuteA)(
	HWND hwnd,
	LPCSTR lpOperation,
	LPCSTR lpFile,
	LPCSTR lpParameters,
	LPCSTR lpDirectory,
	INT nShowCmd);



typedef DWORD(WINAPI* ptrDefWindowProcA)(HWND, UINT, WPARAM, LPARAM);
typedef ATOM(WINAPI* ptrRegisterClassExA)(WNDCLASSEXA* lpwcx);
typedef BOOL(WINAPI* ptrTranslateMessage)(MSG* lpMsg);
typedef LONG(WINAPI* ptrDispatchMessageA)(MSG* lpmsg);
typedef HWND(WINAPI* ptrCreateWindowExA)(
	DWORD dwExStyle,
	LPCSTR lpClassName,
	LPCSTR lpWindowName,
	DWORD dwStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU hMenu,
	HINSTANCE hInstance,
	LPVOID lpParam);

typedef int (WINAPI* ptrSetCursorPos)(int x, int y);
typedef HRSRC(WINAPI* ptrFindResourceA)(HMODULE hModule, LPCSTR lpName, LPCSTR lpType);
typedef DWORD(WINAPI* ptrSizeofResource)(HMODULE hModule, HRSRC hResInfo);
typedef HGLOBAL(WINAPI* ptrLoadResource)(HMODULE hModule, HRSRC hResInfo);
typedef LPVOID(WINAPI* ptrLockResource)(HGLOBAL hResData);

typedef HWND(__stdcall* ptrGetDesktopWindow)();
typedef DWORD(__stdcall* ptrUpdateWindow)(HWND);
typedef HWND(__stdcall* ptrFindWindowExA)(__in_opt HWND hWndParent, __in_opt HWND hWndChildAfter, __in_opt LPCSTR lpszClass, __in_opt LPCSTR lpszWindow);
//HWND FindWindowEx£¨HWND hwndParent£¬HWND hwndChildAfter£¬LPCTSTR lpszClass£¬LPCTSTR lpszWindow);
typedef DWORD(__stdcall* ptrShellExecuteExW)(_Inout_ SHELLEXECUTEINFO* pExecInfo);
typedef DWORD(__stdcall* ptrStrCmpIW)(wchar_t*, wchar_t*);
typedef int (WINAPI* ptrlstrcmpiA)(LPCSTR lpString1, LPCSTR lpString2);
typedef void(__stdcall* ptrPathStripPathA)(char*);
typedef DWORD(__stdcall* ptrGetCurrentDirectoryA)(DWORD, char*);
typedef HANDLE(__stdcall* ptrIcmpCreateFile)(VOID);
typedef DWORD(__stdcall* ptrIcmpSendEcho2)(HANDLE IcmpHandle, HANDLE Event, DWORD, DWORD, IPAddr DestinationAddress,
	LPVOID RequestData, WORD RequestSize, PIP_OPTION_INFORMATION RequestOptions, LPVOID ReplyBuffer, DWORD ReplySize, DWORD Timeout);
typedef DWORD(__stdcall* ptrIcmpSendEcho2Ex)(HANDLE IcmpHandle, HANDLE Event, DWORD, DWORD, IPAddr, IPAddr DestinationAddress,
	LPVOID RequestData, WORD RequestSize, PIP_OPTION_INFORMATION RequestOptions, LPVOID ReplyBuffer, DWORD ReplySize, DWORD Timeout);
typedef DWORD(__stdcall* ptrIcmpCloseHandle)(__in HANDLE IcmpHandle);
typedef DWORD(__stdcall* ptrIcmpParseReplies)(LPVOID ReplyBuffer, __in DWORD ReplySize);

typedef DWORD(__stdcall* ptrAllocateAndInitializeSid)(_In_ PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority, _In_ BYTE nSubAuthorityCount, _In_ DWORD nSubAuthority0,
	_In_ DWORD nSubAuthority1, _In_ DWORD nSubAuthority2, _In_ DWORD nSubAuthority3, _In_ DWORD nSubAuthority4,
	_In_ DWORD nSubAuthority5, _In_ DWORD nSubAuthority6, _In_ DWORD nSubAuthority7, PSID* pSid);
typedef DWORD(__stdcall* ptrEqualSid)(_In_ PSID pSid1, _In_ PSID pSid2);
typedef DWORD(__stdcall* ptrFreeSid)(_In_ PSID pSid);
typedef DWORD(__stdcall* ptrGetTokenInformation)(_In_ HANDLE TokenHandle, _In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
	LPVOID TokenInformation, DWORD TokenInformationLength, PDWORD ReturnLength);
typedef DWORD(__stdcall* ptrOpenProcessToken)(_In_ HANDLE ProcessHandle, _In_ DWORD DesiredAccess, PHANDLE TokenHandle);
typedef DWORD(__stdcall* ptrGetKeyboardState)(LPBYTE);
typedef DWORD(__stdcall* ptrGetCursorPos)(LPPOINT lpPoint);

typedef DWORD(__stdcall* ptrGetExitCodeProcess)(HANDLE, LPDWORD);
typedef DWORD(__stdcall* ptrCreatePipe)(PHANDLE hReadPipe, PHANDLE hWritePipe, LPSECURITY_ATTRIBUTES lpPipeAttributes, DWORD nSize);

typedef DWORD(__stdcall* ptrRemoveDirectoryA)(_In_ LPCSTR lpPathName);
typedef DWORD(__stdcall* ptrGetDiskFreeSpaceExA)(LPCSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes);

typedef DWORD(__stdcall* ptrSetCurrentDirectoryA)(_In_ LPCSTR lpPathName);
typedef DWORD(__stdcall* ptrGetUserNameA)(LPSTR lpBuffer, _Inout_ LPDWORD pcbBuffer);
typedef DWORD(__stdcall* ptrGetComputerNameA)(LPSTR lpBuffer, _Inout_ LPDWORD nSize);

typedef LPTSTR(__stdcall* ptrlstrcpyA)(LPSTR lpString1, _In_ LPCSTR lpString2);
typedef DWORD(__stdcall* ptrlstrcatA)(LPSTR lpString1, _In_ LPCSTR lpString2);
typedef LPTSTR(__stdcall* ptrlstrcmpA)(_In_ LPCSTR lpString1, _In_ LPCSTR lpString2);
typedef DWORD(__stdcall* ptrDeleteFileW)(LPWSTR);
typedef LPCWSTR(__stdcall* ptrGetCommandLineW)();
typedef wchar_t** (__stdcall* ptrCommandLineToArgvW)(__in LPCWSTR lpCmdLine, __out int* pNumArgs);
typedef DWORD(__stdcall* ptrReleaseMutex)(__in HANDLE hMutex);

typedef HANDLE(__stdcall* ptrGetCurrentProcess)();
typedef DWORD(__stdcall* ptrGetProcAddress)(HMODULE, LPSTR);
typedef HMODULE(__stdcall* ptrLoadLibraryA)(LPSTR);
typedef BOOL(WINAPI* ptrFreeLibrary)(_In_ HMODULE hModule);
typedef HANDLE(__stdcall* ptrCreateFileA)(LPSTR, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef HANDLE(__stdcall* ptrCreateFileW)(LPWSTR, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef UINT(__stdcall* ptrWriteFile)(HANDLE, CHAR*, unsigned int, LPDWORD, int);
typedef UINT(__stdcall* ptrCloaseHandle)(HANDLE);
typedef LPSTR(__stdcall* ptrVirtualAlloc)(DWORD, DWORD, DWORD, DWORD);
typedef UINT(__stdcall* ptrVirtualFree)(LPVOID, DWORD, DWORD);
typedef DWORD(__stdcall* ptrGetModuleFileNameA)(HANDLE, LPSTR, DWORD);
typedef DWORD(__stdcall* ptrGetStartupInfoA)(LPSTARTUPINFOA);
typedef DWORD(__stdcall* ptrCreateProcessA)(LPSTR, LPSTR, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
typedef DWORD(__stdcall* ptrWinExec)(LPSTR, DWORD);
typedef UINT(__stdcall* ptrGetSystemDirectoryA)(LPSTR, DWORD);
typedef UINT(__stdcall* ptrExitProcess)(DWORD);
typedef HANDLE(__stdcall* ptrGetModuleHandleA)(LPSTR);
typedef DWORD(__stdcall* ptrGetFileSize)(HANDLE, LPDWORD);
typedef DWORD(__stdcall* ptrSetFilePointer)(HANDLE, DWORD, LPDWORD, DWORD);
typedef UINT(__stdcall* ptrReadFile)(HANDLE, CHAR*, unsigned int, LPDWORD, int);
typedef UINT(__stdcall* ptrCreateDirectoryA)(CHAR*, LPSECURITY_ATTRIBUTES);
typedef DWORD(__stdcall* ptrlstrlenA)(char*);
typedef DWORD(__stdcall* ptrlstrlenW)(LPWSTR);
typedef CHAR* (__stdcall* ptrGlobalAlloc)(DWORD, DWORD);
typedef DWORD(__stdcall* ptrGlobalFree)(char*);
typedef DWORD(__stdcall* ptrGetTickCount)();
typedef DWORD(__stdcall* ptrGetLocalTime)(LPSYSTEMTIME);
typedef VOID(__stdcall* ptrSleep)(DWORD);
typedef UINT(__stdcall* ptrGetVolumeInformationA)(LPSTR, LPSTR, DWORD, LPDWORD, LPDWORD, LPDWORD, LPSTR, DWORD);
typedef char* (__stdcall* ptrGetCommandLineA)();
typedef DWORD(__stdcall* ptrDeleteFileA)(LPSTR);
typedef UINT(__stdcall* ptrGetLogicalDriveStringsA)(DWORD, LPSTR);
typedef HANDLE(__stdcall* ptrFindFirstFileA)(LPSTR, LPWIN32_FIND_DATAA);
typedef DWORD(__stdcall* ptrFindNextFileA)(HANDLE, LPWIN32_FIND_DATAA);
typedef DWORD(__stdcall* ptrFindClose)(HANDLE);
typedef DWORD(__stdcall* ptrDeleteFileA)(LPSTR);
typedef DWORD(__stdcall* ptrGetDriveTypeA)(LPSTR);

typedef HANDLE(WINAPI* ptrCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL(WINAPI* ptrProcess32First)(HANDLE hSnapshot, LPPROCESSENTRY32W lppe);
typedef BOOL(WINAPI* ptrProcess32Next)(HANDLE hSnapshot, LPPROCESSENTRY32W lppe);

typedef DWORD(__stdcall* ptrGetVersionExA)(LPOSVERSIONINFO);
typedef VOID(__stdcall* ptrGetSystemInfo)(LPSYSTEM_INFO);
typedef DWORD(__stdcall* ptrMoveFileExA)(LPSTR, LPSTR, DWORD);

typedef DWORD(__stdcall* ptrRtlGetLastWin32Error)();
typedef DWORD(__stdcall* ptrRtlZeroMomery)(char*, DWORD);
typedef DWORD(__stdcall* ptrRtlMoveMomery)(char*, char*, DWORD);
typedef DWORD(__stdcall* ptrRtlCompareMomery)(char*, char*, DWORD);

typedef int(__stdcall* ptrWSAStartup)(WORD, LPWSADATA);
typedef DWORD(__stdcall* ptrsend)(SOCKET, char*, DWORD, DWORD);
typedef DWORD(__stdcall* ptrrecv)(SOCKET, char*, DWORD, DWORD);
typedef DWORD(__stdcall* ptrclosesocket)(SOCKET);
typedef DWORD(__stdcall* ptrconnect)(SOCKET, sockaddr*, DWORD);
typedef DWORD(__stdcall* ptrinet_addr)(char*);
typedef unsigned short(__stdcall* ptrntohs)(unsigned short);
typedef DWORD(__stdcall* ptrgethostname)(char*, int);
typedef SOCKET(__stdcall* ptrsocket)(UINT, UINT, UINT);
typedef hostent* (__stdcall* ptrgethostbyname)(char*);
typedef int(__stdcall* ptrsetsockopt)(__in SOCKET s, __in int level, __in int optname, __in_bcount_opt(optlen) const char FAR* optval, __in int optlen);

typedef DWORD(__stdcall* ptrRegCreateKeyA)(HKEY, char*, PHKEY);
typedef DWORD(__stdcall* ptrRegCloseKey)(HKEY);


typedef DWORD(__stdcall* ptrRegQueryValueExA)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef DWORD(__stdcall* ptrRegSetValueExA)(HKEY, char*, DWORD, DWORD, LPVOID, DWORD);
typedef int(__stdcall* ptrRegOpenKeyExA)(HKEY, char*, DWORD, REGSAM, PHKEY);
typedef int(__stdcall* ptrRegEnumKeyExA)(HKEY, DWORD, char*, LPDWORD, LPDWORD, char*, LPDWORD, PFILETIME);
typedef DWORD(__stdcall* ptrRegDeleteKeyA)(__in HKEY hKey, __in LPCSTR lpSubKey);

typedef HDC(__stdcall* ptrGetDC)(HWND);
typedef DWORD(__stdcall* ptrReleaseDC)(HWND, HDC);
typedef DWORD(__cdecl* ptrwsprintfA)(char*, char*, ...);
typedef DWORD(__cdecl* ptrwsprintfW)(__out LPWSTR, __in __format_string LPCWSTR, ...);
typedef short(__stdcall* ptrGetAsyncKeyState)(DWORD);
typedef short(__stdcall* ptrGetKeyState)(DWORD);

typedef HDC(__stdcall* ptrCreateDCA)(LPSTR, LPSTR, LPSTR, DEVMODE*);
typedef int(__stdcall* ptrGetObjectA)(IN HGDIOBJ, IN int, OUT LPVOID);
typedef HDC(__stdcall* ptrCreateDCA)(LPSTR, LPSTR, LPSTR, DEVMODE*);
typedef HBITMAP(__stdcall* ptrCreateCompatibleBitmap)(HDC, DWORD, DWORD);
typedef HDC(__stdcall* ptrCreateCompatibleDC)(HDC);
typedef DWORD(__stdcall* ptrCreateDIBSection)(HDC, BITMAPINFO*, DWORD, DWORD, HANDLE, DWORD);
typedef HGDIOBJ(__stdcall* ptrSelectObject)(HDC, HGDIOBJ);
typedef DWORD(__stdcall* ptrBitBlt)(HDC, DWORD, DWORD, DWORD, DWORD, HDC, DWORD, DWORD, DWORD);
typedef DWORD(__stdcall* ptrDeleteDC)(HDC);
typedef DWORD(__stdcall* ptrGetDeviceCaps)(HDC, DWORD);
typedef HDC(__stdcall* ptrSelectPalette)(HDC, HPALETTE, DWORD);
typedef DWORD(__stdcall* ptrRealizePalette)(HDC);
typedef HGDIOBJ(__stdcall* ptrGetStockObject)(DWORD);
typedef DWORD(__stdcall* ptrGetDIBits)(HDC, HBITMAP, DWORD, DWORD, LPVOID, LPBITMAPINFO, DWORD);
typedef DWORD(__stdcall* ptrDeleteObject)(HGDIOBJ);

typedef DWORD(__stdcall* ptrGetAdaptersInfo)(PIP_ADAPTER_INFO, LPDWORD);

typedef UINT(__cdecl* ptrwcscmp)(const wchar_t*, const wchar_t*);
typedef char* (__cdecl* _ptrstrstr)(char*, char*);
typedef char* (__cdecl* _ptrHexStrStr)(char*, int, char*, int);

typedef int(__stdcall* ptrGdipSaveImageToStream)(DWORD);
typedef int(__stdcall* ptrEncoderQuality)(DWORD);
typedef int(__stdcall* ptrGdipDisposeImage)(DWORD);
typedef int(__stdcall* ptrGdipCreateBitmapFromStream)(DWORD);
typedef int(__stdcall* ptrGdipAlloc)(DWORD);
typedef int(__stdcall* ptrGdipCloneImage)(DWORD);
typedef int(__stdcall* ptrGdipFree)(DWORD);

typedef int(__stdcall* ptrGetImageEncodersSize)(LPUINT, LPUINT);
typedef int(__stdcall* ptrGetImageEncoders)(UINT, UINT, Gdiplus::ImageCodecInfo*);
typedef int(__stdcall* ptrGdiplusStartup)(ULONG_PTR*, Gdiplus::GdiplusStartupInput*, DWORD);
typedef int(__stdcall* ptrGdiplusShutdown)(UINT);

typedef int(__stdcall* _ptrCreateStreamOnHGlobal)(HGLOBAL hGlobal, BOOL fDeleteOnRelease, LPSTREAM* ppstm);

typedef BOOL(__stdcall* ptrSetupDiEnumDeviceInfo)(IN  HDEVINFO  DeviceInfoSet, IN  DWORD MemberIndex, OUT PSP_DEVINFO_DATA DeviceInfoData);
typedef BOOL(__stdcall* ptrSetupDiSetClassInstallParamsA)(IN HDEVINFO DeviceInfoSet, IN PSP_DEVINFO_DATA  DeviceInfoData, PSP_CLASSINSTALL_HEADER ClassInstallParams, DWORD ClassInstallParamsSize);
typedef BOOL(__stdcall* ptrSetupDiCallClassInstaller)(IN DI_FUNCTION InstallFunction, IN HDEVINFO DeviceInfoSet, IN PSP_DEVINFO_DATA DeviceInfoData OPTIONAL);
typedef HDEVINFO(__stdcall* ptrSetupDiGetClassDevsA)(CONST GUID* ClassGuid, PCSTR Enumerator, HWND hwndParent, DWORD Flags);
typedef HKEY(__stdcall* ptrSetupDiOpenClassRegKey)(IN CONST GUID* ClassGuid, REGSAM samDesired);

typedef BOOL(WINAPI* ptrOpenClipboard)(__in_opt HWND hWndNewOwner);
typedef BOOL(WINAPI* ptrCloseClipboard)(VOID);
typedef BOOL(WINAPI* ptrIsClipboardFormatAvailable)(__in UINT format);
typedef HANDLE(WINAPI* ptrGetClipboardData)(__in UINT uFormat);
typedef LPVOID(WINAPI* ptrGlobalLock)(__in HGLOBAL hMem);
typedef BOOL(WINAPI* ptrGlobalUnlock)(__in HGLOBAL hMem);
typedef int (WINAPI* ptrMultiByteToWideChar)(UINT CodePage, DWORD  dwFlags, LPCSTR   lpMultiByteStr, int cbMultiByte, LPWSTR  lpWideCharStr, int  cchWideChar);
typedef int (WINAPI* ptrWideCharToMultiByte)(UINT  CodePage, DWORD dwFlags, LPCWSTR  lpWideCharStr, int cchWideChar, LPSTR   lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL  lpUsedDefaultChar);

typedef	BOOL(WINAPI* ptrReadDirectoryChangesW)(HANDLE hDirectory, LPVOID lpBuffer, DWORD nBufferLength, BOOL bWatchSubtree, DWORD dwNotifyFilter, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
typedef BOOL(WINAPI* ptrDeviceIoControl)(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

typedef BOOL(WINAPI* ptrGetMessageA)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);

typedef HANDLE(__stdcall* ptrCreateThread)(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
typedef int(__stdcall* ptrSystemParametersInfoA)(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);

typedef MMRESULT(WINAPI* ptrmmioAscend)(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuAscend);
typedef HMMIO(WINAPI* ptrmmioOpenA)(LPSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen);

typedef HMMIO(WINAPI* ptrmmioClose)(HANDLE, DWORD);
typedef MMRESULT(WINAPI* ptrmmioCreateChunk)(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuCreate);
typedef LONG(WINAPI* ptrmmioWrite)(HMMIO hmmio, const char* pch, LONG cch);
typedef INT(WINAPI* ptrwaveInOpen)(LPHWAVEIN phwi, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
typedef MMRESULT(WINAPI* ptrwaveInClose)(HWAVEIN hwi);
typedef MMRESULT(WINAPI* ptrwaveInPrepareHeader)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT(WINAPI* ptrwaveInUnprepareHeader)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT(WINAPI* ptrwaveInAddBuffer)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT(WINAPI* ptrwaveInStart)(HWAVEIN hwi);
typedef MMRESULT(WINAPI* ptrwaveInStop)(HWAVEIN hwi);
typedef MMRESULT(WINAPI* ptrwaveInReset)(HWAVEIN hwi);

typedef LRESULT(WINAPI* ptrSendMessageA)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
typedef LRESULT(WINAPI* ptrSendMessageW)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
typedef HWND(WINAPI* ptrcapCreateCaptureWindowA)(LPCSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, int nID);
typedef HWND(WINAPI* ptrcapCreateCaptureWindowW)(LPCWSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, int nID);

typedef HANDLE(WINAPI* ptrCreateMutexA)(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName);
typedef BOOL(WINAPI* ptrIsWindowsVersionOrGreater)(void);
typedef BOOL(WINAPI* ptrIsWindowsXPOrGreater)(void);
typedef BOOL(WINAPI* ptrIsWindowsVistaOrGreater)(void);
typedef BOOL(WINAPI* ptrIsWindows7OrGreater)(void);
typedef BOOL(WINAPI* ptrIsWindows8OrGreater)(void);
typedef BOOL(WINAPI* ptrIsWindowsServer)(void);

typedef DWORD(WINAPI* ptrGlobalMemoryStatusEx)(LPMEMORYSTATUSEX);

typedef	BOOL(WINAPI* ptrVerifyVersionInfoW)(LPOSVERSIONINFOEXW lpVersionInformation, DWORD dwTypeMask, DWORDLONG dwlConditionMask);
typedef	ULONGLONG(NTAPI* ptrVerSetConditionMask)(ULONGLONG ConditionMask, DWORD TypeMask, BYTE  Condition);

typedef LSTATUS(APIENTRY* ptrRegCreateKeyExA)(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);
typedef DWORD(__stdcall* ptrCopyFileA)(char* lpExistingFile, char* lpDstFile, int iReplaceIfExist);

typedef BOOL(WINAPI* ptrEnumProcesses) (DWORD* lpidProcess, DWORD cb, LPDWORD lpcbNeeded);
typedef BOOL(WINAPI* ptrEnumProcessModules)(HANDLE hProcess, HMODULE* lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef HANDLE(__stdcall* ptrOpenProcess)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
typedef DWORD(WINAPI* ptrGetModuleBaseNameA)(HANDLE hProcess, HMODULE hModule, LPSTR lpBaseName, DWORD nSize);
typedef HANDLE(WINAPI* ptrCreateRemoteThread)(HANDLE hProcess, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
typedef LPVOID(WINAPI* ptrVirtualAllocEx)(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
typedef BOOL(WINAPI* ptrVirtualFreeEx)(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
typedef BOOL(WINAPI* ptrWriteProcessMemory)(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten);
typedef DWORD(WINAPI* ptrWaitForSingleObject)(HANDLE hHandle, DWORD dwMilliseconds);
typedef BOOL(WINAPI* ptrGetExitCodeThread)(HANDLE hThread, LPDWORD lpExitCode);
typedef DWORD(WINAPI* ptrWaitForMultipleObjects)(DWORD nCount, CONST HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);

typedef int (WINAPI* ptrExitThread)(__in DWORD dwExitCode);
typedef HWND(WINAPI* ptrGetForegroundWindow)(VOID);
typedef int (WINAPI* ptrGetWindowTextA)(HWND hWnd, char* strBuf, int iSize);

typedef HANDLE(WINAPI* ptrCreateEventA)(LPSECURITY_ATTRIBUTES, INT, INT, CHAR*);
typedef DWORD(WINAPI* ptrSetEvent)(HANDLE);
typedef DWORD(WINAPI* ptrGetCurrentProcessId)();

typedef void (WINAPI* ptrAVIFileInit)();
typedef DWORD(WINAPI* ptrAVIStreamRelease)(PAVISTREAM);
typedef DWORD(WINAPI* ptrAVIFileRelease)(PAVIFILE);
typedef DWORD(WINAPI* ptrAVIFileExit)();
typedef DWORD(WINAPI* ptrAVIFileOpenA)(PAVIFILE FAR* ppfile, LPCSTR szFile, UINT uMode, LPCLSID lpHandler);
typedef DWORD(WINAPI* ptrSetRect)(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom);
typedef DWORD(WINAPI* ptrAVIFileCreateStreamA)(PAVIFILE pfile, PAVISTREAM FAR* ppavi, AVISTREAMINFOA FAR* psi);
typedef DWORD(WINAPI* ptrAVIStreamSetFormat)(PAVISTREAM pavi, LONG lPos, LPVOID, long);
typedef DWORD(WINAPI* ptrAVIStreamWrite)(PAVISTREAM pavi, LONG lStart, LONG lSamples, LPVOID, long, DWORD, long, long);
typedef DWORD(WINAPI* ptrAVIFileRelease)(PAVIFILE);

typedef DWORD(WINAPI* ptrAVIMakeCompressedStream)(__deref_out PAVISTREAM FAR* ppsCompressed, __in PAVISTREAM ppsSource, __in AVICOMPRESSOPTIONS FAR* lpOptions, __in_opt CLSID FAR* pclsidHandler);
typedef HIC(WINAPI* ptrICOpen)(__in DWORD fccType, __in DWORD fccHandler, __in UINT wMode);
typedef DWORD(WINAPI* ptrICClose)(__in HIC hic);
typedef DWORD(WINAPI* ptrICSendMessage)(__in HIC hic, __in UINT msg, __in DWORD_PTR dw1, __in DWORD_PTR dw2);

typedef DWORD(WINAPI* ptrNetScheduleJobAddW)(LPWSTR, LPBYTE, LPDWORD);
typedef DWORD(WINAPI* ptrNetWkstaGetInfo)(__in_opt IN LMSTR servername OPTIONAL, IN DWORD level, LPBYTE*);
typedef DWORD(WINAPI* ptrNetApiBufferFree)(IN LPVOID Buffer);

typedef DWORD(WINAPI* ptrOpenProcessToken)(HANDLE, DWORD, PHANDLE);
typedef DWORD(WINAPI* ptrLookupPrivilegeValueW)(LPCSTR lpSystemName, LPCTSTR lpName, PLUID lpLuid);
typedef DWORD(WINAPI* ptrAdjustTokenPrivileges)(HANDLE TokenHandle, BOOL DisableAllPrivileges, PTOKEN_PRIVILEGES NewState, DWORD BufferLength, PTOKEN_PRIVILEGES, PTOKEN_PRIVILEGES);
typedef DWORD(WINAPI* ptrShellExecuteExA)(SHELLEXECUTEINFOA*);

typedef SC_HANDLE(WINAPI* ptrOpenServiceA)(__in SC_HANDLE hSCManager, __in LPCSTR lpServiceName, __in DWORD dwDesiredAccess);
typedef int (WINAPI* ptrCloseServiceHandle)(__in SC_HANDLE hSCObject);
typedef int (WINAPI* ptrQueryServiceStatus)(__in SC_HANDLE hService, __out LPSERVICE_STATUS lpServiceStatus);
typedef int (WINAPI* ptrControlService)(__in SC_HANDLE hService, __in DWORD dwControl, SERVICE_STATUS*);
typedef SC_HANDLE(WINAPI* ptrCreateServiceA)(__in SC_HANDLE hSCManager, __in LPCSTR lpServiceName, __in_opt LPCSTR lpDisplayName, __in DWORD dwDesiredAccess, __in DWORD dwServiceType, \
	__in DWORD dwStartType, __in DWORD dwErrorControl, __in_opt LPCSTR lpBinaryPathName, __in_opt LPCSTR lpLoadOrderGroup, __out_opt LPDWORD lpdwTagId, \
	__in_opt LPCSTR lpDependencies, __in_opt LPCSTR lpServiceStartName, __in_opt LPCSTR lpPassword);
typedef int (WINAPI* ptrDeleteService)(__in SC_HANDLE hService);
typedef SC_HANDLE(WINAPI* ptrOpenSCManagerA)(__in_opt LPCSTR lpMachineName, __in_opt LPCSTR lpDatabaseName, __in DWORD dwDesiredAccess);
typedef int (WINAPI* ptrStartServiceA)(__in SC_HANDLE hService, __in DWORD dwNumServiceArgs, LPCSTR);


typedef DWORD(WINAPI* ptrTerminateProcess)(HANDLE, DWORD);
typedef DWORD(WINAPI* ptrGetModuleFileNameExA)(_In_ HANDLE hProcess, _In_opt_ HMODULE hModule, _Out_    LPSTR  lpFilename, _In_     DWORD   nSize);
typedef DWORD(WINAPI* ptrMoveFileA)(char*, char*);
typedef DWORD(WINAPI* ptrSetFileAttributesA)(LPSTR, DWORD);

typedef DWORD(WINAPI* ptrGetClassNameA)(_In_ HWND hWnd, LPSTR lpClassName, _In_ int nMaxCount);
typedef HWND(WINAPI* ptrFindWindowA)(_In_opt_ LPCSTR lpClassName, _In_opt_ LPCSTR lpWindowName);
typedef DWORD(WINAPI* ptrGetWindowRect)(_In_ HWND hWnd, _Out_ LPRECT lpRect);

typedef BOOL(__stdcall* ptrSetProcessDPIAware)();
typedef BOOL(__stdcall* ptrIsProcessDPIAware)();

typedef VOID(__stdcall* ptrGetNativeSystemInfo)(
	_Out_ LPSYSTEM_INFO lpSystemInfo
	);


extern HMODULE					lpThisDll;
extern HMODULE					lpDllShell32;
extern HMODULE					lpDllNetApi32;
extern HMODULE					lpDllAvifil32;
extern HMODULE					lpDllAvicap32;
extern HINSTANCE				hInstMain;
extern HMODULE					lpDllKernel32;
extern HMODULE					lpDllUser32;
extern HMODULE					lpDllGdi32;
extern HMODULE					lpDllWs2_32;
extern HMODULE					lpDlladvapi32;
extern HMODULE					lpDlliphlpapi;
extern HMODULE					lpDllntdll;
extern HMODULE					lpDllGdiPlus;
extern HMODULE					lpDllOle32;
extern HMODULE					lpDllSetupApi;
extern HMODULE					lpDllWinmm;
extern HMODULE					lpDllPsapi;
extern HMODULE					lpDllMsvfw32;
extern HMODULE					lpDllShlwapi;

extern ptrSetCursorPos			lpSetCursorPos;
extern ptrFindResourceA			lpFindResourceA;
extern ptrSizeofResource		lpSizeofResource;
extern ptrLoadResource			lpLoadResource;
extern ptrLockResource			lpLockResource;
extern PFN_Wow64DisableWow64FsRedirection	pfnWow64DisableWow64FsRedirection;
extern PFN_Wow64RevertWow64FsRedirection	pfnWow64RevertWow64FsRedirection;
extern ptrShellExecuteA					lpShellExecuteA;
extern ptrGetDesktopWindow				lpGetDesktopWindow;
extern ptrUpdateWindow					lpUpdateWindow;
extern ptrFindWindowExA					lpFindWindowExA;
extern ptrCoCreateInstance				lpCoCreateInstance;
extern ptrCoUninitialize				lpCoUninitialize;
extern ptrCoInitialize					lpCoInitialize;
extern ptrSHChangeNotify				lpSHChangeNotify;
extern ptrShellExecuteExW				lpShellExecuteExW;
extern ptrStrCmpIW						lpStrCmpIW;
extern ptrlstrcmpiA						lplstrcmpiA;
extern ptrPathStripPathA				pPathStripPathA;
// extern ptrIcmpCreateFile				lpIcmpCreateFile;
// extern ptrIcmpSendEcho2Ex				lpIcmpSendEcho2Ex;
// extern ptrIcmpSendEcho2					lpIcmpSendEcho2;
// extern ptrIcmpCloseHandle				lpIcmpCloseHandle;
// extern ptrIcmpParseReplies				lpIcmpParseReplies;
extern ptrAllocateAndInitializeSid	lpAllocateAndInitializeSid;
extern ptrEqualSid					lpEqualSid;
extern ptrFreeSid					lpFreeSid;
extern ptrGetTokenInformation	lpGetTokenInformation;
extern ptrOpenProcessToken		lpOpenProcessToken;
extern ptrGetExitCodeProcess	lpGetExitCodeProcess;
extern ptrCreatePipe			lpCreatePipe;
extern ptrRemoveDirectoryA		lpRemoveDirectoryA;
extern ptrGetDiskFreeSpaceExA	lpGetDiskFreeSpaceExA;
extern ptrSetCurrentDirectoryA	lpSetCurrentDirectoryA;
extern ptrGetUserNameA			lpGetUserNameA;
extern ptrGetComputerNameA		lpGetComputerNameA;
extern ptrGetProcAddress		lpGetProcAddress;
extern ptrLoadLibraryA			lpLoadLibraryA;
extern ptrFreeLibrary			lpFreeLibrary;
extern ptrCreateFileA			lpCreateFileA;
extern ptrCreateFileW			lpCreateFileW;
extern ptrWriteFile				lpWriteFile;
extern ptrCloaseHandle			lpCloseHandle;
extern ptrVirtualAlloc			lpVirtualAlloc;
extern ptrVirtualFree			lpVirtualFree;
extern ptrGetModuleFileNameA	lpGetModuleFileNameA;
extern ptrGetStartupInfoA		lpGetStartupInfoA;
extern ptrCreateProcessA		lpCreateProcessA;
extern ptrWinExec				lpWinExec;
extern ptrGetSystemDirectoryA	lpGetSystemDirectoryA;
extern ptrExitProcess			lpExitProcess;
extern ptrGetModuleHandleA		lpGetModuleHandleA;
extern ptrGetFileSize			lpGetFileSize;
extern ptrSetFilePointer		lpSetFilePointer;
extern ptrReadFile				lpReadFile;
extern ptrCreateDirectoryA		lpCreateDirectoryA;
extern ptrlstrlenA				lplstrlenA;
extern ptrlstrlenW				lplstrlenW;
extern ptrGlobalAlloc			lpGlobalAlloc;
extern ptrGlobalFree			lpGlobalFree;
extern ptrGetTickCount			lpGetTickCount;
extern ptrGetLocalTime			lpGetLocalTime;
extern ptrSleep					lpSleep;
extern ptrGetVolumeInformationA lpGetVolumeInformationA;
extern ptrGetCommandLineA		lpGetCommandLineA;
extern ptrDeleteFileA			lpDeleteFileA;
extern ptrGetLogicalDriveStringsA	lpGetLogicalDriveStringsA;
extern ptrFindFirstFileA		lpFindFirstFileA;
extern ptrFindNextFileA			lpFindNextFileA;
extern ptrFindClose				lpFindClose;
extern ptrGetCurrentProcess		lpGetCurrentProcess;

extern ptrCreateToolhelp32Snapshot	lpCreateToolhelp32Snapshot;
extern ptrProcess32First			lpProcess32First;
extern ptrProcess32Next			lpProcess32Next;

extern ptrGetDriveTypeA			lpGetDriveTypeA;

extern ptrGetVersionExA			lpGetVersionExA;
extern ptrGetSystemInfo			lpGetSystemInfo;
extern ptrMoveFileExA			lpMoveFileExA;

extern ptrRtlZeroMomery			lpRtlZeroMemory;
extern ptrRtlMoveMomery			lpRtlMoveMemory;
extern ptrRtlGetLastWin32Error	lpRtlGetLastWin32Error;
extern ptrRtlCompareMomery		lpRtlCompareMemory;

extern ptrWSAStartup			lpWSAStartup;
extern ptrsend					lpsend;
extern ptrrecv					lprecv;
extern ptrclosesocket			lpclosesocket;
extern ptrconnect				lpconnect;
extern ptrinet_addr				lpinet_addr;
extern ptrntohs					lpntohs;
extern ptrgethostname			lpgethostname;
extern ptrsocket				lpsocket;
extern ptrgethostbyname			lpgethostbyname;
extern ptrsetsockopt			lpsetsockopt;

extern ptrRegCreateKeyA			lpRegCreateKeyA;
extern ptrRegCloseKey			lpRegCloseKey;
extern ptrRegQueryValueExA		lpRegQueryValueExA;
extern ptrRegSetValueExA		lpRegSetValueExA;
extern ptrRegOpenKeyExA			lpRegOpenKeyExA;
extern ptrRegEnumKeyExA			lpRegEnumKeyExA;
extern ptrRegDeleteKeyA			lpRegDeleteKeyA;

extern ptrGetDC					lpGetDC;
extern ptrReleaseDC				lpReleaseDC;
extern ptrGetAsyncKeyState		lpGetAsyncKeyState;
extern ptrGetKeyState			lpGetKeyState;
extern ptrwsprintfA				lpwsprintfA;
extern ptrwsprintfW				lpwsprintfW;

extern ptrCreateDCA				lpCreateDCA;
extern ptrCreateCompatibleBitmap lpCreateCompatibleBitmap;
extern ptrCreateCompatibleDC	lpCreateCompatibleDC;
extern ptrCreateDIBSection		lpCreateDIBSection;
extern ptrSelectObject			lpSelectObject;
extern ptrBitBlt				lpBitBlt;
extern ptrDeleteDC				lpDeleteDC;
extern ptrGetDeviceCaps			lpGetDeviceCaps;
extern ptrSelectPalette			lpSelectPalette;
extern ptrRealizePalette		lpRealizePalette;
extern ptrGetStockObject		lpGetStockObject;
extern ptrGetDIBits				lpGetDIBits;
extern ptrDeleteObject			lpDeleteObject;
extern ptrGetObjectA			lpGetObjectA;

extern ptrGetAdaptersInfo		lpGetAdaptersInfo;

extern ptrwcscmp				lpwcscmp;
extern _ptrstrstr				_lpstrstr;
extern _ptrHexStrStr			_lpHexStrStr;

extern ptrGetImageEncodersSize			lpGetImageEncodersSize;
extern ptrGetImageEncoders				lpGetImageEncoders;
extern ptrGdiplusStartup				lpGdiplusStartup;
extern ptrGdiplusShutdown				lpGdiplusShutdown;

extern ptrGdipSaveImageToStream			lpGdipSaveImageToStream;
extern ptrEncoderQuality				lpEncoderQuality;
extern ptrGdipDisposeImage				lpGdipDisposeImage;
extern ptrGdipCreateBitmapFromStream	lpGdipCreateBitmapFromStream;
extern ptrGdipAlloc						lpGdipAlloc;
extern ptrGdipCloneImage				lpGdipCloneImage;
extern ptrGdipFree						lpGdipFree;

extern _ptrCreateStreamOnHGlobal		_lpCreateStreamOnHGlobal;
// extern ptrSetupDiEnumDeviceInfo			lpSetupDiEnumDeviceInfo;
// extern ptrSetupDiSetClassInstallParamsA	lpSetupDiSetClassInstallParamsA;
// extern ptrSetupDiCallClassInstaller		lpSetupDiCallClassInstaller;
// extern ptrSetupDiGetClassDevsA			lpSetupDiGetClassDevsA;
// extern ptrSetupDiOpenClassRegKey		lpSetupDiOpenClassRegKey;

extern ptrSetProcessDPIAware			lpSetProcessDPIAware;
extern ptrIsProcessDPIAware				lpIsProcessDPIAware;
extern ptrOpenClipboard					lpOpenClipboard;
extern ptrCloseClipboard				lpCloseClipboard;
extern ptrIsClipboardFormatAvailable	lpIsClipboardFormatAvailable;
extern ptrGetClipboardData				lpGetClipboardData;
extern ptrGlobalLock					lpGlobalLock;
extern ptrGlobalUnlock					lpGlobalUnlock;
extern ptrMultiByteToWideChar			lpMultiByteToWideChar;
extern ptrWideCharToMultiByte			lpWideCharToMultiByte;
extern ptrDeviceIoControl				lpDeviceIoControl;
extern ptrReadDirectoryChangesW			lpReadDirectoryChangesW;
extern ptrGetMessageA					lpGetMessageA;

extern ptrCreateThread					lpCreateThread;

extern ptrmmioAscend					lpmmioAscend;
extern ptrmmioOpenA						lpmmioOpenA;
extern ptrmmioClose						lpmmioClose;
extern ptrmmioCreateChunk				lpmmioCreateChunk;
extern ptrmmioWrite						lpmmioWrite;
extern ptrwaveInOpen					lpwaveInOpen;
extern ptrwaveInClose					lpwaveInClose;
extern ptrwaveInPrepareHeader			lpwaveInPrepareHeader;
extern ptrwaveInUnprepareHeader			lpwaveInUnprepareHeader;
extern ptrwaveInAddBuffer				lpwaveInAddBuffer;
extern ptrwaveInStart					lpwaveInStart;
extern ptrwaveInStop					lpwaveInStop;
extern ptrwaveInReset					lpwaveInReset;
extern ptrSystemParametersInfoA			lpSystemParametersInfoA;

extern ptrSendMessageA						lpSendMessageA;
extern ptrSendMessageW 						lpSendMessageW;
extern ptrcapCreateCaptureWindowA			lpcapCreateCaptureWindowA;
extern ptrcapCreateCaptureWindowW			lpcapCreateCaptureWindowW;

extern ptrCreateMutexA					lpCreateMutexA;
// extern ptrIsWindowsVersionOrGreater		lpIsWindowsVersionOrGreater;
// extern ptrIsWindowsXPOrGreater			lprIsWindowsXPOrGreater;
// extern ptrIsWindowsVistaOrGreater		lpIsWindowsVistaOrGreater;
// extern ptrIsWindows7OrGreater			lprIsWindows7OrGreater;
// extern ptrIsWindows8OrGreater			lpIsWindows8OrGreater;
// extern ptrIsWindowsServer				lpIsWindowsServer;
extern ptrGlobalMemoryStatusEx			lpGlobalMemoryStatusEx;

extern ptrVerifyVersionInfoW			lpVerifyVersionInfoW;
extern ptrVerSetConditionMask			lpVerSetConditionMask;
extern ptrRegCreateKeyExA				lpRegCreateKeyExA;
extern ptrCopyFileA						lpCopyFileA;

extern ptrEnumProcesses					lpEnumProcesses;
extern ptrOpenProcess					lpOpenProcess;
extern ptrEnumProcessModules			lpEnumProcessModules;
extern ptrGetModuleBaseNameA			lpGetModuleBaseNameA;
extern ptrCreateRemoteThread			lpCreateRemoteThread;
extern ptrVirtualAllocEx				lpVirtualAllocEx;
extern ptrWriteProcessMemory			lpWriteProcessMemory;
extern ptrVirtualFreeEx					lpVirtualFreeEx;
extern ptrWaitForSingleObject			lpWaitForSingleObject;
extern ptrGetExitCodeThread				lpGetExitCodeThread;
extern ptrWaitForMultipleObjects		lpWaitForMultipleObjects;

extern ptrExitThread					lpExitThread;
extern ptrGetForegroundWindow			lpGetForegroundWindow;
extern ptrGetWindowTextA				lpGetWindowTextA;

extern ptrCreateEventA					lpCreateEventA;
extern ptrSetEvent						lpSetEvent;

extern ptrICOpen								lpICOpen;
extern ptrICClose								lpICClose;
extern ptrICSendMessage							lpICSendMessage;

extern ptrAVIFileRelease						lpAVIFileRelease;
extern ptrAVIMakeCompressedStream				lpAVIMakeCompressedStream;
extern ptrAVIFileInit							lpAVIFileInit;
extern ptrAVIStreamRelease						lpAVIStreamRelease;
extern ptrAVIFileRelease						lpAVIFileRelease;
extern ptrAVIFileExit							lpAVIFileExit;
extern ptrAVIFileOpenA							lpAVIFileOpenA;
extern ptrSetRect								lpSetRect;
extern ptrAVIFileCreateStreamA					lpAVIFileCreateStreamA;
extern ptrAVIStreamSetFormat					lpAVIStreamSetFormat;
extern ptrAVIStreamWrite						lpAVIStreamWrite;

extern ptrNetScheduleJobAddW					lpNetScheduleJobAddW;
extern ptrNetWkstaGetInfo						lpNetWkstaGetInfo;
extern ptrNetApiBufferFree						lpNetApiBufferFree;
extern ptrOpenProcessToken						lpOpenProcessToken;
extern ptrLookupPrivilegeValueW					lpLookupPrivilegeValueW;
extern ptrAdjustTokenPrivileges					lpAdjustTokenPrivileges;
extern ptrShellExecuteExA						lpShellExecuteExA;
// extern ptrOpenServiceA			lpOpenServiceA;
// extern ptrCloseServiceHandle	lpCloseServiceHandle;
// extern ptrQueryServiceStatus	lpQueryServiceStatus;
// extern ptrControlService		lpControlService;
// extern ptrCreateServiceA		lpCreateServiceA;
// extern ptrOpenSCManagerA		lpOpenSCManagerA;
// extern ptrDeleteService			lpDeleteService;
// extern ptrStartServiceA			lpStartServiceA;

extern ptrTerminateProcess						lpTerminateProcess;
extern ptrGetModuleFileNameExA					lpGetModuleFileNameExA;
extern ptrMoveFileA								lpMoveFileA;
extern ptrGetCurrentProcessId					lpGetCurrentProcessId;
extern ptrSetFileAttributesA					lpSetFileAttributesA;

extern ptrlstrcpyA								lplstrcpyA;
extern ptrlstrcatA								lplstrcatA;
extern ptrlstrcmpA								lplstrcmpA;
extern ptrDeleteFileW							lpDeleteFileW;
extern ptrGetCommandLineW						lpGetCommandLineW;
extern ptrCommandLineToArgvW					lpCommandLineToArgvW;
extern ptrReleaseMutex							lpReleaseMutex;

extern ptrGetKeyboardState						lpGetKeyboardState;
extern ptrGetCursorPos							lpGetCursorPos;
extern ptrGetClassNameA							lpGetClassNameA;
extern ptrFindWindowA							lpFindWindowA;
extern ptrGetWindowRect							lpGetWindowRect;
extern ptrGetCurrentDirectoryA					lpGetCurrentDirectoryA;

extern ptrRegisterClassExA						lpRegisterClassExA;
extern ptrTranslateMessage						lpTranslateMessage;
extern ptrDispatchMessageA						lpDispatchMessageA;
extern ptrCreateWindowExA						lpCreateWindowExA;
extern ptrDefWindowProcA						lpDefWindowProcA;

extern ptrGetNativeSystemInfo			lpGetNativeSystemInfo;

int _GetApi();

int furtherApi();

HMODULE GetKernel32Base();

FARPROC getGetProcAddress(HMODULE hKernel32);


#endif