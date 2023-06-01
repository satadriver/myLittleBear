
//#include <windows.h>


#define WCHAR wchar_t;
#define LPCSTR const char*
#define LPSTR char*
#define DWORD unsigned long
#define LPCWSTR const wchar_t *
#define LPWSTR wchar_t *
#define HANDLE void*
#define BOOL int
#define TRUE 1
#define FALSE 0
#define VOID void
#define LPDWORD DWORD *
#define LPVOID void *
#define PUINT unsigned int*
#define LPCVOID const void *




// extern "C" __declspec(dllexport) char *VerLanguageNameA = "KERNEL32.VerLanguageNameA";
// extern "C" __declspec(dllexport) char *VerLanguageNameW = "KERNEL32.VerLanguageNameW";
// extern "C" __declspec(dllexport) BOOL __stdcall GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
// extern "C" __declspec(dllexport) int __stdcall GetFileVersionInfoByHandle(void *Src, HANDLE hFile, int a3, int a4);
// extern "C" __declspec(dllexport) BOOL __stdcall GetFileVersionInfoExW(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
// extern "C" __declspec(dllexport) DWORD __stdcall GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle);
// extern "C" __declspec(dllexport) DWORD __stdcall GetFileVersionInfoSizeExW(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle);
// extern "C" __declspec(dllexport) DWORD __stdcall GetFileVersionInfoSizeW(LPCWSTR lptstrFilename, LPDWORD lpdwHandle);
// extern "C" __declspec(dllexport) BOOL __stdcall GetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
// extern "C" __declspec(dllexport) DWORD __stdcall VerFindFileA(DWORD uFlags, LPCSTR szFileName, LPCSTR szWinDir, LPCSTR szAppDir, LPSTR szCurDir, PUINT lpuCurDirLen, LPSTR szDestDir, PUINT lpuDestDirLen);
// extern "C" __declspec(dllexport) DWORD __stdcall VerFindFileW(DWORD uFlags, LPCWSTR szFileName, LPCWSTR szWinDir, LPCWSTR szAppDir, LPWSTR szCurDir, PUINT lpuCurDirLen, LPWSTR szDestDir, PUINT lpuDestDirLen);
// extern "C" __declspec(dllexport) DWORD __stdcall VerInstallFileA(DWORD uFlags, LPCSTR szSrcFileName, LPCSTR szDestFileName, LPCSTR szSrcDir, LPCSTR szDestDir, LPCSTR szCurDir, LPSTR szTmpFile, PUINT lpuTmpFileLen);
// extern "C" __declspec(dllexport) DWORD __stdcall VerInstallFileW(DWORD uFlags, LPCWSTR szSrcFileName, LPCWSTR szDestFileName, LPCWSTR szSrcDir, LPCWSTR szDestDir, LPCWSTR szCurDir, LPWSTR szTmpFile, PUINT lpuTmpFileLen);
// extern "C" __declspec(dllexport) BOOL __stdcall VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen);
// extern "C" __declspec(dllexport) BOOL __stdcall VerQueryValueW(LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen);
