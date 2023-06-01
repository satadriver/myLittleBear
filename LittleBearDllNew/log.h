#pragma once

#ifndef LOG_H_H_H
#define LOG_H_H_H

#include <windows.h>

int __cdecl log(const WCHAR* format, ...);

int __cdecl log(const CHAR* format, ...);

//int writeLog(char* pLog);

int __cdecl writeLog(char* format, ...);

int writeRawLog(char* pLog);

#endif