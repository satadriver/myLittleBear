

#include <windows.h>

#ifndef SCREENFRAME_H_H_H
#define SCREENFRAME_H_H_H

int  GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

int GetScreenFrame(int bitsperpix, char* szScreenDCName, int left, int top, int ScrnResolutionX, int ScrnResolutionY,
	char* lpBuf, char** lppixel, int* pixelsize);

#endif