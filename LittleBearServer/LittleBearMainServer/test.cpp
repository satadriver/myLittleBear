#include "mysqlOper.h"
#include "FileReadLine.h"
#include "JsonSplit.h"
#include "PublicFunc.h"
#include "Public.h"
#include "filehelper.h"
#include "jsonParser.h"


void mytest() {
#ifdef USE_MYSQL
	MySql* mysql = new MySql();
#endif
	char* file = 0;
	int filesize = 0;
	int iret = 0;

	filesize = FileHelper::fileReader("installedApps.json", &file, &filesize);

	string data = string(file, filesize);

	delete file;

	vector <string> ret = JsonParser::getArrayFromEnter(data);
	if (ret.size() > 0)
	{
		for (unsigned int i = 0; i < ret.size(); i++)
		{
			char* result = 0;
			int iret = GBKToUTF8(ret[i].c_str(), &result);
			string name = "";
			string ver = "";
			string date = "";
			string path = "";
			string unpath = "";
			string publisher = "";
			JsonSplit::splitInstallAppsOld(result, name, ver, path, unpath, publisher, date);

			delete result;
		}
	}
	else {
		ret = JsonParser::getJsonFromArray(data);
		for (unsigned int i = 0; i < ret.size(); i++)
		{
			char* result = 0;
			int iret = GBKToUTF8(ret[i].c_str(), &result);
			string name = "";
			string ver = "";
			string date = "";
			string path = "";
			string unpath = "";
			string publisher = "";
			JsonSplit::splitInstallApps(result, name, ver, path, unpath, publisher, date);

			replaceSlashAndEnterAndQuot(path);
			replaceSlashAndEnterAndQuot(unpath);
			delete result;
		}
	}
#ifdef USE_MYSQL
	delete mysql;
#endif
	return;
		}