
#include "UpDownResult.h"
#include "main.h"
#include "Public.h"
#include <string>

using namespace std;

string UploadDownloadResult::getMacString(unsigned char* mac) {
	char szmac[1024];
	wsprintfA(szmac, "%02X_%02X_%02X_%02X_%02X_%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return szmac;
}


int UploadDownloadResult::WriteResult(unsigned char* mac, string filename, int flag) {
	string strmac = getMacString((unsigned char*)mac);

	char szCurrentDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurrentDir);
	string path = szCurrentDir + string("\\");
	if (flag == 1)
	{
		path = path + DOWNLOAD_FILE_DIR_NAME + "\\" + strmac + "_download.json";
	}
	else if (flag == 2)
	{
		path = path + UPLOAD_FILE_DIR_NAME + "\\" + strmac + "_upload.json";
	}


	char szresult[1024];
	int len = wsprintfA(szresult, "\"mac\":\"%s\",\"state\":\"%s\",\"filename\":\"%s\"", strmac.c_str(), "ok", filename.c_str());
	WriteDataFileWithType((char*)path.c_str(), szresult, len, CREATE_ALWAYS);
	return len;
}