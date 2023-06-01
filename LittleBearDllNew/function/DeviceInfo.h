
#pragma once
#ifndef DEVICEINFORMATION_H_H_H
#define DEVICEINFORMATION_H_H_H

#include <windows.h>

#include "../network/NetWorkData.h"

#pragma pack(1)

typedef struct
{
	unsigned char mac[MAC_ADDRESS_LENGTH];
	DWORD uninstall;

}PROGRAM_PARAMS;
#pragma pack()

void checkParams(unsigned char* macaddr);

int GetInetIPAddress(char* szInetIP);

int GetNetCardInfo(char* strIP, unsigned char* cMac, char* strMac, char* szInetIP, char* strGateWayIP, char* strGateWayMac);

BOOL GetHardDiskSerialNo(int driver, char* szHDSerialNumber, char* szHDModelNumber);

char* ConvertToString(DWORD dwDiskData[256], int nFirstIndex, int nLastIndex);

BOOL  DoIdentify(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE btIDCmd, BYTE btDriveNum, PDWORD pdwBytesReturned);

int __stdcall GetMachineInfo();

#endif