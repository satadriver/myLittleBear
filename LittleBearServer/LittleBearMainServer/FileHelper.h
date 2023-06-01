#pragma once


#include <windows.h>
#include <iostream>

using namespace std;

class FileHelper {
public:
	static int FileHelper::fileReader(string filename, char** lpbuf, int* bufsize);
	static int FileHelper::fileWriter(string filename, const char* lpdate, int datesize);
	static int FileHelper::fileWriter(string filename, const char* lpdate, int datesize, int cover);
	int CheckPathExist(string path);
};
