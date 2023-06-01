#pragma once
#ifndef SOUNDRECORD_H_H_H
#define SOUNDRECORD_H_H_H

#include <windows.h>

#define IN_BUFFERS_NUMBER				4           //�����������ݻ�����
#define SOUND_RECORD_CHANNELS_NUMBER	1
#define SOUND_RECORD_SAMPLES_PER_SEC	8000		//44100
#define SOUND_RECORD_BITS_PER_SAMPLE	8

#pragma pack(1)
typedef struct soundInDataAndControls
{
	WAVEHDR buffers[IN_BUFFERS_NUMBER];  //�����������ݻ�����
	int recordingStatus;                 //¼��״̬
	HWAVEIN inWaveDevice;
	int buffers_left;                    //�����ڴ���Ŀ
}SoundThreadInfo;
#pragma pack()


#define REC_STATUS_STARTING_RECORD		1
#define REC_STATUS_RECORDING			2
#define REC_STATUS_ALL_DONE				-2			//¼�����
#define REC_STATUS_FINISHING			-1			//�������
#define REC_STATUS_FAIL					-3
#define SIZE_OF_WAVEHDR					32          //waveͷ��С

int __stdcall SoundRecord(unsigned long DelayTime);

#endif