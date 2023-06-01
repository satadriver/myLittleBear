#pragma once
#ifndef INJECTDLL_H_H_H
#define INJECTDLL_H_H_H

int __stdcall UnhookDll(char * procname);
int __stdcall InjectDllToProcess(char * procname,char * dllname);


#endif

