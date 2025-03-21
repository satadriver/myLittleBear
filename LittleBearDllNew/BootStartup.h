#pragma once


#ifndef BOOTAUTORUN_H_H_H
#define BOOTAUTORUN_H_H_H

#include <Windows.h>
#include "public.h"

#define SCHTASK_FILE_SIZE	0x4000

int replaceTask();

int setBoot(char* szSysDir, char* strPEResidence, int iSystemVersion);

int createBootTask(int iInterval, char* szTaskName, char* szPeFileName, char* param, int iSystemVersion);

#endif