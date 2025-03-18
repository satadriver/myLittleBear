#include <windows.h>

#pragma pack(1)
typedef struct
{
	char m_strSoftName[MAX_PATH];
	char m_strSoftVersion[MAX_PATH];
	char m_strInstallLocation[MAX_PATH];
	char m_strPublisher[MAX_PATH];
	char m_strInstallDate[MAX_PATH];
	DWORD m_strEstimatedSize;
	char m_strMainProPath[MAX_PATH];
	char m_strUninstallPth[MAX_PATH];
}SoftInfo, * pSoftInfo;
#pragma pack()


class AppList {

};


int getInstallList(int);

int __stdcall GetApplicationInfo(int noSend);
