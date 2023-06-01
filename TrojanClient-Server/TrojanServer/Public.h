

#define SERVER_PORT				65535
#define IP_CONFIG_FILE			"ip.txt"
//#define EXE_FILE_NAME			"winsecurity.exe"
//#define DLL_FILE_NAME			"QMNetworkMgr.dll"
#define COMMAND_BUF_SIZE		4096
#define MAX_LISTEN_COUNT		16
#define CRYPT_KEY_SIZE			16
#define LOG_FILENAME			"log.txt"
#define WHITELIST_FOLDER		"whitelist"

#define CMD_SENDWHITELISTANDDLL		1


// typedef struct{
// 	int totoalsize;
// 	int cmd;
// 	int exenamelen;
// 
// }WHITELISTANDDLLHEADER,LPWHITELISTANDDLLHEADER;

int WriteLogFile(char * pLog);

int GenerateCryptKey(unsigned char * key);
int CryptData(char * lpdata,int len,unsigned char * key);
int GetNameFromFullName(char* strFullName,char * strDst);
int GetPathFromFullName(char* strFullName,char * strDst);