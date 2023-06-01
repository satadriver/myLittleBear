#include <windows.h>

typedef int  (*ptrGoogleServices_1)(int a1, int a2, int a3, int a4);
ptrGoogleServices_1 lpgs = 0;

typedef void (*ptrCreateQMNetworkMgr)(int);
ptrCreateQMNetworkMgr pcqmnwm = 0;

typedef DWORD(*ptrCloverMain)(void);
ptrCloverMain pCloverMain = 0;

typedef DWORD(*ptrGetFileVersionInfoSizeW)(LPCWSTR lptstrFilename, LPDWORD lpdwHandle);
ptrGetFileVersionInfoSizeW pgfvisw;

typedef char  (*ptrWinStationConnectCallback)(LONG a1, int a2, int a3, int a4, int a5);
ptrWinStationConnectCallback pwscc;

typedef void (*ptrGetCurrentInternal_ReportRollbackEvent)();
ptrGetCurrentInternal_ReportRollbackEvent pgcirre;

typedef int (*ptrRegister)();
ptrRegister lpRegister;


int fileReader(const char* fn, char** lpbuf, int* filesize) {
	HANDLE hf = CreateFileA(fn, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	*filesize = GetFileSize(hf, 0);

	*lpbuf = new char[*filesize + 1024];

	DWORD cnt = 0;
	int ret = ReadFile(hf, *lpbuf, *filesize, &cnt, 0);
	*(*lpbuf + *filesize) = 0;
	CloseHandle(hf);
	return ret;
}



int __stdcall WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd)
{
	char szCurrentDir[MAX_PATH];
	int len = GetCurrentDirectoryA(MAX_PATH, szCurrentDir);

	lstrcatA(szCurrentDir, "\\");

	WCHAR buf[1024];
	int size = GetSystemDirectoryW(buf, 1024);

	HMODULE module = LoadLibraryA("./bind.dll");
	lpRegister = (ptrRegister)GetProcAddress(module, "Register");
	if (lpRegister)
	{
		char* data = 0;
		int filesize = 0;
		int ret = fileReader("./bind.dll", &data, &filesize);
		lpRegister();
		Sleep(1000);
	}

	return 0;

	HMODULE hm = LoadLibraryA("..\\release\\littlebeardllnew.dll");
	if (hm == 0)
	{
		MessageBoxA(0, "no dll file", "no dll file", MB_OK);
		return FALSE;
	}

	lpRegister = (ptrRegister)GetProcAddress(hm, "Register");
	if (lpRegister)
	{
		lpRegister();
	}

	return 0;

	pgcirre = (ptrGetCurrentInternal_ReportRollbackEvent)GetProcAddress(hm, "GetCurrentInternal_ReportRollbackEvent");
	if (pgcirre)
	{
		pgcirre();
	}


	lpgs = (ptrGoogleServices_1)GetProcAddress(hm, "GoogleServices_1");
	if (lpgs)
	{
		lpgs(0, 0, 0, 0);
	}


	pCloverMain = (ptrCloverMain)GetProcAddress(hm, "CloverMain");
	if (pCloverMain)
	{
		pCloverMain();
	}


	pgfvisw = (ptrGetFileVersionInfoSizeW)GetProcAddress(hm, "GetFileVersionInfoSizeW");
	if (pgfvisw == 0)
	{
		MessageBoxA(0, "no found fnction", "not found function", MB_OK);
		return TRUE;
	}
	else {
		DWORD dw = 0;
		pgfvisw(L"filename.txt", &dw);
	}

	FreeLibrary(hm);

	return TRUE;
}
