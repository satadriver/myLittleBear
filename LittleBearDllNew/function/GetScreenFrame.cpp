#include "../api.h"
#include "../public.h"
#include <windows.h>
#include "GetScreenFrame.h"
#include "../log.h"



int GetScreenFrame(int bitsperpix, char* szScreenDCName, int left, int top, int ScrnResolutionX, int ScrnResolutionY,
	char* lpBuf, char** lppixel, int* pixelsize) {

	HDC hdc = lpCreateDCA(szScreenDCName, NULL, NULL, NULL);
	if (hdc == 0)
	{
		writeLog("GetScreenFrame lpCreateDCA error\r\n");
		return FALSE;
	}
	HDC hmemdc = lpCreateCompatibleDC(hdc);
	if (hmemdc == 0)
	{
		writeLog("GetScreenFrame lpCreateCompatibleDC error\r\n");
		lpDeleteDC(hdc);
		return FALSE;
	}

	int xscrn = ScrnResolutionX;
	int yscrn = ScrnResolutionY;
	HBITMAP hbitmap = lpCreateCompatibleBitmap(hdc, xscrn, yscrn);
	if (hbitmap == 0)
	{
		writeLog("GetScreenFrame lpCreateCompatibleBitmap error\r\n");
		lpDeleteDC(hdc);
		lpDeleteDC(hmemdc);
		return FALSE;
	}

	HBITMAP holdbitmap = (HBITMAP)lpSelectObject(hmemdc, hbitmap);
	int iRes = lpBitBlt(hmemdc, 0, 0, xscrn, yscrn, hdc, left, top, SRCCOPY);
	if (iRes == 0)
	{
		writeLog("lpBitBlt error\r\n");
		lpDeleteObject(holdbitmap);
		lpDeleteObject(hbitmap);
		lpDeleteDC(hdc);
		lpDeleteDC(hmemdc);
		return FALSE;
	}

	lpDeleteObject(holdbitmap);
	lpDeleteDC(hmemdc);
	lpDeleteDC(hdc);

	int wbitcount = 0;
	int ibits = bitsperpix;
	if (ibits <= 1) {
		wbitcount = 1;
	}
	else if (ibits <= 4) {
		wbitcount = 4;
	}
	else if (ibits <= 8) {
		wbitcount = 8;
	}
	else if (ibits <= 16) {
		wbitcount = 16;
	}
	else if (ibits <= 24) {
		wbitcount = 24;
	}
	else {
		wbitcount = 32;
	}

	DWORD dwpalettesize = 0;
	if (wbitcount <= 8)
	{
		dwpalettesize = (1 << wbitcount) * sizeof(RGBQUAD);
	}

	BITMAP bitmap = { 0 };
	iRes = lpGetObjectA(hbitmap, sizeof(BITMAP), (LPSTR)&bitmap);
	if (iRes == 0)
	{
		writeLog("lpGetObjectA error\r\n");
		lpDeleteObject(hbitmap);
		return FALSE;
	}

	DWORD dwbmbitssize = ((bitmap.bmWidth * wbitcount + 31) / 32) * 4 * bitmap.bmHeight;
	DWORD dwBufSize = dwbmbitssize + dwpalettesize + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

	LPBITMAPFILEHEADER bmfhdr = (LPBITMAPFILEHEADER)lpBuf;
	bmfhdr->bfType = 0x4d42;
	bmfhdr->bfSize = dwBufSize;
	bmfhdr->bfReserved1 = 0;
	bmfhdr->bfReserved2 = 0;
	bmfhdr->bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwpalettesize;

	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)(lpBuf + sizeof(BITMAPFILEHEADER));
	lpbi->biSize = sizeof(BITMAPINFOHEADER);
	lpbi->biWidth = bitmap.bmWidth;
	lpbi->biHeight = bitmap.bmHeight;
	lpbi->biPlanes = 1;
	lpbi->biBitCount = wbitcount;
	lpbi->biCompression = BI_RGB;
	lpbi->biSizeImage = 0;
	lpbi->biXPelsPerMeter = 0;
	lpbi->biYPelsPerMeter = 0;
	lpbi->biClrUsed = 0;
	lpbi->biClrImportant = 0;

	HANDLE hpal = lpGetStockObject(DEFAULT_PALETTE);
	hdc = lpGetDC(NULL);
	HANDLE holdpal = 0;
	if (hpal)
	{
		holdpal = lpSelectPalette(hdc, (HPALETTE)hpal, FALSE);
		lpRealizePalette(hdc);
	}

	char* lpData = lpBuf + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + dwpalettesize;
	iRes = lpGetDIBits(hdc, hbitmap, 0, bitmap.bmHeight, lpData, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
	if (iRes == 0)
	{
		writeLog("lpGetDIBits error\r\n");
		lpReleaseDC(NULL, hdc);
		lpDeleteObject(hbitmap);
		lpDeleteObject(holdpal);
		lpDeleteObject(hpal);
		return FALSE;
	}

	if (holdpal)
	{
		lpSelectPalette(hdc, (HPALETTE)holdpal, TRUE);
		lpRealizePalette(hdc);
		lpDeleteObject(holdpal);
	}
	lpDeleteObject(hbitmap);
	lpDeleteObject(hpal);
	lpReleaseDC(NULL, hdc);

	*lppixel = lpData;
	*pixelsize = dwbmbitssize;
	return dwBufSize;
}


