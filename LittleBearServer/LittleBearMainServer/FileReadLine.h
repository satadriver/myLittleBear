#pragma once
#include <iostream>

using namespace std;

class FileReadLine {
public:
	~FileReadLine();
	FileReadLine(string filename);
	string readline();
	static string FileReadLine::getmac(unsigned char * cMAC);

private:
	FILE * mfp;
};