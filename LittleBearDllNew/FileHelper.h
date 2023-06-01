
#pragma once

#ifndef FILEHELPER_H_H_H

#define FILEHELPER_H_H_H

#include <iostream>
#include "windows.h"

using namespace std;

LPVOID FileMapping(const WCHAR* name, DWORD size);


class FileHelper {

	FileHelper() {};
	~FileHelper() {};

public:
	static int CheckPathExist(string path);

	static int CheckFileExist(string filename);

	static int fileReader(char* filename, char** data, int* datasize);

	static int FileHelper::fileWriter(char* filename, char* data, int datasize, int append);
};

#endif