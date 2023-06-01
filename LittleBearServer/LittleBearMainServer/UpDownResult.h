#pragma once

#include <iostream>

using namespace std;

class UploadDownloadResult {
public:
	static int WriteResult(unsigned char * mac, string filename,int flag);

	static string UploadDownloadResult::getMacString(unsigned char * mac);
};