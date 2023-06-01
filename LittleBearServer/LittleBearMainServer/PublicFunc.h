#pragma once

#include <windows.h>
#include "main.h"
#include <iostream>

using namespace std;

class PublicFunction {
public:
	static int autorun();

	static int addService(string path, string params);
};

DWORD Get2CommandParams(char* szParam, char szCommands[2][CMD_BUFFER_SIZE], char* split);

int CheckIfDataPathExist(char* strDataPath);

string makeJsonFromTxt(char* utf);

int checkAccountName(string acc);