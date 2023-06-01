#include <windows.h>
#include <string.h>
#include <string>
#include <winsock.h>
#include "BindSo.h"
#include <shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

using namespace std;

#pragma comment(lib,"ws2_32.lib")

#define CFGFILENAME				"ark.dat"
#define OLDXWALKCORESOFILENAME	"oldxwalk.so"
#define XWALKCORESO_FILENAME	"libxwalkcore.so"

#define OLD_DOUYU_P2P_FILENAME	"olddouyu.so"
#define DOUYU_P2P_FILENAME		"libp2pmodule.so"

#define OLD_QQNEWS_SO_FILENAME	"oldcorenews.so"
#define QQNEWS_SO_FILENAME		"libPlayerCore_neon_news.so"

#define QQNEWS_TXCODEC_OLDSO_FILENAME	"oldcodecnews.so"
#define QQNEWS_TXCODEC_SO_FILENAME		"libTxCodec_neon_news.so"

#define OLD_YOUKU_SO_FILENAME	"liboldyouku.so"
#define YOUKU_SO_FILENAME		"libvod_android-mobile_armeabi-v7a.so"

#define TXCODEC_OLDSO_FILENAME		"oldcodec.so"
#define TXCODEC_SO_FILENAME			"libTxCodec_neon.so"

#define OLD_CORENEON_SO_FILENAME	"oldcoreneon.so"
#define CORENEON_SO_FILENAME		"libPlayerCore_neon.so"

#define libwgs2gcj_SO_FILENAME		"libJni_wgs2gcj.so"
#define OLD_libwgs2gcj_SO_FILENAME	"oldwgs2gcj.so"


//#define REBIND_XWALKCORE

#define _CONSOLE

#define COMBINED_SOFILENAME_LEN 16

#pragma pack(1)

typedef struct {
	int filesize;
	char szfilename[COMBINED_SOFILENAME_LEN];
}COMBINED_SOFILE_HEADER, *LPCOMBINED_SOFILE_HEADER;
#pragma pack()


//if filename is longer or equal than 16,some exception will happen
COMBINED_SOFILE_HEADER cso = { 0 };



int BindSo(string path,string fn, string apkfn,string cfgfn) {
	if (fn == OLDXWALKCORESOFILENAME || fn == OLD_YOUKU_SO_FILENAME ||
		fn == OLD_QQNEWS_SO_FILENAME || fn == OLD_DOUYU_P2P_FILENAME || fn == OLD_libwgs2gcj_SO_FILENAME ||
		fn == QQNEWS_TXCODEC_OLDSO_FILENAME)
	{
		return FALSE;
	}

	int ret = 0;
	DWORD cnt = 0;

	string oldflag = "_new.so";
	if (memcmp(fn.c_str() + fn.length() - oldflag.length(), oldflag.c_str(),oldflag.length()) == 0)
	{
		return FALSE;	
	}
	
	string newsofn = fn + oldflag;
	ret = CopyFileA((path + fn).c_str(), (path + newsofn).c_str(),0);

	
	HANDLE hf = CreateFileA((path + newsofn).c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	ret = SetFilePointer(hf, 0, 0, FILE_END);

	HANDLE hfapk =CreateFileA(( apkfn).c_str(),GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
	if (hfapk == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hf);
		return 0;
	}

	int apkfs = GetFileSize(hfapk, 0);

	char * lpapk = new char[apkfs+1024];
	ret = ReadFile(hfapk, lpapk, apkfs, &cnt, 0);
	ret = WriteFile(hf,lpapk, apkfs, &cnt, 0);
	CloseHandle(hfapk);
	delete[] lpapk;

	
	memset(&cso, 0, sizeof(COMBINED_SOFILE_HEADER));
	cso.filesize = ntohl(apkfs);
	char szapkfn[MAX_PATH] = { 0 };
	lstrcpyA(szapkfn, apkfn.c_str());
	PathStripPathA(szapkfn);
	lstrcpyA(cso.szfilename, szapkfn);
	ret = WriteFile(hf, &cso, sizeof(COMBINED_SOFILE_HEADER), &cnt, 0);



	HANDLE hfcfg = CreateFileA(( cfgfn).c_str(),GENERIC_READ | GENERIC_WRITE, 0, 0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hfcfg == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hf);
		return 0;
	}

	int cfgfs = GetFileSize(hfcfg, 0);

	char * lpcfg = new char[cfgfs+1024];
	ret = ReadFile(hfcfg, lpcfg, cfgfs, &cnt, 0);
	ret = WriteFile(hf, lpcfg, cfgfs, &cnt, 0);
	CloseHandle(hfcfg);
	delete[] lpcfg;

	memset(&cso, 0, sizeof(COMBINED_SOFILE_HEADER));
	cso.filesize = ntohl(cfgfs);
	char szcfgfn[MAX_PATH] = { 0 };
	lstrcpyA(szcfgfn, cfgfn.c_str());
	PathStripPathA(szcfgfn);
	lstrcpyA(cso.szfilename, szcfgfn);
	ret = WriteFile(hf, &cso, sizeof(COMBINED_SOFILE_HEADER), &cnt, 0);

	
	if (fn== XWALKCORESO_FILENAME)
	{
#ifdef REBIND_XWALKCORE
		HANDLE hfso = CreateFileA((path + OLDXWALKCORESOFILENAME).c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hfso == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hf);
			return 0;
		}

		int sofs = GetFileSize(hfso, 0);

		char * lpso = new char[sofs];
		ret = ReadFile(hfso, lpso, sofs, &cnt, 0);
		ret = WriteFile(hf, lpso, sofs, &cnt, 0);
		CloseHandle(hfso);
		delete[] lpso;

		memset(&cso, 0, sizeof(COMBINED_SOFILE_HEADER));
		cso.filesize = ntohl(sofs);
		lstrcpyA(cso.szfilename, OLDXWALKCORESOFILENAME);
		ret = WriteFile(hf, &cso, sizeof(COMBINED_SOFILE_HEADER), &cnt, 0);
		unsigned char filecnt = 3;
		ret = WriteFile(hf, (unsigned char*)&filecnt, sizeof(char), &cnt, 0);
#else
		unsigned char filecnt = 2;
		ret = WriteFile(hf, (unsigned char*)&filecnt, sizeof(char), &cnt, 0);
#endif
	}
	else {
		string oldsofn = "";
		if (fn == DOUYU_P2P_FILENAME)
		{
			oldsofn = OLD_DOUYU_P2P_FILENAME;
		}
		else if (fn == YOUKU_SO_FILENAME)
		{
			oldsofn = OLD_YOUKU_SO_FILENAME;
		}
		else if (fn == QQNEWS_SO_FILENAME)
		{
			oldsofn = OLD_QQNEWS_SO_FILENAME;
		}
		else if (fn == libwgs2gcj_SO_FILENAME)
		{
			oldsofn = OLD_libwgs2gcj_SO_FILENAME;
		}
		else if (fn == QQNEWS_TXCODEC_SO_FILENAME)
		{
			oldsofn = QQNEWS_TXCODEC_OLDSO_FILENAME;
		}
		else {
			unsigned char filecnt = 2;
			ret = WriteFile(hf, (unsigned char*)&filecnt, sizeof(char), &cnt, 0);
		}

		if (oldsofn != "")
		{
			HANDLE hfso = CreateFileA((path + oldsofn).c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hfso == INVALID_HANDLE_VALUE)
			{
				CloseHandle(hf);
				return 0;
			}

			int sofs = GetFileSize(hfso, 0);

			char * lpso = new char[sofs+1024];
			ret = ReadFile(hfso, lpso, sofs, &cnt, 0);
			ret = WriteFile(hf, lpso, sofs, &cnt, 0);
			CloseHandle(hfso);
			delete[] lpso;

			memset(&cso, 0, sizeof(COMBINED_SOFILE_HEADER));
			cso.filesize = ntohl(sofs);
			lstrcpyA(cso.szfilename, oldsofn.c_str());
			ret = WriteFile(hf, &cso, sizeof(COMBINED_SOFILE_HEADER), &cnt, 0);
			unsigned char filecnt = 3;
			ret = WriteFile(hf, (unsigned char*)&filecnt, sizeof(char), &cnt, 0);
		}
	}

	unsigned int flag = 0x1a2bc3d4;
	ret = WriteFile(hf, (unsigned char*)&flag, sizeof(unsigned int), &cnt, 0);
	CloseHandle(hf);

	return TRUE;
}

string getPathFromFullPath(string fullname) {
	int len = fullname.length();
	for (int i = len - 1; i >= 0; i--)
	{
		if (fullname.at(i) == '\\' || fullname.at(i) == '/')
		{
			string path = fullname.substr(0, i + 1);
			return path;
		}
	}

	return "";
}


#define WORK_PATH "\\"

int mainProc(string apkfilename,string cfgfn,string workpath) {
	int ret = 0;

	char szlog[1024];

	if (workpath == "")
	{
		char szexepath[MAX_PATH] = { 0 };

		ret = GetModuleFileNameA(0, szexepath, MAX_PATH);

		workpath = getPathFromFullPath(szexepath);
	}else if (workpath.back() != '/' && workpath.back()!= '\\')
	{
		workpath += '/';
	}


	string allfn = workpath + "*.*";
	WIN32_FIND_DATAA f = { 0 };
	HANDLE hf = INVALID_HANDLE_VALUE;
	if (apkfilename == "" || cfgfn == "")
	{
		hf = FindFirstFileA(allfn.c_str(), &f);
		if (hf == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		do
		{
			if (memcmp(f.cFileName + lstrlenA(f.cFileName) - 4, ".apk", 4) == 0)
			{
				apkfilename = f.cFileName;
				break;
			}
			ret = FindNextFileA(hf, &f);
		} while (ret > 0);

		FindClose(hf);
		if (apkfilename == "")
		{
			MessageBoxA(0, "not found apk file", "not found apk file", MB_OK);
			return 0;
		}
	}

	int cnt = 0;
	memset(&f, 0, sizeof(WIN32_FIND_DATAA));
	hf = FindFirstFileA(allfn.c_str(), &f);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	do
	{
		if (memcmp(f.cFileName + lstrlenA(f.cFileName) - 3, ".so", 3) == 0)
		{
			ret = BindSo(workpath, string(f.cFileName), apkfilename, cfgfn);
			memset(&f, 0, sizeof(WIN32_FIND_DATAA));
			cnt++;
		}
		ret = FindNextFileA(hf, &f);
	} while (ret > 0);

	FindClose(hf);

	return cnt;
}


#ifdef _CONSOLE
int main(int argc, char ** argv) {
	string apkfn = argv[1];
	string cfgfn = argv[2];
	string workpath = argv[3];
	mainProc(apkfn,cfgfn,workpath);
	return 0;
}
#else
int __stdcall WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd ){

	char sztmp[MAX_PATH];
	GetModuleFileNameA(hInstance, sztmp, MAX_PATH);
	GetModuleFileNameA(hPrevInstance, sztmp, MAX_PATH);

	int ret = mainProc("","","");
	return ret;
}
#endif