#pragma once


int WriteLog(char * pLog);
int GetPathFromFullName(char* strFullName, char * strDst);
int GetMainNameFormName(char * szName, char * szMainName);
int GetNameFromFullName(char * strFullName, char * dstname);
int FindExeFile(char * strpath, char * lpfn);
int MainProc();