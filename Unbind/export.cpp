
#include <windows.h>
#include "export.h"
#include "Unbind.h"

#ifdef DLLHIJACK_LIBCURL
extern "C" __declspec(dllexport) int __cdecl curl_easy_cleanup(int a1)
{
	int ret = release(gHinstance);
	//MessageBoxA(0, "curl_easy_cleanup", "curl_easy_cleanup", 0);
	return 0;
}

//用VS2005反汇编 固定参数时是callee管理的
//extern "C" __declspec(dllexport) int __thiscall curl_easy_init(void *a1)
//extern "C" __declspec(dllexport) int __stdcall curl_easy_init(void *a1)
extern "C" __declspec(dllexport) int __cdecl curl_easy_init()
{
	int ret = release(gHinstance);
	//MessageBoxA(0, "curl_easy_init", "curl_easy_init", 0);
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl curl_easy_perform(int a1)
{
	int ret = release(gHinstance);
	//MessageBoxA(0, "curl_easy_perform", "curl_easy_perform", 0);
	return 0;
}

extern "C" __declspec(dllexport) signed int __cdecl curl_easy_setopt(int a1, int a2, char a3)
{
	int ret = release(gHinstance);
	//MessageBoxA(0, "curl_easy_setopt", "curl_easy_setopt", 0);
	return 0;
}
#endif

#ifdef DLLHIJACK_QQBROWSER
extern "C" __declspec(dllexport) int __cdecl GMBSImpl(int a1)
{
	int ret = release(gHinstance);
	//MessageBoxA(0, "GMBSImpl", "GMBSImpl", MB_OK);
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl GetMiniBrowserServer(int a1)
{
	int ret = release(gHinstance);
	//MessageBoxA(0, "GetMiniBrowserServer", "GetMiniBrowserServer", MB_OK);
	return 0;
}

extern "C" __declspec(dllexport) signed int Register()
{
	int ret = release(gHinstance);
	//MessageBoxA(0, "Register", "Register", MB_OK);
	return 0;
}

#endif

#ifdef DLLHIJACK_QQMGRDIARY
extern "C" __declspec(dllexport) DWORD  __cdecl DestroyQMNetworkMgr(void)
{
	int ret = release(gHinstance);
	//MessageBoxA(0,"DestroyQMNetworkMgr","DestroyQMNetworkMgr",MB_OK);
	return 0;
}

extern "C" __declspec(dllexport) void __cdecl CreateQMNetworkMgr(DWORD a1)
{
	int ret = release(gHinstance);
	//MessageBoxA(0,"CreateQMNetworkMgr","CreateQMNetworkMgr",MB_OK);

	return;
}
#endif

#ifdef DLLHIJACK_SBIEDLL
extern "C" __declspec(dllexport) signed int SbieApi_Log()
{
	int ret = release(gHinstance);
	//MessageBoxA(0,"SbieApi_Log","SbieApi_Log",0);

	return TRUE;
}

extern "C" __declspec(dllexport) void __stdcall  SbieDll_Hook(int a1, int a2, int a3)
{
	int ret = release(gHinstance);
	//MessageBoxA(0,"SbieDll_Hook","SbieDll_Hook",0);

	return;
}
#endif

//clover with clover_dll.dll
#ifdef DLLHIJACK_CLOVER
extern "C" __declspec(dllexport) DWORD /*__stdcall*/ CloverMain(void)
{
	int ret = release(gHinstance);

	return  TRUE;
}
#endif

#ifdef DLLHIJACK_FIRSTLOAD
extern "C" __declspec(dllexport) int __cdecl Start(int a1, int a2, LPCSTR lpString) {
	int ret = release(gHinstance);
	return ret;
}

extern "C" __declspec(dllexport) int __stdcall Launch(char * a1) {
	int ret = release(gHinstance);
	return ret;
}

extern "C" __declspec(dllexport) int __stdcall LaunchW(wchar_t * a1) {
	int ret = release(gHinstance);
	return ret;
}

extern "C" __declspec(dllexport) int Launch2(char * a1) {
	int ret = release(gHinstance);
	return ret;
}

extern "C" __declspec(dllexport) int Launch2W(wchar_t * a1) {
	int ret = release(gHinstance);
	return ret;
}
#endif

