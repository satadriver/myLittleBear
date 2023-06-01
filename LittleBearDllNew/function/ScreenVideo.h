#pragma once
#ifndef SCREENVIDEO_H_H_H
#define SCREENVIDEO_H_H_H

#include <windows.h>

int __stdcall GetScreenVideo(unsigned long TimeDelay);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

#endif