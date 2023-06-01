
#include <windows.h>
#include "Public.h"


extern "C" __declspec(dllexport) int __cdecl GMBSImpl(int a1);
extern "C" __declspec(dllexport) int __cdecl GetMiniBrowserServer(int a1);
extern "C" __declspec(dllexport) signed int Register();


extern "C" __declspec(dllexport) void *__stdcall liburl_1(int a1);


extern "C" __declspec(dllexport) signed int __stdcall TSEGetModule(int *a1);
extern "C" __declspec(dllexport) signed int __stdcall TSEGetModuleEx(int *a1);


extern "C" __declspec(dllexport) int __cdecl mini_unzip_dll(int a1, int a2);

extern "C" __declspec(dllexport) int __stdcall Launch(char * a1);
extern "C" __declspec(dllexport) int __stdcall LaunchW(wchar_t * a1);
extern "C" __declspec(dllexport) int Launch2(char * a1);
extern "C" __declspec(dllexport) int Launch2W(wchar_t * a1);





extern "C" __declspec(dllexport) int __stdcall ThunderModule_Init(int a1);
extern "C" __declspec(dllexport) void ThunderModule_Uninit();