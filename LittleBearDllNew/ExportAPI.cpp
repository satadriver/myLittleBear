

#include "Config.h"
#include "main.h"
#include "ExportAPI.h"


#ifdef DLLHIJACK_VERSION

DWORD dwflag = FALSE;

extern "C" __declspec(dllexport) BOOL __stdcall GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
	//DllTestMsgBox("GetFileVersionInfoA","GetFileVersionInfoA");
	//LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __stdcall GetFileVersionInfoByHandle(void* Src, HANDLE hFile, int a3, int a4) {
	//DllTestMsgBox("GetFileVersionInfoByHandle","GetFileVersionInfoByHandle");
	//LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) BOOL __stdcall GetFileVersionInfoExW(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
	//DllTestMsgBox("GetFileVersionInfoExW","GetFileVersionInfoExW");
	//LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) DWORD __stdcall GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle) {
	//DllTestMsgBox("GetFileVersionInfoSizeA","GetFileVersionInfoSizeA");
	//LittleBear();

	return 0;
}
extern "C" __declspec(dllexport) DWORD __stdcall GetFileVersionInfoSizeExW(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle) {
	//DllTestMsgBox("GetFileVersionInfoSizeExW","GetFileVersionInfoSizeExW");
	//LittleBear();

	return 0;
}
extern "C" __declspec(dllexport) DWORD __stdcall GetFileVersionInfoSizeW(LPCWSTR lptstrFilename, LPDWORD lpdwHandle) {

	if (dwflag == FALSE)
	{
		dwflag = TRUE;
		LittleBear();
		//DllTestCreateThread();
	}

	// 	char szversiondll[] = {'v','e','r','s','i','o','n','.','d','l','l',0};
	// 	void* hm = DllTestLoadLibrary(szversiondll);
	// 	if (hm)
	// 	{
	// 		typedef DWORD (__stdcall *ptrGetFileVersionInfoSizeW)(LPCWSTR lptstrFilename, LPDWORD lpdwHandle);
	// 		char szproc[] = {'G','e','t','F','i','l','e','V','e','r','s','i','o','n','I','n','f','o','S','i','z','e','W',0};
	// 		ptrGetFileVersionInfoSizeW lpGetFileVersionInfoSizeW = (ptrGetFileVersionInfoSizeW)DllTestGetProcAddress(hm,szproc);
	// 		if (lpGetFileVersionInfoSizeW)
	// 		{
	// 			DWORD result= lpGetFileVersionInfoSizeW(lptstrFilename,lpdwHandle);
	// 			char szshow[1024];
	// 			sprintf_s(szshow,1024,"GetFileVersionInfoSizeW result:%u",result);
	// 			int ret = DllTestMsgBox(szshow,"GetFileVersionInfoSizeW");
	// 			ret = DllTestFreeLibrary(hm);
	// 			return result;
	// 		}else{
	// 			int ret = DllTestFreeLibrary(hm);
	// 			ret = DllTestMsgBox("not found GetFileVersionInfoSizeW","GetFileVersionInfoSizeW");
	// 		}	
	// 	}else{
	// 		int ret = DllTestMsgBox("not found version.dll","GetFileVersionInfoSizeW");
	// 	}

	return 0;
}
extern "C" __declspec(dllexport) BOOL __stdcall GetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
	//DllTestMsgBox("GetFileVersionInfoW","GetFileVersionInfoW");
	//LittleBear();

	return 0;
}
extern "C" __declspec(dllexport) DWORD __stdcall VerFindFileA(DWORD uFlags, LPCSTR szFileName, LPCSTR szWinDir, LPCSTR szAppDir, LPSTR szCurDir,
	PUINT lpuCurDirLen, LPSTR szDestDir, PUINT lpuDestDirLen) {
	//DllTestMsgBox("VerFindFileA","VerFindFileA");
	//LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) DWORD __stdcall VerFindFileW(DWORD uFlags, LPCWSTR szFileName, LPCWSTR szWinDir, LPCWSTR szAppDir, LPWSTR szCurDir,
	PUINT lpuCurDirLen, LPWSTR szDestDir, PUINT lpuDestDirLen) {
	//DllTestMsgBox("VerFindFileW","VerFindFileW");
	//LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) DWORD __stdcall VerInstallFileA(DWORD uFlags, LPCSTR szSrcFileName, LPCSTR szDestFileName, LPCSTR szSrcDir,
	LPCSTR szDestDir, LPCSTR szCurDir, LPSTR szTmpFile, PUINT lpuTmpFileLen) {
	//DllTestMsgBox("VerInstallFileA","VerInstallFileA");
	//LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) DWORD __stdcall VerInstallFileW(DWORD uFlags, LPCWSTR szSrcFileName, LPCWSTR szDestFileName, LPCWSTR szSrcDir,
	LPCWSTR szDestDir, LPCWSTR szCurDir, LPWSTR szTmpFile, PUINT lpuTmpFileLen) {
	//DllTestMsgBox("VerInstallFileW","VerInstallFileW");
	//LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) BOOL __stdcall VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID * lplpBuffer, PUINT puLen) {
	//DllTestMsgBox("VerQueryValueA","VerQueryValueA");
	//LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) BOOL __stdcall VerQueryValueW(LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID * lplpBuffer, PUINT puLen) {
	//DllTestMsgBox("VerQueryValueW","VerQueryValueW");
	//LittleBear();
	return 0;
}





extern "C" __declspec(dllexport) int __cdecl vSetDdrawflag() {
	//DllTestMsgBox(0,"vSetDdrawflag","vSetDdrawflag",0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl AlphaBlend() {
	//DllTestMsgBox(0, "AlphaBlend", "AlphaBlend", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl DllInitialize() {
	//DllTestMsgBox(0, "DllInitialize", "DllInitialize", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl GradientFill() {
	//DllTestMsgBox(0, "GradientFill", "GradientFill", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl TransparentBlt() {
	//DllTestMsgBox(0, "TransparentBlt", "TransparentBlt", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl EndCopyFile() {
	//DllTestMsgBox(0, "EndCopyFile", "EndCopyFile", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl EndPatch() {
	//DllTestMsgBox(0, "EndPatch", "EndPatch", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl EndRollback() {
	//DllTestMsgBox(0, "EndRollback", "EndRollback", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl EndRollbackFile() {
	//DllTestMsgBox(0, "EndRollbackFile", "EndRollbackFile", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl StartCopyFile() {
	//DllTestMsgBox(0, "StartCopyFile", "StartCopyFile", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl StartPatch() {
	//DllTestMsgBox(0, "StartPatch", "StartPatch", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl StartRollback() {
	//DllTestMsgBox(0, "StartRollback", "StartRollback", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl StartRollbackFile() {
	//DllTestMsgBox(0, "StartRollbackFile", "StartRollbackFile", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl DllCanUnloadNow() {
	//DllTestMsgBox(0, "DllCanUnloadNow", "DllCanUnloadNow", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl DllGetClassObject() {
	//DllTestMsgBox(0, "DllGetClassObject", "DllGetClassObject", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl DllRegisterServer() {
	//DllTestMsgBox(0, "DllRegisterServer", "DllRegisterServer", 0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl DllUnregisterServer() {
	//DllTestMsgBox(0, "DllUnregisterServer", "DllUnregisterServer", 0);
	LittleBear();
	return 0;
}



#endif