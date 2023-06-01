

#ifndef REGHELPER_H_H_H
#define REGHELPER_H_H_H

#include <windows.h>


DWORD __SetRegistryKeyValueChar(HKEY hMainKey, char* szSubKey, char* szKeyName, char* szKeyValue);

DWORD __SetRegistryKeyValueDword(HKEY hMainKey, char* szSubKey, char* szKeyName, DWORD dwKeyValue);

int SetBootForRegRun(HKEY hMainKey, char* strPEResidence);

DWORD QueryRegistryValue(HKEY hMainKey, char* szSubKey, char* szKeyName, unsigned char* szKeyValue, int* buflen);

#endif