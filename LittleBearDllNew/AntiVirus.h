#pragma once
#ifndef GETANTIVIRUSSOFTINFO_H_H_H
#define GETANTIVIRUSSOFTINFO_H_H_H

#include <windows.h>

int GainAdminPrivileges(char* strApp, UINT idd);

int SetRunAsAdmin();

int GetAntivirusSoft();

BOOL IsAdministratorUser();

int IsUACEnable(int iCpuBits, int* iEnableLUA, int* iConsentPromptBehaviorAdmin, int* iPromptOnSecureDesktop);

BOOL IsUserAdminGroup();

BOOL IsAdminProcess(UINT PID);

BOOL IsAdministrator();

bool IsProcessRunAsAdmin();


#endif
