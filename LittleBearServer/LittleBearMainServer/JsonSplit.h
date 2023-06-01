#pragma once

#include <iostream>

using namespace std;

class JsonSplit {
public:

	static int splitRunningApps(string data, string & name, string & id);
	static int splitInstallApps(string data, string & name, string &ver, string &path, string &unpath, string &publisher, string &date);

	static int JsonSplit::splitInstallAppsOld(string data, string & name, 
			string &ver, string &path, string &unpath, string &publisher, string &date);
};