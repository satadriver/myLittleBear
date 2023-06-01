#include "JsonSplit.h"
#include "jsonParser.h"


int JsonSplit::splitRunningApps(string data, string & pname, string & pid) {
	pname = JsonParser::getStringValue(data, "process");
	pid = JsonParser::getStringValue(data, "ID");
	return 0;
}

int JsonSplit::splitInstallApps(string data, string & name, string &ver, string &path, string &unpath, string &publisher, string &date) {
	name = JsonParser::getStringValue(data, "name");
	ver = JsonParser::getStringValue(data, "ver");
	path = JsonParser::getStringValue(data, "install");
	publisher = JsonParser::getStringValue(data, "publisher");
	date = JsonParser::getStringValue(data, "date");
	unpath = JsonParser::getStringValue(data, "uninstall");
	return 0;
}


int JsonSplit::splitInstallAppsOld(string data, string & name, string &ver, string &path, string &unpath, string &publisher, string &date) {
	name = JsonParser::getOtherValue(data, "name");
	ver = JsonParser::getOtherValue(data, "ver");
	path = JsonParser::getOtherValue(data, "install");
	publisher = JsonParser::getOtherValue(data, "publisher");
	date = JsonParser::getOtherValue(data, "data");
	unpath = JsonParser::getOtherValue(data, "uninstall");
	return 0;
}