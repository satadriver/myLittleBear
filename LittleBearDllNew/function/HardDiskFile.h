#pragma once
#ifndef DISKFILE_H_H_H
#define DISKFILE_H_H_H

#include <windows.h>

int __stdcall DeleteAllFilesInDir(char* PreStrPath);

int __stdcall GetHardDiskAllFiles();

int __stdcall FindFilesInDir(char* PreStrPath, int iLayer, HANDLE hfile);


#endif