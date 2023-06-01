

#include <windows.h>
#include <string>

using     namespace    std;

#pragma pack(1)

struct TIconHeader
{
	WORD idReserved;
	WORD idType;
	WORD idCount;            // 目录数
};


struct TResDirHeader
{
	BYTE bWidth;             // 图像宽度，以象素为单位。一个字节
	BYTE bHeight;            // 图像高度，以象素为单位。一个字节
	BYTE bColorCount;        // 图像中的颜色数（如果是>=8bpp的位图则为0）
	BYTE bReserved;          // 保留字必须是0
	WORD wPlanes;            // 标设备说明位面数，其值将总是被设为1
	WORD wBitCount;          // 每象素所占位数
	DWORD lBYTEsInRes;       // 每份资源所占字节数
	DWORD lImageOffset;      // 图像数据（iconimage）起点偏移位置
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


	// assume the TIconResDirGrp STRUCT     　
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