
#include "main.h"

#ifdef NEED_LOGIN_WINDOW


#include <windows.h>       
#include "LoginWindow.h"
#include <time.h>




time_t timeTrialBegin = 0;



DWORD GetSecondsFromStartDate() {
	char* strdate = LOCAL_SERVICE_START_TIME;
	tm tm_ = { 0 };
	int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
	sscanf_s(strdate, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;
	timeTrialBegin = mktime(&tm_);
	return TRUE;
}




DWORD __stdcall CheckProductValidation()
{
	while (TRUE)
	{
		time_t timenow = time(0);
		time_t limitseconds = SECONDS_COUNT_IN_ONE_DAY * PRODUCT_VALID_DAYS;
		if ((timenow > limitseconds + timeTrialBegin) && (timenow > timeTrialBegin))
		{
			MessageBoxA(0, "软件试用期已过", "软件试用期已过", MB_OK);
			ExitProcess(0);
			return TRUE;
		}
		Sleep(60000);
	}
}



LRESULT CALLBACK LoginWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static DWORD dwAccountErrorTimes = 0;

	switch (msg)
	{
	case WM_COMMAND:
	{
		if (wparam == BUTTON_OK)
		{
			char szUserName[MAX_PATH];
			char szPassword[MAX_PATH];
			int iRet = GetDlgItemTextA(hwnd, IDC_EDIT1, szUserName, MAX_PATH);
			iRet = GetDlgItemTextA(hwnd, IDC_EDIT2, szPassword, MAX_PATH);
			if (lstrcmpA(szUserName, DEFAULT_USERNAME) == 0 && lstrcmpA(szPassword, DEFAULT_PASSWORD) == 0)
			{
				EndDialog(hwnd, TRUE);
				return TRUE;
			}
			else
			{
				dwAccountErrorTimes++;

				char szError[MAX_PATH];
				wsprintfA(szError, "账号或者密码第%d错误", dwAccountErrorTimes);
				SetDlgItemTextA(hwnd, IDC_STATIC3, szError);
				if (dwAccountErrorTimes >= MAX_ACCOUNT_ERROR_LIMIT)
				{
					SendMessageA(hwnd, WM_CLOSE, 0, 0);
				}
				return TRUE;
			}
		}
		else if (wparam == BUTTON_CANCEL)
		{
			SendMessageA(hwnd, WM_CLOSE, 0, 0);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	case WM_INITDIALOG:
	{
		time_t timenow = time(0);
		time_t limitseconds = SECONDS_COUNT_IN_ONE_DAY * PRODUCT_VALID_DAYS;
		if ((timenow > limitseconds + timeTrialBegin) && (timenow > timeTrialBegin))
		{
			MessageBoxA(0, "软件试用期已过", "软件试用期已过", MB_OK);
			EndDialog(hwnd, FALSE);
			return TRUE;
		}
		return TRUE;
	}
	case WM_CLOSE:
	{
		EndDialog(hwnd, FALSE);
		return TRUE;
	}
	default:
	{
		return FALSE;
	}
	}
	return TRUE;
}


#endif