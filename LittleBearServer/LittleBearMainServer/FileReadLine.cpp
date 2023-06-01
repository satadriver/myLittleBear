#include "FileReadLine.h"
#include <windows.h>


FileReadLine::FileReadLine(string filename) {
	mfp = fopen(filename.c_str(), "rb");

}


FileReadLine::~FileReadLine() {
	fclose(mfp);
}


string FileReadLine::readline() {

	char buf[4096];
	char* ret = fgets(buf, 4096, mfp);
	if (ret <= 0)
	{
		return "";
	}

	return string(buf);
}


string FileReadLine::getmac(unsigned char* cMAC) {
	char szbuf[256] = { 0 };
	wsprintfA(szbuf, "%02X_%02X_%02X_%02X_%02X_%02X", cMAC[0], cMAC[1], cMAC[2], cMAC[3], cMAC[4], cMAC[5]);
	return string(szbuf);
}