#pragma once
#ifndef SCREENSNAPSHOT_H_H_H
#define SCREENSNAPSHOT_H_H_H

#include <windows.h>
#include "../network/NetWorkData.h"

#define SREENSNAPSHOT_WAIT_TIME_INTERVAL		1000
#define VIRTUAL_KEY_SIZE						256

int __stdcall GetScreenSnapshot(LPSCREENSNAPSHOT_TIME lpstTime);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

#endif