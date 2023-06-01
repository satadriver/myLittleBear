
#include "main.h"


#ifdef NEED_LOGIN_WINDOW
#ifndef LOGINWINDOWS_H_H_H
#define LOGINWINDOWS_H_H_H

#include <time.h>
#include <windows.h>

#define IDD_DIALOG1 0x1000
#define IDC_EDIT1 0x1001
#define IDC_EDIT2 0x1002
#define IDC_STATIC1 0x1003
#define IDC_STATIC2 0x1004
#define BUTTON_OK 0x1005
#define BUTTON_CANCEL 0x1006
#define IDC_STATIC3 0x1007

#define MAX_ACCOUNT_ERROR_LIMIT		3
#define DEFAULT_PASSWORD			"12345678"
#define DEFAULT_USERNAME			"username"
#define SECONDS_COUNT_IN_ONE_DAY	86400
#define LOCAL_SERVICE_START_TIME	"2018-5-1 00:00:00"
#define PRODUCT_VALID_DAYS			60

extern time_t timeTrialBegin;
LRESULT CALLBACK LoginWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
DWORD GetSecondsFromStartDate();
DWORD __stdcall CheckProductValidation();

#endif

#endif