


#include <windows.h>
#include "zlib.h"
#include "zconf.h"
#pragma comment(lib,"zlib.lib")

#define WORK_PATH					"\\workpath\\"
#define CPP_SOURCE_FILE_NAME		"decryptfromdata.cpp"
#define SUBBYTE_KEY_FORMAT			"\r\n#define subbytekey %u\r\n\r\n"
#define EXEFILENAME_FORMAT			"\r\n#define exefilename \"%s\"\r\n\r\n"
#define DLLFILENAME_FORMAT			"\r\n#define dllfilename \"%s\"\r\n\r\n"
#define EXE_SEGMENT_NAME_FORMAT		"\r\n#define exesegment_%u \""
#define DLL_SEGMENT_NAME_FORMAT		"\r\n#define dllsegment_%u \""
#define KEY_SEGMENT_NAME_FORMAT		"\r\n#define cryptkey \"%s\"\r\n\r\n"
#define SEGMENT_NAME_END			"\"\r\n\r\n"
#define SEGMENT_INSERT_BEGIN		"\r\n#define segment_insert_begin\r\n\r\n"
#define SEGMENT_INSERT_END			"\r\n#define segment_insert_end\r\n\r\n"
#define MAX_VAR_BLOCK_SIZE			0x2000
#define MAX_SEGEMENT_CNT			128
#define CRYPT_KEY_SIZE				16
#define DEFAULT_SUBBYTE_VALUE		13



DWORD GetCryptKey(unsigned char * pKey)
{
	SYSTEMTIME stSystime = {0};
	GetSystemTime(&stSystime);

	DWORD dwTickCnt[CRYPT_KEY_SIZE/sizeof(DWORD)] = {0};
	for (int i= 0; i < CRYPT_KEY_SIZE/sizeof(DWORD); i ++)
	{
		dwTickCnt[i] = GetTickCount();
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




void CryptCharWithSubByte(unsigned char * pdata,int size,unsigned char subbyte){
	char * sztmpbuf = new char[size + 0x1000];

	int j = 0;
	for(int i = 0; i < size ; i ++)
	{
		//if (pdata[i] != '_' && pdata[i] != '-' && pdata[i] != '.')
		//{
			sztmpbuf[j] = pdata[i] - subbyte;
			if (sztmpbuf[j] == '\\')
			{
				j ++;
				sztmpbuf[j] = '\\';
			}else if (sztmpbuf[j] == '\"')
			{
				sztmpbuf[j] = '\\';
				j ++;

				sztmpbuf[j] = '\"';
			}else if (sztmpbuf[j] == '\'')
			{
				sztmpbuf[j] = '\\';
				j ++;

				sztmpbuf[j] = '\'';
			}
			j ++;
		//}else{
		//	sztmpbuf[j] = pdata[i];
		//	j ++;
		//}
	}

	memmove(pdata,sztmpbuf,j);
	*(pdata + j) = 0;
	delete []sztmpbuf;
}



int __stdcall WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd ){

	int ret = 0;
	DWORD dwcnt = 0;

	

	unsigned char digitalkey[CRYPT_KEY_SIZE];
	GetCryptKey(digitalkey);

	SYSTEMTIME systime = {0};
	GetSystemTime(&systime);
	int subbyte = systime.wSecond%DEFAULT_SUBBYTE_VALUE;
	if(subbyte == 0){
		subbyte = DEFAULT_SUBBYTE_VALUE;
	}

	WIN32_FIND_DATAA stfd = {0};
	char szallexefile[] = "*.exe";
	char szworkpath[MAX_PATH];
	ret = GetCurrentDirectoryA(MAX_PATH,szworkpath);
	lstrcatA(szworkpath,WORK_PATH);
	char szfindexefilename[MAX_PATH];
	lstrcpyA(szfindexefilename,szworkpath);
	lstrcatA(szfindexefilename,szallexefile);
	HANDLE hfind = FindFirstFileA(szfindexefilename,&stfd);
	if (hfind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	char szexefilefullname[MAX_PATH];
	lstrcpyA(szexefilefullname,szworkpath);
	lstrcatA(szexefilefullname,stfd.cFileName);
	char szexefilename[MAX_PATH];
	lstrcpyA(szexefilename,stfd.cFileName);
	FindClose(hfind);
	CryptCharWithSubByte((unsigned char*)szexefilename,lstrlenA(szexefilename),subbyte);

	char szalldllfile[] = "*.dll";
	char szfinddllfilename[MAX_PATH];
	lstrcpyA(szfinddllfilename,szworkpath);
	lstrcatA(szfinddllfilename,szalldllfile);
	hfind = FindFirstFileA(szfinddllfilename,&stfd);
	if (hfind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	char szdllfilefullname[MAX_PATH];
	lstrcpyA(szdllfilefullname,szworkpath);
	lstrcatA(szdllfilefullname,stfd.cFileName);
	char szdllfilename[MAX_PATH];
	lstrcpyA(szdllfilename,stfd.cFileName);
	FindClose(hfind);
	CryptCharWithSubByte((unsigned char*)szdllfilename,lstrlenA(szdllfilename),subbyte);


	HANDLE hfexe = CreateFileA(szexefilefullname,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hfexe == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	int fexesize = GetFileSize(hfexe,0);
	char * exebuf = new char [fexesize];
	ret = ReadFile(hfexe,exebuf,fexesize,&dwcnt,0);
	CloseHandle(hfexe);
	unsigned long exezipbufsize = fexesize*2;
	char * exezipbuf = new char[exezipbufsize];
	ret = compress((unsigned char*)exezipbuf,&exezipbufsize,(unsigned char*)exebuf,fexesize);
	delete []exebuf;
	if (ret)
	{
		return FALSE;
	}
	CryptData((unsigned char*)exezipbuf,exezipbufsize,digitalkey,CRYPT_KEY_SIZE);


	HANDLE hfdll = CreateFileA(szdllfilefullname,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hfdll == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	int fdllsize = GetFileSize(hfdll,0);
	char * dllbuf = new char [fdllsize];
	dwcnt = 0;
	ret = ReadFile(hfdll,dllbuf,fdllsize,&dwcnt,0);
	CloseHandle(hfdll);
	unsigned long dllzipbufsize = fdllsize*2;
	char * dllzipbuf = new char[dllzipbufsize];
	ret = compress((unsigned char*)dllzipbuf,&dllzipbufsize,(unsigned char*)dllbuf,fdllsize);
	delete []dllbuf;
	if (ret)
	{
		return FALSE;
	}
	CryptData((unsigned char*)dllzipbuf,dllzipbufsize,digitalkey,CRYPT_KEY_SIZE);


	char szcppfilename[MAX_PATH];
	lstrcpyA(szcppfilename,szworkpath);
	lstrcatA(szcppfilename,CPP_SOURCE_FILE_NAME);
	HANDLE hfcpp = CreateFileA(szcppfilename,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hfcpp == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int fcppsize = GetFileSize(hfcpp,0);
	char *cppfilebuf = new char [fcppsize + 0x1000];
	ret = ReadFile(hfcpp,cppfilebuf,fcppsize,&dwcnt,0);
	*(cppfilebuf + fcppsize) = 0;
	CloseHandle(hfcpp);
	char * lpseginsertbegin = strstr(cppfilebuf,SEGMENT_INSERT_BEGIN);
	char * lpseginsertend = strstr(cppfilebuf,SEGMENT_INSERT_END);
	if (lpseginsertbegin && lpseginsertend)
	{
		lpseginsertend = lpseginsertend + lstrlenA(SEGMENT_INSERT_END);
		int userfullen = fcppsize - (lpseginsertend - cppfilebuf);
		memmove(cppfilebuf,lpseginsertend,userfullen);
		*(cppfilebuf + userfullen) = 0;
		fcppsize = userfullen;
	}
	hfcpp = CreateFileA(szcppfilename,GENERIC_READ | GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	ret = SetFilePointer(hfcpp,0,0,FILE_BEGIN);
	ret = WriteFile(hfcpp,SEGMENT_INSERT_BEGIN,lstrlenA(SEGMENT_INSERT_BEGIN),&dwcnt,0);



	char szsubbytekey[MAX_PATH];
	int subbytekeylen = wsprintfA(szsubbytekey,SUBBYTE_KEY_FORMAT,subbyte);
	ret = WriteFile(hfcpp,szsubbytekey,subbytekeylen,&dwcnt,0);


	char asciikey[MAX_PATH] = {0};
	for(int j =0, k = 0 ; j < CRYPT_KEY_SIZE;  ){
		asciikey[k] = '\\';
		asciikey[k+1] = 'x';
		unsigned char tmpchar = digitalkey[j] >> 4;
		if (tmpchar >= 0 && tmpchar <= 9)
		{
			tmpchar += 0x30;
		}
		else if (tmpchar >= 10 && tmpchar <= 15)
		{
			tmpchar += 55;
		}
		else{
			MessageBoxA(0,"digital to ascii key error","crypt ascii key error",MB_OK);
			ExitProcess(0);
		}
		asciikey[k + 2] = tmpchar;

		tmpchar = digitalkey[j] & 0xf;
		if (tmpchar >= 0 && tmpchar <= 9)
		{
			tmpchar += 0x30;
		}
		else if (tmpchar >= 10 && tmpchar <= 15)
		{
			tmpchar += 55;
		}
		else{
			MessageBoxA(0,"digital to ascii key error","crypt ascii key error",MB_OK);
			ExitProcess(0);
		}
		asciikey[k + 3] = tmpchar;

		k += 4;
		j ++;
	}
	char szcryptkey[MAX_PATH];
	int cyrptkeylen = wsprintfA(szcryptkey,KEY_SEGMENT_NAME_FORMAT,asciikey);
	ret = WriteFile(hfcpp,szcryptkey,cyrptkeylen,&dwcnt,0);


	char szfilenamebuf[MAX_PATH];
	int filenamelen = wsprintfA(szfilenamebuf,EXEFILENAME_FORMAT,szexefilename);
	ret = WriteFile(hfcpp,szfilenamebuf,filenamelen,&dwcnt,0);
	filenamelen = wsprintfA(szfilenamebuf,DLLFILENAME_FORMAT,szdllfilename);
	ret = WriteFile(hfcpp,szfilenamebuf,filenamelen,&dwcnt,0);


	char szvarname[MAX_PATH];
	int segbufsize =  MAX_SEGEMENT_CNT * MAX_VAR_BLOCK_SIZE;
	char * segbuf = new char [segbufsize];
	int segbuflen = 0;
	char * exezipbufptr = exezipbuf;

	int blockcnt = exezipbufsize/MAX_VAR_BLOCK_SIZE;
	int blockmod = exezipbufsize%MAX_VAR_BLOCK_SIZE;
	int exeblockcnt = 0;
	for (exeblockcnt =0; exeblockcnt < blockcnt; exeblockcnt ++)
	{
		segbuflen = wsprintfA(szvarname,EXE_SEGMENT_NAME_FORMAT,exeblockcnt);
		memmove(segbuf,szvarname,segbuflen);

		unsigned char * lpascii = (unsigned char*)segbuf + segbuflen;
		unsigned char * lpdigit = (unsigned char*)exezipbufptr;
		for(int j =0, k = 0 ; j < MAX_VAR_BLOCK_SIZE;  ){
			lpascii[k] = '\\';
			lpascii[k+1] = 'x';
			unsigned char tmpchar = lpdigit[j] >> 4;
			if (tmpchar >= 0 && tmpchar <= 9)
			{
				tmpchar += 0x30;
			}
			else if (tmpchar >= 10 && tmpchar <= 15)
			{
				tmpchar += 55;
			}
			else{
				MessageBoxA(0,"exe segment error","exe segment error",MB_OK);
				ExitProcess(0);
			}
			lpascii[k + 2] = tmpchar;

			tmpchar = lpdigit[j] & 0xf;
			if (tmpchar >= 0 && tmpchar <= 9)
			{
				tmpchar += 0x30;
			}
			else if (tmpchar >= 10 && tmpchar <= 15)
			{
				tmpchar += 55;
			}
			else{
				MessageBoxA(0,"exe segment error","exe segment error",MB_OK);
				ExitProcess(0);
			}
			lpascii[k + 3] = tmpchar;

			k += 4;
			j ++;
		}

		segbuflen = segbuflen + (4*MAX_VAR_BLOCK_SIZE);
		memmove(segbuf + segbuflen,SEGMENT_NAME_END,lstrlenA(SEGMENT_NAME_END));
		segbuflen = segbuflen + lstrlenA(SEGMENT_NAME_END);
		ret = WriteFile(hfcpp,segbuf,segbuflen,&dwcnt,0);

		exezipbufptr += MAX_VAR_BLOCK_SIZE;
	}

	if(blockmod)
	{
		segbuflen = wsprintfA(szvarname,EXE_SEGMENT_NAME_FORMAT,exeblockcnt);
		memmove(segbuf,szvarname,segbuflen);

		unsigned char * lpascii = (unsigned char*)segbuf + segbuflen;
		unsigned char * lpdigit = (unsigned char*)exezipbufptr;
		
		for(int j =0, k = 0 ; j < blockmod; ){
			lpascii[k] = '\\';
			lpascii[k+1] = 'x';
			unsigned char tmpchar = lpdigit[j] >> 4;
			if (tmpchar >= 0 && tmpchar <= 9)
			{
				tmpchar += 0x30;
			}
			else if (tmpchar >= 10 && tmpchar <= 15)
			{
				tmpchar += 55;
			}
			else{
				MessageBoxA(0,"exe mod segment error","exe segment error",MB_OK);
				ExitProcess(0);
			}
			lpascii[k + 2] = tmpchar;

			tmpchar = lpdigit[j] & 0xf;
			if (tmpchar >= 0 && tmpchar <= 9)
			{
				tmpchar += 0x30;
			}
			else if (tmpchar >= 10 && tmpchar <= 15)
			{
				tmpchar += 55;
			}
			else{
				MessageBoxA(0,"exe mod segment error","exe segment error",MB_OK);
				ExitProcess(0);
			}
			lpascii[k + 3] = tmpchar;

			k += 4;
			j ++;
		}

		segbuflen = segbuflen + (4*blockmod);
		memmove(segbuf + segbuflen,SEGMENT_NAME_END,lstrlenA(SEGMENT_NAME_END));
		segbuflen = segbuflen + lstrlenA(SEGMENT_NAME_END);
		ret = WriteFile(hfcpp,segbuf,segbuflen,&dwcnt,0);
		exezipbufptr += blockmod;
	}
	delete []exezipbuf;
	


	char * dllzipbufptr = dllzipbuf;
	blockcnt = dllzipbufsize/MAX_VAR_BLOCK_SIZE;
	blockmod = dllzipbufsize%MAX_VAR_BLOCK_SIZE;
	int idllblock =0;
	for ( idllblock =0; idllblock < blockcnt; idllblock++)
	{
		segbuflen = wsprintfA(szvarname,DLL_SEGMENT_NAME_FORMAT,idllblock);
		memmove(segbuf,szvarname,segbuflen);

		unsigned char * lpascii = (unsigned char*)segbuf + segbuflen;
		unsigned char * lpdigit = (unsigned char*)dllzipbufptr;
		for(int j =0, k = 0 ; j < MAX_VAR_BLOCK_SIZE;  ){
			lpascii[k] = '\\';
			lpascii[k+1] = 'x';
			unsigned char tmpchar = lpdigit[j] >> 4;
			if (tmpchar >= 0 && tmpchar <= 9)
			{
				tmpchar += 0x30;
			}
			else if (tmpchar >= 10 && tmpchar <= 15)
			{
				tmpchar += 55;
			}
			else{
				MessageBoxA(0,"dll segment error","exe segment error",MB_OK);
				ExitProcess(0);
			}
			lpascii[k + 2] = tmpchar;

			tmpchar = lpdigit[j] & 0xf;
			if (tmpchar >= 0 && tmpchar <= 9)
			{
				tmpchar += 0x30;
			}
			else if (tmpchar >= 10 && tmpchar <= 15)
			{
				tmpchar += 55;
			}
			else{
				MessageBoxA(0,"dll segment error","exe segment error",MB_OK);
				ExitProcess(0);
			}
			lpascii[k + 3] = tmpchar;

			k += 4;
			j ++;
		}

		segbuflen = segbuflen + (4*MAX_VAR_BLOCK_SIZE);
		memmove(segbuf + segbuflen,SEGMENT_NAME_END,lstrlenA(SEGMENT_NAME_END));
		segbuflen = segbuflen + lstrlenA(SEGMENT_NAME_END);
		ret = WriteFile(hfcpp,segbuf,segbuflen,&dwcnt,0);

		dllzipbufptr += MAX_VAR_BLOCK_SIZE;
	}

	if(blockmod)
	{
		segbuflen = wsprintfA(szvarname,DLL_SEGMENT_NAME_FORMAT,idllblock);
		memmove(segbuf,szvarname,segbuflen);

		unsigned char * lpascii = (unsigned char*)segbuf + segbuflen;
		unsigned char * lpdigit = (unsigned char*)dllzipbufptr;
		for(int j =0,k = 0 ; j < blockmod;  ){
			lpascii[k] = '\\';
			lpascii[k+1] = 'x';
			unsigned char tmpchar = lpdigit[j] >> 4;
			if (tmpchar >= 0 && tmpchar <= 9)
			{
				tmpchar += 0x30;
			}
			else if (tmpchar >= 10 && tmpchar <= 15)
			{
				tmpchar += 55;
			}
			else{
				MessageBoxA(0,"dll mod segment error","exe segment error",MB_OK);
				ExitProcess(0);
			}
			lpascii[k + 2] = tmpchar;

			tmpchar = lpdigit[j] & 0xf;
			if (tmpchar >= 0 && tmpchar <= 9)
			{
				tmpchar += 0x30;
			}
			else if (tmpchar >= 10 && tmpchar <= 15)
			{
				tmpchar += 55;
			}
			else{
				MessageBoxA(0,"dll mod segment error","exe segment error",MB_OK);
				ExitProcess(0);
			}
			lpascii[k + 3] = tmpchar;

			k += 4;
			j ++;
		}

		segbuflen = segbuflen + (4*blockmod);
		memmove(segbuf + segbuflen,SEGMENT_NAME_END,lstrlenA(SEGMENT_NAME_END));
		segbuflen = segbuflen + lstrlenA(SEGMENT_NAME_END);
		ret = WriteFile(hfcpp,segbuf,segbuflen,&dwcnt,0);
		dllzipbufptr += blockmod;
	}
	delete []dllzipbuf;
	delete []segbuf;

	ret = WriteFile(hfcpp,SEGMENT_INSERT_END,lstrlenA(SEGMENT_INSERT_END),&dwcnt,0);

	ret = WriteFile(hfcpp,cppfilebuf,fcppsize,&dwcnt,0);
	CloseHandle(hfcpp);
	delete [] cppfilebuf;

	ExitProcess(0);
	return TRUE;
}
