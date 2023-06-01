#pragma once


void CryptData(unsigned char * pdata, int size, unsigned char * pkey, int keylen);
int GetUserAndComputerName(char * szUsername, char * szComputerName);
int GetWindowsVersion();
int fileWriter(const char * fn, char * lpbuf, int filesize, int append);
int fileReader(char * fn, char ** lpbuf, int * filesize);
int release(unsigned long inst);

int runOther();

extern unsigned long gHinstance;