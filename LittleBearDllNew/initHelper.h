#pragma once

#ifndef INITHELPER_H_H_H
#define INITHELPER_H_H_H

#include <windows.h>

int  isDebugged();

HANDLE  CheckInstanceExist(BOOL* exist);

int initDisplay(int* JPG_STREAM_SIZE, int* BMP_STREAM_SIZE, int* REMOTECONTROL_BUFFER_SIZE);

int getRunningPath();

int createDataPath();

int initSocket();

#endif
