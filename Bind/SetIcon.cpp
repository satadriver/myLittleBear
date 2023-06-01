

#include <windows.h>
#include <string>

using     namespace    std;

#pragma pack(1)

struct TIconHeader
{
	WORD idReserved;
	WORD idType;
	WORD idCount;            // Ŀ¼��
};


struct TResDirHeader
{
	BYTE bWidth;             // ͼ���ȣ�������Ϊ��λ��һ���ֽ�
	BYTE bHeight;            // ͼ��߶ȣ�������Ϊ��λ��һ���ֽ�
	BYTE bColorCount;        // ͼ���е���ɫ���������>=8bpp��λͼ��Ϊ0��
	BYTE bReserved;          // �����ֱ�����0
	WORD wPlanes;            // ���豸˵��λ��������ֵ�����Ǳ���Ϊ1
	WORD wBitCount;          // ÿ������ռλ��
	DWORD lBYTEsInRes;       // ÿ����Դ��ռ�ֽ���
	DWORD lImageOffset;      // ͼ�����ݣ�iconimage�����ƫ��λ��
};


typedef struct TIconResDirGrp
{
	TIconHeader    idHeader;
	TResDirHeader idEntries[1];

} *PIconResDirGrp;
#pragma pack()

// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=

WORD MakeLangID();

// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=

WORD MakeLangID()
{
	return  (SUBLANG_ENGLISH_US << 10) | LANG_ENGLISH;
}

// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=

void ChangeIcon(const char* szFileName,const char* szIconFile)
{

	int i, FileGrpSize;
	DWORD dwFileSize, dwBytesRead;
	void * filemem, *p;
	PIconResDirGrp FileGrp;
	HANDLE hFile, hUpdateRes;
	char*   szResName = "AnyIcon";
	WCHAR   szFileNameWCHAR[MAX_PATH] = { 0 };
	WCHAR   szIconFileWCHAR[MAX_PATH] = { 0 };
	WCHAR   szResNameWCHAR[MAX_PATH] = { 0 };

	MultiByteToWideChar(CP_ACP, 0, szFileName, -1,
		szFileNameWCHAR, sizeof(szFileNameWCHAR));

	MultiByteToWideChar(CP_ACP, 0, szIconFile, -1,
		szIconFileWCHAR, sizeof(szIconFileWCHAR));

	MultiByteToWideChar(CP_ACP, 0, szResName, -1,
		szResNameWCHAR, sizeof(szResNameWCHAR));

	// open the icon file    
	hFile = CreateFile(szIconFileWCHAR, GENERIC_READ | GENERIC_WRITE,

		FILE_SHARE_READ | FILE_SHARE_WRITE,

		NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		//MessageBox(0, L"Failed open Icon File!", NULL, 0);
		return;
	}

	// get the file size     
	dwFileSize = GetFileSize(hFile, NULL);

	filemem = malloc(dwFileSize);

	// read file to memory       
	ReadFile(hFile, filemem, dwFileSize, &dwBytesRead, NULL);

	CloseHandle(hFile);


	// assume the TIconResDirGrp STRUCT     ��
	FileGrp = PIconResDirGrp(filemem);

	// get Icon_Header size    
	FileGrpSize = sizeof(TIconResDirGrp) + (FileGrp->idHeader.idCount - 1)
		* sizeof(TResDirHeader);

	// begin to change the resource    
	hUpdateRes = BeginUpdateResource(szFileNameWCHAR, false);

	// change all frames'resource     
	for (i = 0; i < FileGrp->idHeader.idCount; i++)
	{

		p = (void *)((DWORD)filemem + FileGrp->idEntries[i].lImageOffset);

		// change every frame         
		UpdateResource(hUpdateRes, RT_ICON,
			MAKEINTRESOURCE(FileGrp->idEntries[i].lImageOffset)
			, MakeLangID(), p, FileGrp->idEntries[i].lBYTEsInRes);
	}

	// update header information  
	UpdateResource(hUpdateRes, RT_GROUP_ICON, szResNameWCHAR, MakeLangID(),
		FileGrp, FileGrpSize);

	EndUpdateResource(hUpdateRes, false);
	free(filemem);
}